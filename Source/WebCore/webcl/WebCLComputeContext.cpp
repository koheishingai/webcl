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

#include "WebCLComputeContext.h"
#include "ScriptExecutionContext.h"
#include "Document.h"
#include "DOMWindow.h"
#include "Image.h"
#include "SharedBuffer.h"
#include "CanvasRenderingContext2D.h"
#include "ImageBuffer.h"
#include "CachedImage.h"
#include <wtf/ArrayBuffer.h>
#include "CanvasPixelArray.h"
#include "HTMLCanvasElement.h"


#include <stdio.h>
#include <wtf/ByteArray.h>
#include <CanvasRenderingContext.h>

class CanvasRenderingContext;

using namespace JSC;

namespace WebCore {   

	WebCLComputeContext::WebCLComputeContext(ScriptExecutionContext* context) : ActiveDOMObject(context, this)																				
	{
		m_num_mems = 0;
		m_num_programs = 0;
		m_num_events = 0;
		m_num_samplers = 0;
		m_num_contexts = 0;
		m_num_commandqueues = 0;
	}

	PassRefPtr<WebCLComputeContext> WebCLComputeContext::create(ScriptExecutionContext* context)
	{
		return adoptRef(new WebCLComputeContext(context));
	}

	WebCLComputeContext::~WebCLComputeContext()
	{
	}

	PassRefPtr<WebCLPlatformIDList> WebCLComputeContext::getPlatformIDs(ExceptionCode& ec)
	{
		RefPtr<WebCLPlatformIDList> o = WebCLPlatformIDList::create(this);
		if (o != NULL) {
			m_platform_id = o;
			return o;
		} else {
			ec = FAILURE;
			return NULL;
		}
	}

