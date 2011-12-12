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

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLProgram.h"
#include "WebCLGetInfo.h"
#include "WebCLKernel.h"
#include "WebCLKernelList.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLProgram::~WebCLProgram()
{
}

PassRefPtr<WebCLProgram> WebCLProgram::create(WebCLComputeContext* 
											compute_context, cl_program program)
{
	return adoptRef(new WebCLProgram(compute_context, program));
}

WebCLProgram::WebCLProgram(WebCLComputeContext* compute_context, 
		cl_program program) : m_context(compute_context), m_cl_program(program)
{
	m_num_programs = 0;
	m_num_kernels = 0; 
}

WebCLGetInfo WebCLProgram::getProgramInfo(int param_name, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_uint uint_units = 0;
	size_t sizet_units = 0;
	char program_string[4096];
	cl_context cl_context_id = NULL;
	RefPtr<WebCLContext> contextObj  = NULL;
	if (m_cl_program == NULL) {
			ec = WebCLComputeContext::INVALID_PROGRAM;
			printf("Error: Invalid program object\n");
			return WebCLGetInfo();
	}

	switch(param_name)
	{   
		case WebCLComputeContext::PROGRAM_REFERENCE_COUNT:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCLComputeContext::PROGRAM_NUM_DEVICES:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_NUM_DEVICES , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case WebCLComputeContext::PROGRAM_BINARY_SIZES:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case WebCLComputeContext::PROGRAM_SOURCE:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_SOURCE, sizeof(program_string), &program_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(program_string));
			break;
		case WebCLComputeContext::PROGRAM_BINARIES:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_BINARIES, sizeof(program_string), &program_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(program_string));
			break;
		case WebCLComputeContext::PROGRAM_CONTEXT:
			err=clGetProgramInfo(m_cl_program, CL_PROGRAM_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
			contextObj = WebCLContext::create(m_context, cl_context_id);
			if(contextObj == NULL)
			{
				printf("Error : CL program context not NULL\n");
				return WebCLGetInfo();
			}
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
			break;
			// TODO (siba samal)- Handle Array of cl_device_id 
			//case WebCLComputeContext::PROGRAM_DEVICES:
			//  size_t numDevices;
			//  clGetProgramInfo( m_cl_program, CL_PROGRAM_DEVICES, 0, 0, &numDevices );
			//  cl_device_id *devices = new cl_device_id[numDevices];
			//  clGetProgramInfo( m_cl_program, CL_PROGRAM_DEVICES, numDevices, devices, &numDevices );
			//  return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
		default:
			printf("Error: UNSUPPORTED program Info type = %d ",param_name);
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_PROGRAM:
			ec = WebCLComputeContext::INVALID_PROGRAM;
			printf("Error: CL_INVALID_PROGRAM \n");
			break;
		case CL_INVALID_VALUE:
			ec = WebCLComputeContext::INVALID_VALUE;
			printf("Error: CL_INVALID_VALUE\n");
			break;
		case CL_OUT_OF_RESOURCES:
			ec = WebCLComputeContext::OUT_OF_RESOURCES;
			printf("Error: CL_OUT_OF_RESOURCES \n");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
			printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
			break;
		default:
			ec = WebCLComputeContext::FAILURE;
			printf("Error: Invaild Error Type\n");
			break;
	}
	return WebCLGetInfo();
}


