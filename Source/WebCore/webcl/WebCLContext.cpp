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

#include "WebCLContext.h"
#include "WebCLContext.h"
#include "WebCLCommandQueue.h"
#include "WebCLProgram.h"
#include "WebCLKernel.h"
#include "WebCLMem.h"
#include "WebCLEvent.h"
#include "WebCLSampler.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLContext::~WebCLContext()
{
}

PassRefPtr<WebCLContext> WebCLContext::create(WebCLComputeContext* compute_context, cl_context context_id)
{
	return adoptRef(new WebCLContext(compute_context, context_id));
}

WebCLContext::WebCLContext(WebCLComputeContext* compute_context, cl_context context_id) 
			: m_videoCache(4), m_context(compute_context), m_cl_context(context_id)
{
	m_num_programs = 0;
	m_num_mems = 0;
	m_num_events = 0;
	m_num_samplers = 0;
	m_num_contexts = 0;

}

WebCLGetInfo WebCLContext::getContextInfo(int param_name, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_uint uint_units = 0;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::INVALID_CONTEXT;
		return WebCLGetInfo();
	}
	
	switch(param_name)
	{	
		case WebCLComputeContext::CONTEXT_REFERENCE_COUNT:
			err = clGetContextInfo(m_cl_context, CL_CONTEXT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
			// TODO (siba samal) Handle CONTEXT_DEVICES & CONTEXT_PROPERTIES
		default:
			printf("Error: Unsupported Context Info type\n");
			ec = WebCLComputeContext::FAILURE;
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_CONTEXT:
			ec = WebCLComputeContext::INVALID_CONTEXT;
			printf("Error: CL_INVALID_CONTEXT \n");
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

PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDeviceIDList* devices, 
							int command_queue_prop, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_device_id cl_device = NULL;
	cl_command_queue cl_command_queue_id = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (devices != NULL) {
		cl_device = devices->getCLDeviceIDs();
		if (cl_device == NULL) {
			printf("Error: cl_device null\n");
			return NULL;
		}
	}
	switch (command_queue_prop)
	{
		case WebCLComputeContext::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
			break;
		case WebCLComputeContext::QUEUE_PROFILING_ENABLE:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
			break;
		default:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, NULL, &err);
			break;
	}
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE \n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE \n");
				ec = WebCLComputeContext::INVALID_DEVICE;
				break;
			case CL_INVALID_QUEUE_PROPERTIES:
				printf("Error: CL_INVALID_QUEUE_PROPERTIES \n");
				ec = WebCLComputeContext::INVALID_QUEUE_PROPERTIES;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
		if (o != NULL) {
			m_command_queue = o;
			return o;
		} else {
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	return NULL;
}

PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDeviceID* device, int command_queue_prop, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_device_id cl_device = NULL;
	cl_command_queue cl_command_queue_id = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (device != NULL) {
		cl_device = device->getCLDeviceID();
		if (cl_device == NULL) {
			printf("Error: cl_device null\n");
			return NULL;
		}
	}
	switch (command_queue_prop)
	{
		case WebCLComputeContext::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
			break;
		case WebCLComputeContext::QUEUE_PROFILING_ENABLE:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
			break;
		default:
			cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, NULL, &err);
			break;
	}

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE \n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE  \n");
				ec = WebCLComputeContext::INVALID_DEVICE;
				break;
			case CL_INVALID_QUEUE_PROPERTIES:
				printf("Error: CL_INVALID_QUEUE_PROPERTIES  \n");
				ec = WebCLComputeContext::INVALID_QUEUE_PROPERTIES;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;

			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
		if (o != NULL) {
			m_command_queue = o;
			return o;
		} else {
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	return NULL;
}

