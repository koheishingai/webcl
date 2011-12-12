/*
 * Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided the following conditions
 * are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
 * CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
 * ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
 * NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// local OpenCL info
//
var platform_ids;                           // array of OpenCL platform ids
var platform_id;                            // OpenCL platform id
var device_ids;                             // array of OpenCL device ids
var device_id;                              // OpenCL device id
var context;                                // OpenCL context
var queue;                                  // OpenCL command queue
var program;                                // OpenCL program
var kernel;                                 // OpenCL kernel

var initPosBuffer;                          // OpenCL buffer
var curPosBuffer;                           // OpenCL buffer created from GL VBO
var curNorBuffer;                           // OpenCL buffer created from GL VBO


var globalWorkSize = new Int32Array(2);
var localWorkSize = new Int32Array(2);

function getKernel(id) {
	var kernelScript = document.getElementById(id);
	if(kernelScript === null || kernelScript.type !== "x-kernel")
		return null;

	return kernelScript.firstChild.textContent;
}

function InitCL() {

	try  {
		if(typeof(WebCLComputeContext) === "undefined") {
			console.error("WebCLComputeContext is yet to be defined");
			return null;
		}

		var cl = new WebCLComputeContext();

		if(cl === null) {
			console.error("Failed to create WebCL context");
			return null;
		}

		// Select a compute device
		//
		platform_ids = cl.getPlatformIDs();

		if(platform_ids.length === 0) {
			console.error("No platforms available");
			return null;
		}
		platform_id = platform_ids[0];

		// Select a compute device
		//
		device_ids = platform_id.getDeviceIDs(cl.DEVICE_TYPE_GPU);
		if(device_ids.length === 0) {
			console.error("No devices available");
			return null;
		}
		device_id = device_ids[0];

		// Create a compute context
		//
		//context = cl.createContext(null, device_id, null, null);
		context = cl.createSharedContext(cl.DEVICE_TYPE_GPU, null, null);

		// Create a command queue
		//
		queue = context.createCommandQueue(device_id, null);

		// Create the compute program from the source buffer
		//
		var kernelSource = getKernel("deform_kernel");
		if (kernelSource === null) {
			console.error("No kernel named: " + "deform_kernel");
			return null;
		}

		program = context.createProgramWithSource(kernelSource);

		// Build the program executable
		//
		program.buildProgram(null, null, null);

		// Create the compute kernel in the program we wish to run
		//
		kernel = program.createKernel("displace");

	}
	catch (e)
	{
		console.error("Deform Demo Failed ; Message: "+ e.message);
		test.showFailure();
	}

	return cl;
}

function divide_up(a, b)
{
	return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

function InitCLBuffers(cl) {

	try {

		if(cl === null) return;

		var bufferSize = userData.nVertices * NUM_VERTEX_COMPONENTS * Float32Array.BYTES_PER_ELEMENT;

		// Create CL working buffers
		//
		initPosBuffer = context.createBuffer(cl.MEM_WRITE_ONLY, bufferSize, null);
		if(initPosBuffer === null) {
			console.error("Failed to allocate device memory");
			return null;
		}

		// Create CL buffers from GL VBOs
		// (Initial load of positions is via gl.bufferData)
		//
		curPosBuffer = context.createFromGLBuffer(cl.MEM_READ_WRITE, userData.curPosVBO);
		if(curPosBuffer === null) {
			console.error("Failed to allocate device memory");
			return null;
		}

		curNorBuffer = context.createFromGLBuffer(cl.MEM_READ_WRITE, userData.curNorVBO);
		if(curNorBuffer === null) {
			console.error("Failed to allocate device memory");
			return null;
		}

		// Get the maximum work group size for executing the kernel on the device
		//
		var workGroupSize = kernel.getKernelWorkGroupInfo(device_id, cl.KERNEL_WORK_GROUP_SIZE);

		var groupSize = 4;
		var uiSplitCount = Math.ceil(Math.sqrt(userData.nVertices));
		var uiActive = (workGroupSize / groupSize);
		uiActive = uiActive < 1 ? 1 : uiActive;

		var uiQueued = workGroupSize / uiActive;

		console.log("workGroupSize: " + workGroupSize);
		console.log("uiSplitCount: " + uiSplitCount);
		console.log("uiActive: " + uiActive);
		console.log("uiQueued: " + uiQueued);

		localWorkSize[0] = workGroupSize/8; //uiActive;
		localWorkSize[1] = workGroupSize/64;//uiQueued;

		globalWorkSize[0] = workGroupSize/2; //uiSplitCount; //divide_up(uiSplitCount, uiActive) * uiActive;
		globalWorkSize[1] = workGroupSize/2; //uiSplitCount; //divide_up(uiSplitCount, uiQueued) * uiQueued;

		//localWorkSize[0] = 64; //uiActive;
		//localWorkSize[1] = 4; //uiQueued;

		//globalWorkSize[0] = 256; //uiSplitCount; //divide_up(uiSplitCount, uiActive) * uiActive;
		//globalWorkSize[1] = 256; //uiSplitCount; //divide_up(uiSplitCount, uiQueued) * uiQueued;

		console.log(localWorkSize[0]);
		console.log(localWorkSize[1]);
		console.log(globalWorkSize[0]);
		console.log(globalWorkSize[1]);

		// Initial load of initial position data
		//   
		queue.enqueueWriteBuffer(initPosBuffer, true, 0, bufferSize, userData.initPos, null);

		//queue.finish(null, null);
		queue.finish(finishCB,0);

	}
	catch (e)
	{
		console.error("Deform Demo Failed ; Message: "+ e.message);
		test.showFailure();
	}
}

function finishCB()
{
}

function SimulateCL(cl)
{
	try {
		if(cl === null) return;

		if(GLCL_SHARE_MODE) {
			queue.enqueueAcquireGLObjects(curPosBuffer, null);
			queue.enqueueAcquireGLObjects(curNorBuffer, null);
		}

		var dimx = 16;
		var dimy = 0; 

		kernel.setKernelArgGlobal(0, initPosBuffer);
		kernel.setKernelArgGlobal(1, curNorBuffer);
		kernel.setKernelArgGlobal(2, curPosBuffer);
		kernel.setKernelArg(3, dimx, cl.KERNEL_ARG_INT); 
		kernel.setKernelArg(4, dimy, cl.KERNEL_ARG_INT); 
		kernel.setKernelArg(5, userData.frequency, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(6, userData.amplitude, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(7, userData.phase, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(8, userData.lacunarity, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(9, userData.increment, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(10, userData.octaves, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(11, userData.roughness, cl.KERNEL_ARG_FLOAT);
		kernel.setKernelArg(12, userData.nVertices, cl.KERNEL_ARG_INT);

		queue.enqueueNDRangeKernel(kernel, 2, 0, globalWorkSize, localWorkSize, null);

		queue.finish(GetResults, cl);

	}
	catch (e)
	{
		console.error("Deform Demo Failed ; Message: "+ e.message);
		test.showFailure();
	}
}   

function GetResults(cl)
{ 
	try {		
		if(GLCL_SHARE_MODE) {
			queue.enqueueReleaseGLObjects(curPosBuffer, null);
			queue.enqueueReleaseGLObjects(curNorBuffer, null);
		}    
		else {
			var bufferSize = userData.nVertices * NUM_VERTEX_COMPONENTS * Float32Array.BYTES_PER_ELEMENT;
			queue.enqueueReadBuffer(curPosBuffer, true, 0, bufferSize, userData.curPos, null);
			queue.enqueueReadBuffer(curNorBuffer, true, 0, bufferSize, userData.curNor, null);
		}
	}
	catch (e)
	{
		console.error("Deform Demo Failed ; Message: "+ e.message);
		test.showFailure();
	}

	userData.phase += PHASE_DELTA;
}
