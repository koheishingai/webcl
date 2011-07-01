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

function WebCLComputeContext () {
	this.SUCCESS = 0;
	this.DEVICE_TYPE_GPU = 1;
	this.DEVICE_TYPE_CPU = 2;
	this.PROGRAM_BUILD_LOG = 3;
	this.MEM_READ_ONLY = 4;
	this.MEM_WRITE_ONLY = 5;
	this.MEM_READ_WRITE = 6;
	this.KERNEL_WORK_GROUP_SIZE = 7;

	this.getError = function() { return this.SUCCESS; };
	this.getPlatformIDs = function() { return [11, 22]; };
	this.getDeviceIDs = function(platform_id, device_type) { return [33, 44]; };
	this.createContext = function (context_properties, device_id, notify, userData) { return new WebCLContext(); }; // device_ids is int or array
	this.createCommandQueue = function(context, device_ids, properties) { return new WebCLQueue(); };
	this.createProgramWithSource = function(context, kernelSource) { return new WebCLProgram(); };
	this.buildProgram = function(program, opts, notify, userData) { return this.SUCCESS; };
	this.buildProgram = function(program, device_ids, opts, notify, userData) { return this.SUCCESS; }; // device_ids is int or array
	this.getProgramBuildInfo = function(program, device_id, paramName) { return new WebCLProgramBuildInfo(); };
	this.createKernel = function(program, kernelName) { return new WebCLKernel(); };
	this.createBuffer = function(context, flags, size, host_ptr) { return new WebCLMemObject(); };
	this.createImage2D = function(context, flags, jsImage) { return new WebCLImage(); };
	//this.createImage2D = function(context, flags, canvas) { return new WebCLImage(); };
	//this.createImage2D = function(context, flags, unit8Array) { return new WebCLImage(); };
	this.enqueueWriteBuffer = function(queue, buffer, blocking_write, offset, buffer_size, ptr, event_wait_list) { return new WebCLEvent(); };
	this.enqueueWriteImage  = function(queue, image,  blocking_write, origin, region, jsImage, event_wait_list) { return new WebCLEvent(); };
	this.setKernelArg = function(kernel, arg_index, arg_value) { return this.SUCCESS; };
    this.setKernelArg = function(kernel, arg_index, arg_value, arg_size) { return this.SUCCESS; }; // __local: val (ignored) and size
	this.getKernelWorkGroupInfo = function(kernel, device_id, param_name) { return 512; };
	this.enqueueNDRangeKernel = function(queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, event_wait_list) { return new WebCLEvent(); };
	this.finish = function (queue, userData, cb) { if(cb !== null) cb(userData); return this.SUCCESS; };
    this.enqueueCopyBuffer = function(queue, src_buffer, dst_buffer, event_wait_list) { return new WebCLEvent(); };
	this.enqueueReadBuffer = function(queue, buffer, blocking_read, offset, buffer_size, ptr, event_wait_list) { return new WebCLEvent(); };
	this.enqueueReadImage  = function(queue, image,  blocking_read, origin, region, jsImage, event_wait_list) { return new WebCLEvent(); };
	this.releaseMemObject = function(input) { return this.SUCCESS; };
	this.releaseProgram = function(program) { return this.SUCCESS; };
	this.releaseKernel = function(kernel) { return this.SUCCESS; };
	this.releaseCommandQueue = function(queue) { return this.SUCCESS; };
    this.releaseContext = function(context) { return this.SUCCESS; };
    
    // GL CL Interop
    
    this.createSharedContext = function(device_type, notify, userData) { return new WebCLContext(); };
    this.createFromGLBuffer = function(context, flags, glBuffer) { return new WebCLMemObject(); };
    this.enqueueAcquireGLObjects = function(queue, mem_objects, event_wait_list) { return new WebCLEvent(); };
    this.enqueueReleaseGLObjects = function(queue, mem_objects, event_wait_list) { return new WebCLEvent(); };
}

function WebCLContext() {}
function WebCLQueue() {}
function WebCLProgram() {}
function WebCLKernel() {}
function WebCLMemObject() {}
function WebCLImage() {}
function WebCLEvent() {}
function WebCLProgramBuildInfo() {}
function WebCLKernelWorkGroupInfo() {}