WebCLGetInfo WebCLProgram::getProgramBuildInfo(WebCLDeviceID* device, int param_name, ExceptionCode& ec)
{
	cl_device_id device_id = NULL;
	cl_uint err = 0;
	char buffer[8192];
	size_t len = 0;

	if (m_cl_program == NULL) {
			printf("Error: Invalid program object\n");
			ec = WebCLComputeContext::INVALID_PROGRAM;
			return WebCLGetInfo();
	}
	if (device != NULL) {
		device_id = device->getCLDeviceID();
		if (device_id == NULL) {
			printf("Error: device_id null\n");
			return WebCLGetInfo();
		}
	}

	switch (param_name) {
		case WebCLComputeContext::PROGRAM_BUILD_LOG:
			err = clGetProgramBuildInfo(m_cl_program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(buffer));
			break;
		case WebCLComputeContext::PROGRAM_BUILD_OPTIONS:
			err = clGetProgramBuildInfo(m_cl_program, device_id, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), &buffer, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(buffer));
			break;
		case WebCLComputeContext::PROGRAM_BUILD_STATUS:
			cl_build_status build_status;
			err = clGetProgramBuildInfo(m_cl_program, device_id, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(build_status));
			break;
		default:
			printf("Error: UNSUPPORTED Program Build Info   Type = %d ",param_name);
			return WebCLGetInfo();			
	}
	switch (err) {
		case CL_INVALID_DEVICE:
			ec = WebCLComputeContext::INVALID_DEVICE;
			printf("Error: CL_INVALID_DEVICE   \n");
			break;
		case CL_INVALID_VALUE:
			ec = WebCLComputeContext::INVALID_VALUE;
			printf("Error: CL_INVALID_VALUE \n");
			break;
		case CL_INVALID_PROGRAM:
			ec = WebCLComputeContext::INVALID_PROGRAM;
			printf("Error: CL_INVALID_PROGRAM  \n");
			break;
		case CL_OUT_OF_RESOURCES:
			ec = WebCLComputeContext::OUT_OF_RESOURCES;
			printf("Error: CL_OUT_OF_RESOURCES \n");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
			printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
			break;
		default:
			ec = WebCLComputeContext::FAILURE;
			printf("Error: Invaild Error Type\n");
			break;
	}				
	return WebCLGetInfo();
}

PassRefPtr<WebCLKernel> WebCLProgram::createKernel(	const String& kernel_name,
							ExceptionCode& ec)
{
	cl_int err = 0;	
	cl_kernel cl_kernel_id = NULL;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	// TODO(siba samal) - more detailed error code need to be addressed later
	const char* kernel_name_str = strdup(kernel_name.utf8().data());
	cl_kernel_id = clCreateKernel(m_cl_program, kernel_name_str, &err);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
				ec = WebCLComputeContext::INVALID_PROGRAM_EXECUTABLE;
				break;
			case CL_INVALID_KERNEL_NAME:
				printf("Error: CL_INVALID_KERNEL_NAME\n");
				ec = WebCLComputeContext::INVALID_KERNEL_NAME;
				break;
			case CL_INVALID_KERNEL_DEFINITION:
				printf("Error: CL_INVALID_KERNEL_DEFINITION\n");
				ec = WebCLComputeContext::INVALID_KERNEL_DEFINITION;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLKernel> o = WebCLKernel::create(m_context, cl_kernel_id);
		o->setDeviceID(m_device_id);
		m_kernel_list.append(o);
		m_num_kernels++;

		return o;
	}
	return NULL;
}