PassRefPtr<WebCLProgram> WebCLContext::createProgramWithSource(const String& kernelSource, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_program cl_program_id = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	const char* source = strdup(kernelSource.utf8().data());

	// TODO(won.jeon) - the second and fourth arguments need to be addressed later
	cl_program_id = clCreateProgramWithSource(m_cl_context, 1, (const char**)&source, 
			NULL, &err);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE \n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLProgram> o = WebCLProgram::create(m_context, cl_program_id);
		o->setDeviceID(m_device_id);
		if (o != NULL) {
			m_program_list.append(o);
			m_num_programs++;
			return o;
		} else {
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	return NULL;
}

PassRefPtr<WebCLProgram> WebCLContext::createProgramWithBinary(WebCLDeviceIDList* cl_devices,
								const String& kernelBinary, ExceptionCode& ec)
{
	cl_int err = 0;	
	cl_program cl_program_id = NULL;
	cl_device_id cl_device = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL; 
	}
	if (cl_devices != NULL) {
		cl_device = cl_devices->getCLDeviceIDs();
		if (cl_device == NULL) {
			printf("Error: devices null\n");
			return NULL;
		}
	} else {
		printf("Error: webcl_devices null\n");
		return NULL;
	}

	const char* binary = strdup(kernelBinary.utf8().data());
	// TODO(siba samal) - length & binary_status arguments need to be addressed later
	cl_program_id = clCreateProgramWithBinary(m_cl_context, cl_devices->length(),(const cl_device_id*)&cl_device,
			NULL, (const unsigned char**)&binary, NULL, &err);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE \n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE  \n");
				ec = WebCLComputeContext::INVALID_DEVICE ;
				break;
			case CL_INVALID_BINARY:	
				printf("Error: CL_INVALID_BINARY   \n");
				ec = WebCLComputeContext::INVALID_BINARY;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;

			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLProgram> o = WebCLProgram::create(m_context, cl_program_id);
		o->setDeviceID(m_device_id);
		if (o != NULL) {
			m_program_list.append(o);
			m_num_programs++;
			return o;
		} else {
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createBuffer(int flags, int size, int host_ptr, ExceptionCode& ec)
{
	cl_int err = 0;	
	cl_mem cl_mem_id = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	// TODO(won.jeon) - NULL parameter needs to be addressed later
	switch (flags)
	{
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_ONLY, size, NULL, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_WRITE_ONLY, size, NULL, &err);
			break;
		case WebCLComputeContext::MEM_READ_WRITE:
			cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_WRITE, size, NULL, &err);
			break;
		default:
			printf("Error: Unsupported Mem Flsg\n");
			printf("WebCLContext::createBuffer host_ptr = %d\n", host_ptr);
			ec = WebCLComputeContext::INVALID_CONTEXT;
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clCreateBuffer\n");
		switch(err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_BUFFER_SIZE:
				printf("Error: CL_INVALID_BUFFER_SIZE\n");
				ec = WebCLComputeContext::INVALID_BUFFER_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}


PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, 
		HTMLCanvasElement* canvasElement, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_image_format image_format = {CL_RGBA, CL_UNSIGNED_INT32};
	cl_uint width = 0;
	cl_uint height = 0;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}	
	ImageBuffer* imageBuffer = NULL;
	RefPtr<ByteArray> bytearray = NULL;
	if (canvasElement != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		width = (cl_uint) canvasElement->width();
		height = (cl_uint) canvasElement->height();

		imageBuffer = canvasElement->buffer();
		if ( imageBuffer == NULL)
			printf("image is null\n");
		bytearray = imageBuffer->getUnmultipliedImageData(IntRect(0,0,width,height));

		if ( bytearray == NULL)
			printf("bytearray is null\n");
	}

	if(bytearray->data() == NULL)
	{
		printf("bytearray->data() is null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	void* image = (void*) bytearray->data();
	if(image == NULL)
	{
		printf("image is null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR , 
					&image_format, width, height,0, image, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, (CL_MEM_READ_ONLY || CL_MEM_USE_HOST_PTR ||CL_MEM_COPY_HOST_PTR), 
					&image_format, width, height, 0, image, &err);
			break;
			// TODO (siba samal) Support other mem_flags & testing 
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}	

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLImageElement* image, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = 0;
	cl_image_format image_format;
	cl_uint width = 0;
	cl_uint height = 0;

	Image* imagebuf = NULL;
	CachedImage* cachedImage = NULL; 
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (image != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		cachedImage = image->cachedImage();
		if (cachedImage == NULL) {
			ec = WebCLComputeContext::FAILURE;
			printf("Error: image null\n");
			return NULL;
		} 
		else {
			width = (cl_uint) image->width();
			height = (cl_uint) image->height();
			imagebuf = cachedImage->image();
			if(imagebuf == NULL)
			{
				ec = WebCLComputeContext::FAILURE;
				printf("Error: imagebuf null\n");
				return NULL;
			}
		}
	} 
	else {
		printf("Error: imageElement null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	const char* image1  = (const char*)cachedImage->image()->data()->data() ;
	if(image1 == NULL)
	{
		ec = WebCLComputeContext::FAILURE;
		printf("Error: image data is null\n");
		return NULL;
	}
	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
					&image_format, width, height, 0, (void*)image1, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
					&image_format, width, height, 0, (void*)image1, &err);
			break;
			// TODO (siba samal) Support other flags & testing
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLVideoElement* video, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_image_format image_format;
	cl_uint width = 0;
	cl_uint height = 0;

	RefPtr<Image> image = NULL;
	SharedBuffer* sharedBuffer = NULL;
	const char* image_data = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (video != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		width = (cl_uint) video->width();
		height = (cl_uint) video->height();
		image = videoFrameToImage(video);
		sharedBuffer = image->data();
		if (sharedBuffer == NULL) {
			printf("Error: sharedBuffer null\n");
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		} else {
			image_data = sharedBuffer->data();

			if (image_data == NULL) {
				printf("Error: image_data null\n");
				ec = WebCLComputeContext::FAILURE;
				return NULL;
			}
		}

	} else {
		printf("Error: canvasElement null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
					&image_format, width, height, 0, (void *)image_data, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
					&image_format, width, height, 0, (void *)image_data, &err);
			break;
			// TODO (siba samal) Support other flags & testing
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, ImageData* data, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_image_format image_format;
	cl_uint width = 0;
	cl_uint height = 0;

	CanvasPixelArray* pixelarray = NULL;
	ByteArray* bytearray = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (data != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		pixelarray = data->data();
		if(pixelarray != NULL)
		{
			bytearray = pixelarray->data();
			if(bytearray == NULL)
				return NULL;
		}			
		width = (cl_uint) data->width();
		height = (cl_uint) data->height();
	} else {
		printf("Error: canvasElement null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
					&image_format, width, height, 0, (void*)bytearray, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
					&image_format, width, height, 0, (void*)bytearray, &err);
			break;
			// TODO (siba samal) Support other flags & testing
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags,unsigned int width, 
						unsigned int height,ArrayBuffer* data, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_image_format image_format;
	cl_uint cl_width = 0;
	cl_uint cl_height = 0;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (data != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		cl_width = width;
		cl_height = height;
	} else {
		printf("Error: canvasElement null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
					&image_format, cl_width, cl_height, 0, data->data(), &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
					&image_format, cl_width, cl_height, 0, data->data(), &err);
			break;
			// TODO (siba samal) Support other flags & testing
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage3D(int flags,unsigned int width, 
		unsigned int height,
		unsigned int depth,
		ArrayBuffer* data, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_image_format image_format;
	cl_uint cl_width = 0;
	cl_uint cl_height = 0;
	cl_uint cl_depth = 0;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (data != NULL) {
		image_format.image_channel_data_type = CL_UNSIGNED_INT8;
		image_format.image_channel_order = CL_RGBA;
		cl_width = width;
		cl_height = height;
		cl_depth = depth;
	} else {
		printf("Error: canvasElement null\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
					&image_format, cl_width, cl_height, cl_depth,0, 0, data->data(), &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_WRITE_ONLY, 
					&image_format, cl_width, cl_height, cl_depth, 0, 0,data->data(), &err);
			break;
			// TODO (siba samal) Support other flags & testing
	}
	if (cl_mem_image == NULL) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_HOST_PTR:
				printf("Error: CL_INVALID_HOST_PTR\n");
				ec = WebCLComputeContext::INVALID_HOST_PTR;
				break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
				ec = WebCLComputeContext::IMAGE_FORMAT_NOT_SUPPORTED;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLBuffer(int flags, WebGLBuffer* bufobj, ExceptionCode& ec)
{
	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	Platform3DObject buf_id = 0;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (bufobj != NULL) {
		buf_id = bufobj->object();
		if (buf_id == 0) {
			printf("Error: buf_id null\n");
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	switch (flags) {
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_READ_ONLY, buf_id, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_WRITE_ONLY, buf_id, &err);
			break;
		case WebCLComputeContext::MEM_READ_WRITE:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_READ_WRITE, buf_id, &err);
			break;
		case WebCLComputeContext::MEM_USE_HOST_PTR:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_USE_HOST_PTR, buf_id, &err);
			break;
		case WebCLComputeContext::MEM_ALLOC_HOST_PTR:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, buf_id, &err);
			break;
		case WebCLComputeContext::MEM_COPY_HOST_PTR:
			cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, buf_id, &err);
			break;

	}
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_GL_OBJECT:
				printf("Error: CL_INVALID_GL_OBJECT\n");
				ec = WebCLComputeContext::INVALID_GL_OBJECT;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
		m_mem_list.append(o);
		m_num_mems++;
		return o;

	}
	return NULL;
}

PassRefPtr<WebCLSampler> WebCLContext::createSampler(bool norm_cords, 
		int addr_mode, int fltr_mode, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_bool normalized_coords = CL_FALSE;
	cl_addressing_mode addressing_mode = CL_ADDRESS_NONE;
	cl_filter_mode filter_mode = CL_FILTER_NEAREST;
	cl_sampler cl_sampler_id = NULL;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if(norm_cords)
		normalized_coords = CL_TRUE;
	switch(addr_mode)
	{
		case WebCLComputeContext::ADDRESS_NONE:
			addressing_mode = CL_ADDRESS_NONE;
			break;
		case WebCLComputeContext::ADDRESS_CLAMP_TO_EDGE:
			addressing_mode = CL_ADDRESS_CLAMP_TO_EDGE;
			break;
		case WebCLComputeContext::ADDRESS_CLAMP:
			addressing_mode = CL_ADDRESS_CLAMP;
			break;
		case WebCLComputeContext::ADDRESS_REPEAT: 
			addressing_mode = CL_ADDRESS_REPEAT;
			break;
		default:
			printf("Error: Invaild Addressing Mode\n");
			ec = WebCLComputeContext::FAILURE;
			return NULL;
	}
	switch(fltr_mode)
	{
		case WebCLComputeContext::FILTER_LINEAR:
			filter_mode = CL_FILTER_LINEAR;
			break;
		case WebCLComputeContext::FILTER_NEAREST :
			filter_mode = CL_FILTER_NEAREST ;
			break;
		default:
			printf("Error: Invaild Filtering Mode\n");
			ec = WebCLComputeContext::FAILURE;
			return NULL;
	}
	cl_sampler_id = clCreateSampler(m_cl_context, normalized_coords, addressing_mode, 
			filter_mode, &err);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE \n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_OPERATION :
				printf("Error: CL_INVALID_OPERATION   \n");
				ec = WebCLComputeContext::INVALID_OPERATION  ;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;

			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLSampler> o = WebCLSampler::create(m_context, cl_sampler_id);
		if (o != NULL) {
			m_sampler_list.append(o);
			m_num_samplers++;
			return o;
		} else {
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}
	return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLTexture2D(int flags, 
		GC3Denum texture_target, GC3Dint miplevel, GC3Duint texture, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_id = NULL;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	switch (flags)
	{
		case WebCLComputeContext::MEM_READ_ONLY:
			cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_ONLY, 
					texture_target, miplevel, texture, &err);
			break;
		case WebCLComputeContext::MEM_WRITE_ONLY:
			cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_WRITE_ONLY,
					texture_target, miplevel, texture, &err);
			break;
		case WebCLComputeContext::MEM_READ_WRITE:
			cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_WRITE,
					texture_target, miplevel, texture, &err);
			break;
	}
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MIP_LEVEL:
				printf("Error: CL_INVALID_MIP_LEVEL\n");
				ec = WebCLComputeContext::INVALID_MIP_LEVEL;
				break;
			case CL_INVALID_GL_OBJECT  :
				printf("Error: CL_INVALID_GL_OBJECT\n");
				ec = WebCLComputeContext::INVALID_GL_OBJECT;
				break;

			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
				ec = WebCLComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
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
		RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
		m_mem_list.append(o);
		m_num_mems++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLEvent> WebCLContext::createUserEvent( ExceptionCode& ec)
{
	cl_int err = -1;	
	cl_event event = NULL;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	//(TODO) To be uncommented for OpenCL1.1
	//event =  clCreateUserEvent(cl_context_id, &err);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT :
				printf("Error: CL_INVALID_CONTEXT \n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_OUT_OF_RESOURCES :
				printf("Error: CCL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY :
				printf("Error: CCL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, event);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}


void WebCLContext::retainCLResource( ExceptionCode& ec)
{
	cl_int err = 0;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clRetainContext(m_cl_context);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT :
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
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
		printf("Success: clRetainContext\n");
		// TODO - Check if has to be really added
		RefPtr<WebCLContext> o = WebCLContext::create(m_context, m_cl_context);
		m_context_list.append(o);
		m_num_contexts++;
		return;
	}
	return;
}

void WebCLContext::releaseCLResource( ExceptionCode& ec)
{
	cl_int err = 0;
	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clReleaseContext(m_cl_context);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_CONTEXT :
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_OUT_OF_RESOURCES  :
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY  :
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		m_context = NULL;
		return;
	}
	return;
}

void WebCLContext::setDeviceID(RefPtr<WebCLDeviceID> device_id)
{
	m_device_id = device_id;
}
cl_context WebCLContext::getCLContext()
{
	return m_cl_context;
}
PassRefPtr<Image> WebCLContext::videoFrameToImage(HTMLVideoElement* video)
{
	if (!video || !video->videoWidth() || !video->videoHeight()) {
		return 0;
	}
	IntSize size(video->videoWidth(), video->videoHeight());
	ImageBuffer* buf = m_videoCache.imageBuffer(size);
	if (!buf) {
		return 0;
	}
	IntRect destRect(0, 0, size.width(), size.height());
	// FIXME: Turn this into a GPU-GPU texture copy instead of CPU readback.
	video->paintCurrentFrameInContext(buf->context(), destRect);
	return buf->copyImage();
}

WebCLContext::LRUImageBufferCache::LRUImageBufferCache(int capacity)
	: m_buffers(adoptArrayPtr(new OwnPtr<ImageBuffer>[capacity]))
	  , m_capacity(capacity)
{
}

ImageBuffer* WebCLContext::LRUImageBufferCache::imageBuffer(const IntSize& size)
{
	int i;
	for (i = 0; i < m_capacity; ++i) {
		ImageBuffer* buf = m_buffers[i].get();
		if (!buf)
			break;
		if (buf->size() != size)
			continue;
		bubbleToFront(i);
		return buf;
	}

	OwnPtr<ImageBuffer> temp = ImageBuffer::create(size);
	if (!temp)
		return 0;
	i = std::min(m_capacity - 1, i);
	m_buffers[i] = temp.release();

	ImageBuffer* buf = m_buffers[i].get();
	bubbleToFront(i);
	return buf;
}

void WebCLContext::LRUImageBufferCache::bubbleToFront(int idx)
{
	for (int i = idx; i > 0; --i)
		m_buffers[i].swap(m_buffers[i-1]);
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