	WebCLGetInfo WebCLComputeContext::getImageInfo(WebCLImage* image, cl_image_info param_name, ExceptionCode& ec) 	
	{
		cl_mem cl_Image_id = NULL;
		cl_int err = 0;
		size_t sizet_units = 0;
		if (image != NULL) {
			cl_Image_id = image->getCLImage();
			if (cl_Image_id == NULL) {
				printf("Error: cl_Image_id null\n");
				ec = FAILURE;
				return WebCLGetInfo();
			}
		}

		switch(param_name)
		{   
			case IMAGE_ELEMENT_SIZE:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_ELEMENT_SIZE, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_ROW_PITCH:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_ROW_PITCH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_SLICE_PITCH:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_WIDTH:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_WIDTH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_HEIGHT:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_HEIGHT, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_DEPTH:
				err=clGetImageInfo(cl_Image_id, CL_IMAGE_DEPTH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
				// TODO (siba samal) Handle Image Format & CL_IMAGE_D3D10_SUBRESOURCE_KHR types
			default:
				printf("Error: Unsupported Image Info type\n");
				return WebCLGetInfo();
		}
		switch (err) {
			case CL_INVALID_MEM_OBJECT:
				ec = INVALID_MEM_OBJECT;
				printf("Error: CL_INVALID_MEM_OBJECT \n");
				break;
			case CL_INVALID_VALUE:
				ec = INVALID_VALUE;
				printf("Error: CL_INVALID_VALUE \n");
				break;
			case CL_OUT_OF_RESOURCES:
				ec = OUT_OF_RESOURCES;
				printf("Error: CL_OUT_OF_RESOURCES\n");
				break; 
			case CL_OUT_OF_HOST_MEMORY:
				ec = OUT_OF_HOST_MEMORY;
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = FAILURE;
				break;
		}				
		return WebCLGetInfo();

	}

	void WebCLComputeContext::waitForEvents(WebCLEventList* events, ExceptionCode& ec)
	{
		cl_int err = 0;
		cl_event* cl_event_id = NULL;

		if (events != NULL) {
			cl_event_id = events->getCLEvents();
			if (cl_event_id == NULL) {
				printf("Error: cl_event null\n");
				ec = FAILURE;
				return;
			}
		}
		err = clWaitForEvents(events->length(), cl_event_id);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_CONTEXT:
					printf("Error: CL_INVALID_CONTEXT \n");
					ec = INVALID_CONTEXT;
					break;
				case CL_INVALID_VALUE:
					printf("Error: CL_INVALID_VALUE \n");
					ec = INVALID_VALUE;
					break;
				case CL_INVALID_EVENT :
					printf("Error: CL_INVALID_EVENT  \n");
					ec = INVALID_VALUE;
					break;
					//OpenCL 1.1
					//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
					//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST \n");
					//	break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES \n");
					ec = OUT_OF_RESOURCES;
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY \n");
					ec = OUT_OF_HOST_MEMORY;
					break;

				default:
					printf("Error: Invaild Error Type\n");
					ec = FAILURE;
					break;

			}
		} 
		return;
	}


	PassRefPtr<WebCLContext> WebCLComputeContext::createContext(int contextProperties, 
			WebCLDeviceIDList* devices, int pfn_notify, int user_data, ExceptionCode& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = 0;
		cl_device_id cl_device = NULL;

		if (devices != NULL) {
			cl_device = devices->getCLDeviceIDs();
			if (cl_device == NULL) {
				printf("Error: devices null\n");
				return NULL;
			}
		} else {
			printf("Error: webcl_devices null\n");
			printf("Unused in createContext prop=%d pfn_notify=%d user_data=%d\n", 
					contextProperties, pfn_notify, user_data);

			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = clCreateContext(NULL, 1, &cl_device, NULL, NULL, &err);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec = INVALID_PLATFORM;
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("CL_INVALID_PROPERTY  \n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec = INVALID_DEVICE;
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec = INVALID_OPERATION;
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec = DEVICE_NOT_AVAILABLE;
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec = OUT_OF_RESOURCES;
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec = OUT_OF_HOST_MEMORY;				
					break;
					// TODO (siba samal) Error handling following Error Types
					//	case CL_INVALID_D3D10_DEVICE_KHR:
					//		printf("Error: CL_INVALID_D3D10_DEVICE_KHR \n");
					//		break;
					//		case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//			printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//			break;

				default:
					printf("Error: Invalid ERROR Type\n");
					ec = FAILURE;
					break;

			}
		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLContext> WebCLComputeContext::createContextFromType(int contextProperties, 
			int device_type, int pfn_notify, int user_data, ExceptionCode& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = 0;

		//TODO (siba samal) Need to handle context properties	
		if((CONTEXT_PLATFORM != contextProperties) &&  (0 != contextProperties))
		{
			printf("Error: INVALID CONTEXT PROPERTIES\n");
			return NULL;
		}

		switch(device_type) {
			case DEVICE_TYPE_GPU:
				cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
				break;
			case DEVICE_TYPE_CPU:
				cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_CPU, NULL, NULL, &err);
				break;  
			case DEVICE_TYPE_ACCELERATOR:
				cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_ACCELERATOR, NULL, NULL, &err);
				break;  
			case DEVICE_TYPE_DEFAULT:
				cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, &err);
				break;
			case DEVICE_TYPE_ALL:
				cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);
				break;
			default:
				printf("Error:Invalid Device Type \n");
				break;
		}

		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec = INVALID_PLATFORM;
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("CL_INVALID_PROPERTY  \n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec = INVALID_DEVICE;
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec = INVALID_OPERATION;
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec = DEVICE_NOT_AVAILABLE;
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec = OUT_OF_RESOURCES;
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec = OUT_OF_HOST_MEMORY;
					break;
					// TODO (siba samal) Error handling following Error Types
					//	case CL_INVALID_D3D10_DEVICE_KHR:
					//		printf("Error: CL_INVALID_D3D10_DEVICE_KHR \n");
					//		break;
					//		case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//			printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//			break;

				default:
					printf("Error: Invalid ERROR Type\n");
					printf("CreateContext prop=%d pfn_notify=%d user_data=%d\n", 
											contextProperties, pfn_notify, user_data);
					ec = FAILURE;
					break;

			}
		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLContext> WebCLComputeContext::createSharedContext(int device_type, 
			int pfn_notify, int user_data, ExceptionCode& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = NULL;

		CGLContextObj kCGLContext = CGLGetCurrentContext();
		CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);

		cl_context_properties properties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)kCGLShareGroup, 0
		};
		// TODO (siba samal) Handle NULL parameters
		cl_context_id = clCreateContext(properties, 0, 0, 0, 0, &err);

		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec = INVALID_PLATFORM;
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("Error: CL_INVALID_PROPERTY\n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE \n");
					ec = INVALID_DEVICE;
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec = INVALID_OPERATION;
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec = DEVICE_NOT_AVAILABLE;
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec = OUT_OF_RESOURCES;
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec = OUT_OF_HOST_MEMORY;
					break;
					//case CL_INVALID_D3D10_DEVICE_KHR:
					//	printf("Error: CL_INVALID_D3D10_DEVICE_KHR\n");
					//	break;
					//case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//	printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//	break;

				default:
					printf("Error: Invalid ERROR type\n");
					ec = FAILURE;
					break;
			}

		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				printf("WebCLComputeContext::createSharedContext device_type=%d pfn_notify=%d user_data=%d\n",
						device_type, pfn_notify, user_data);
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLContext> WebCLComputeContext::createContext(int contextProperties, 
			WebCLDeviceID* device, int pfn_notify, int user_data, ExceptionCode& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_device_id cl_device = NULL;

		if (device != NULL) {
			cl_device = device->getCLDeviceID();
			if (cl_device == NULL) {
				printf("Error: devices null\n");
				return NULL;
			}
			m_device_id_ = device;
		} else {
			printf("Error: webcl_devices null\n");
			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = clCreateContext(NULL, 1, &cl_device, NULL, NULL, &err);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec = INVALID_PLATFORM;
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("Error: CL_INVALID_PROPERTY\n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec = INVALID_DEVICE;
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec = INVALID_OPERATION;
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec = DEVICE_NOT_AVAILABLE;
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec = OUT_OF_RESOURCES;
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec = OUT_OF_HOST_MEMORY;
					break;
					//case CL_INVALID_D3D10_DEVICE_KHR:
					//	printf("Error: CL_INVALID_D3D10_DEVICE_KHR\n");
					//	break;
					//case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//	printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//	break;

				default:
					printf("Error: Invaild Error Type\n");
					ec = FAILURE;
					break;
			}

		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			o->setDeviceID(m_device_id_);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				ec = FAILURE;

				printf("WebCLComputeContext::createContext prop=%d pfn_notify=%d user_data=%d\n", 
						contextProperties, pfn_notify, user_data);
				return NULL;
			}
		}
		return NULL;
	}


	void WebCLComputeContext::unloadCompiler(ExceptionCode& ec)
	{
		cl_int err =  clUnloadCompiler();
		if (err != CL_SUCCESS) {
			printf("Error: Invaild Error Type\n");
			ec = FAILURE; 
		}
		else {
		}
		return;
	}

} // namespace WebCore

#endif // ENABLE(WEBCL)