PassRefPtr<WebCLKernelList> WebCLProgram::createKernelsInProgram( ExceptionCode& ec)
{
	cl_int err = 0;
	cl_kernel* kernelBuf = NULL;
	cl_uint num = 0;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	err = clCreateKernelsInProgram (m_cl_program, NULL, NULL, &num);
	if (err != CL_SUCCESS) {
		printf("Error: clCreateKernelsInProgram \n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if(num == 0) {
		printf("Warning: createKernelsInProgram - Number of Kernels is 0 \n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	kernelBuf = (cl_kernel*)malloc (sizeof(cl_kernel) * num);
	if (!kernelBuf) {
		return NULL;
	}

	err = clCreateKernelsInProgram (m_cl_program, num, kernelBuf, NULL);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
				ec = WebCLComputeContext::INVALID_PROGRAM_EXECUTABLE;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLKernelList> o = WebCLKernelList::create(m_context, kernelBuf, num);
		printf("WebCLKernelList Size = %d \n\n\n\n", num);
		//m_kernel_list = o;
		m_num_kernels = num;
		return o;
	}
	return NULL;
}

void WebCLProgram::buildProgram(int options, int pfn_notify,
		int user_data, ExceptionCode& ec)
{
	cl_int err = 0;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}

	// TODO(siba samal) - needs to be addressed later
	err = clBuildProgram(m_cl_program, 0, NULL, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec = WebCLComputeContext::INVALID_DEVICE;
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec = WebCLComputeContext::INVALID_BINARY;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec = WebCLComputeContext::INVALID_BUILD_OPTIONS;
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec = WebCLComputeContext::COMPILER_NOT_AVAILABLE;
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec = WebCLComputeContext::BUILD_PROGRAM_FAILURE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLComputeContext::buildProgram normal options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::buildProgram(WebCLDeviceID* device_id,int options, 
		int pfn_notify, int user_data, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_device_id cl_device = NULL;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	cl_device = device_id->getCLDeviceID();
	if (cl_device == NULL) {
		printf("Error: devices null\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}

	// TODO(siba samal) - NULL parameters needs to be addressed later
	err = clBuildProgram(m_cl_program, 1, (const cl_device_id*)&cl_device, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec = WebCLComputeContext::INVALID_DEVICE;
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec = WebCLComputeContext::INVALID_BINARY;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec = WebCLComputeContext::INVALID_BUILD_OPTIONS;
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec = WebCLComputeContext::COMPILER_NOT_AVAILABLE;
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec = WebCLComputeContext::BUILD_PROGRAM_FAILURE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLComputeContext::buildProgram WebCLDeviceID options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::buildProgram( WebCLDeviceIDList* cl_devices, int options, 
		int pfn_notify, int user_data, ExceptionCode& ec)
{
	cl_int err = 0;	
	cl_device_id cl_device = NULL;

	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return ;
	}
	if (cl_devices != NULL) {
		cl_device = cl_devices->getCLDeviceIDs();
		if (cl_device == NULL) {
			printf("Error: devices null\n");
			return;
		}
	} else {
		printf("Error: webcl_devices null\n");
		return;
	}

	// TODO(siba samal) - NULL parameters needs to be addressed later
	err = clBuildProgram(m_cl_program, cl_devices->length(), (const cl_device_id*)&cl_device, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec = WebCLComputeContext::INVALID_DEVICE;
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec = WebCLComputeContext::INVALID_BINARY;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec = WebCLComputeContext::INVALID_BUILD_OPTIONS;
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec = WebCLComputeContext::COMPILER_NOT_AVAILABLE;
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec = WebCLComputeContext::BUILD_PROGRAM_FAILURE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLComputeContext::buildProgram WebCLDeviceIDList  options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::releaseCLResource( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clReleaseProgram(m_cl_program);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM  \n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES ;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY ;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		for (int i = 0; i < m_num_programs; i++) {
			if ((m_program_list[i].get())->getCLProgram() == m_cl_program) {
				m_program_list.remove(i);
				m_num_programs = m_program_list.size();
				break;
			}
		}
		return;
	}
	return;
}


void WebCLProgram::retainCLResource( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clRetainProgram(m_cl_program);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec = WebCLComputeContext::INVALID_PROGRAM;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES ;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY ;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		printf("Success: clRetainProgram\n");
		// TODO - Check if has to be really added
		RefPtr<WebCLProgram> o = WebCLProgram::create(m_context, m_cl_program);
		m_program_list.append(o);
		m_num_programs++;
		return;
	}
	return;
}
	
void WebCLProgram::setDeviceID(RefPtr<WebCLDeviceID> m_device_id_)
{
	m_device_id = m_device_id_;
}

cl_program WebCLProgram::getCLProgram()
{
	return m_cl_program;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
