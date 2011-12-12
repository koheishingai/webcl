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

#include "WebCLCommandQueue.h"
#include "WebCLEventList.h"
#include "WebCLComputeContext.h"
#include <wtf/ArrayBuffer.h>
#include <wtf/ByteArray.h>
#include "CanvasPixelArray.h"

namespace WebCore {

WebCLCommandQueue::~WebCLCommandQueue()
{
}

PassRefPtr<WebCLCommandQueue> WebCLCommandQueue::create(WebCLComputeContext* compute_context, cl_command_queue command_queue)
{
	return adoptRef(new WebCLCommandQueue(compute_context, command_queue));
}

WebCLCommandQueue::WebCLCommandQueue(WebCLComputeContext* compute_context, cl_command_queue command_queue) : m_context(compute_context), m_cl_command_queue(command_queue)
{
}

WebCLGetInfo WebCLCommandQueue:: getCommandQueueInfo(int param_name, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_uint uint_units = 0;
	cl_context cl_context_id = NULL;
	RefPtr<WebCLContext> contextObj = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
		return WebCLGetInfo();
	}		
	switch(param_name)
	{

		case WebCLComputeContext::QUEUE_REFERENCE_COUNT:
			err=clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCLComputeContext::QUEUE_CONTEXT:
			clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
			contextObj = WebCLContext::create(m_context, cl_context_id);
			if(contextObj != NULL)
			{
			}
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
			break;
			// TODO (siba samal) Support for QUEUE_DEVICE & QUEUE_PROPERTIES
			// {
			// case WebCLComputeContext::QUEUE_DEVICE:
			// cl_device_id device_id = NULL;
			// clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_DEVICE, sizeof(cl_device_id), &device_id, NULL);
			// printf("SUCCESS: Cl Device ID  = %u\n",(unsigned int)device_id );
			// return WebCLGetInfo(static_cast<unsigned int>(device_id));							
			// }
			// {
			// case WebCLComputeContext::QUEUE_PROPERTIES:
			// cl_command_queue_properties queue_properties = NULL;
			// clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties), &queue_properties, NULL);
			//return WebCLGetInfo(static_cast<unsigned int>(queue_properties));

			//}

		default:
			printf("Error: Unsupported Commans Queue Info type\n");
			ec = WebCLComputeContext::FAILURE;
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_COMMAND_QUEUE:
			printf("Error: CL_INVALID_COMMAND_QUEUE \n");
			ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
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
	return WebCLGetInfo();
}

PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
		Float32Array* ptr, int event_wait_list, ExceptionCode& ec)
{
	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return  NULL;
	}	

	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}
	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_write) {
		case true:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, offset, 
					buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, offset, 
					buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
				printf("WebCLCommandQueue::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
						offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
		ImageData* data, int event_wait_list, ExceptionCode& ec)
{
	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;
    ByteArray* bytearray = NULL;
	CanvasPixelArray* pixelarray = NULL;
	//unsigned char *byte_ptr = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return  NULL;
	}	

	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}
	printf("\nStart data ptr\n");

    if (data != NULL) {
		
        pixelarray = data->data();
   
	printf("Start pixelarray ptr=\n");
     if(pixelarray != NULL)
        {
            bytearray = pixelarray->data();
            if(bytearray == NULL)
                return NULL;
        }
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLComputeContext::FAILURE;
        return NULL;
    }
	printf("Start clEnqueueWriteBuffer ptr=\n");
	//byte_ptr = &bytearray->data()[0];

	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_write) {
		case true:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, offset, 
					buffer_size, bytearray->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, offset, 
					buffer_size, bytearray->data(), event_wait_list, NULL, &cl_event_id);
			break;
	}
	printf("End clEnqueueWriteBuffer ptr=\n");
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
				printf("WebCLCommandQueue::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
						offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
	
		
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}
#if 0
PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size,
		ImageData* ptr, int event_wait_list)
{
	printf("WebCLCommandQueue::enqueueWriteBuffer(ImageData) offset=%d buffer_size=%d event_wait_list=%d\n",
			offset, buffer_size, event_wait_list);

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}	
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		//cl_mem cl_mem_id = (m_mem_list[0].get())->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}

	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_write) {
		case true:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, ptr->data()->data()->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, ptr->data()->data()->data(), event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				break;
			case CL_INVALID_CONTEXT:
				ec = WebCLComputeContext::INVALID_CONTEXT;
				printf("Error: CL_INVALID_CONTEXT\n");
				break;
			case CL_INVALID_MEM_OBJECT:
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				break;
			case CL_INVALID_VALUE:
				ec = WebCLComputeContext::INVALID_VALUE;
				printf("Error: CL_INVALID_VALUE\n");
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				ec = WebCLComputeContext::FAILURE;
				printf("Error: Invaild Error Type\n");
				break;
		}

	} else {
		printf("Success: clEnqueueWriteBuffer\n");
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		printf("m_num_events=%ld\n", m_num_events);
		return o;
	}
	return NULL;
}
#endif
PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
		Int32Array* ptr, int event_wait_list, ExceptionCode& ec)
{

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}


	}

	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_write) {
		case true:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, ptr->data(), event_wait_list ,NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				//	ec = WebCLComputeContext::ISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
				printf("WebCLCommandQueue::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
						offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		printf("Success: clEnqueueWriteBuffer\n");
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
		Uint8Array* ptr, int event_wait_list, ExceptionCode& ec)
{

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}


	}

	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_write) {
		case true:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, ptr->data(),event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
				printf("WebCLCommandQueue::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
						offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;

		}
	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}


PassRefPtr<WebCLEvent>  WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
		Float32Array* ptr, int event_wait_list, ExceptionCode& ec)
{

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}


	}
	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_read) {
		case true:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, ptr->data(),event_wait_list, NULL,&cl_event_id);
			break;
		case false:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, ptr->data(),event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueReadBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET :
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLCommandQueue::enqueueReadBuffer(Float32Array) offset=%d buffer_size=%d event_wait_list=%d\n", offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

PassRefPtr<WebCLEvent>  WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
		ImageData *data, int event_wait_list, ExceptionCode& ec)
{

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;
	CanvasPixelArray* pixelarray = NULL;
    ByteArray* bytearray = NULL;
	//unsigned char * byte_data_ptr;
	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}

    if (data != NULL) {
        pixelarray = data->data();
        if(pixelarray != NULL)
        {
            bytearray = pixelarray->data();
            if(bytearray == NULL)
                return NULL;
        }
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLComputeContext::FAILURE;
        return NULL;
    }
	
	//byte_data_ptr = &(bytearray->data()[0]);
	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_read) {
		case true:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, bytearray->data(), event_wait_list, NULL,&cl_event_id);
			break;
		case false:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, bytearray->data(), event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueReadBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET :
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLCommandQueue::enqueueReadBuffer(Float32Array) offset=%d buffer_size=%d event_wait_list=%d\n", offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;

		}

	} else {
		
//		data = &(ImageData::create(IntSize(240,400) ,bytearray));

		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

#if 0
PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
		HTMLCanvasElement* canvas_element, int event_wait_list, ExceptionCode& ec)
{
	// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
	printf("WebCLCommandQueue::enqueueReadBuffer(HTMLCanvasElement) offset=%d buffer_size=%d event_wait_list=%d\n", 
			offset, buffer_size, event_wait_list);

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;
	CanvasRenderingContext2D* rendering_context = NULL;
	RefPtr<ImageData> image_data = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}
	if (canvas_element != NULL) {
		//graphics_context = canvas_element->drawingContext();
		rendering_context = static_cast<CanvasRenderingContext2D*>(canvas_element->renderingContext());

	} else {
		printf("Error: canvas_element == NULL\n");
		return NULL;
	}

	unsigned char* tmp = (unsigned char*)malloc(buffer_size*sizeof(unsigned char));
	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_read) {
		case true:
			//err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
			//		offset, buffer_size, image_data->data()->data()->data(), 0, NULL, NULL);
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE,
					offset, buffer_size, tmp,event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, image_data->data()->data()->data(),event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				break;
			case CL_INVALID_CONTEXT:
				ec = WebCLComputeContext::INVALID_CONTEXT;
				printf("Error: CL_INVALID_CONTEXT\n");
				break;
			case CL_INVALID_MEM_OBJECT:
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				break;
			case CL_INVALID_VALUE:
				ec = WebCLComputeContext::INVALID_VALUE;
				printf("Error: CL_INVALID_VALUE\n");
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				break;
		}
		ec = WebCLComputeContext::FAILURE;

	} else {
		printf("Success: clEnqueueReadBuffer\n");

		for (int i = 0; i < buffer_size; i++) {
			//printf("%u ", image_data->data()->data()->get(i));
			printf("%u ", tmp[i]);
			if ((i+1) % 4 == 0) {
				printf("\n");
			}
		}
		printf("\n");
		//ExceptionCode ec = 0;
		RefPtr<ByteArray> array = ByteArray::create(buffer_size);
		for (int i = 0; i < buffer_size; i++) {
			//array->set(i, (unsigned char)tmp[i]);
			array->set(i, (unsigned char)128);
			if ((i+1) % 4 == 0)
				array->set(i, (unsigned char)127);
		}
		image_data = ImageData::create(IntSize(canvas_element->width(), canvas_element->height()), array);	
		//rendering_context->drawImage(image_data.get(), 0.0, 0.0, ec);
		//rendering_context->paintRenderingResultsToCanvas();
		//GraphicsContext* graphics_context = canvas_element->drawingContext();
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}
#endif
PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
		Int32Array* ptr, int event_wait_list, ExceptionCode& ec)
{

	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (mem != NULL) {
		cl_mem_id = mem->getCLMem();
		if (cl_mem_id == NULL) {
			printf("Error: cl_mem_id null\n");
			return NULL;
		}
	}
	// TODO(won.jeon) - NULL parameters need to be addressed later
	switch(blocking_read) {
		case true:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
					offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
					offset, buffer_size, ptr->data(), event_wait_list, NULL,&cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueReadBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: C_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
				//	ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST \n");
				//	ec = WebCLComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("WebCLCommandQueue::enqueueReadBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
						offset, buffer_size, event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		printf("ptr->data() [%d][%d][%d][%d][%d]\n", (ptr->data())[0], (ptr->data())[1], 
				(ptr->data())[2], (ptr->data())[3], (ptr->data())[4]);
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}


#if 0
// NOTE(won.jeon)
// it only works with integer-type of global/local workgroup size
// it does not work with vector-type of global/local workgroup size
// it will be deprecated
PassRefPtr<WebCLEvent>  WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, unsigned int work_dim, unsigned int global_work_offset, 
		unsigned int global_work_size, unsigned int local_work_size, int event_wait_list, ExceptionCode& ec)
{
	printf("WebCLCommandQueue::enqueueNDRangeKernel(int) event_wait_list=%d\n", event_wait_list);

	cl_int err = 0;
	cl_kernel cl_kernel_id = NULL;
	cl_event cl_event_id = NULL;

	size_t g_work_offset = global_work_offset;
	size_t g_work_size = global_work_size;
	size_t l_work_size = local_work_size;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (kernel != NULL) {
		cl_kernel_id = kernel->getCLKernel();
		if (cl_kernel_id == NULL) {
			printf("Error: cl_kernel_id null\n");
			return NULL;
		}
	}

	err = clEnqueueNDRangeKernel(m_cl_command_queue, cl_kernel_id, work_dim, 
			&g_work_offset, &g_work_size, &l_work_size, event_wait_list, NULL, &cl_event_id);
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueNDRangeKernel\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	} else {
		printf("Success: clEnqueueNDRangeKernel\n");
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		printf("m_num_events=%ld\n", m_num_events);
		return o;
	}
	return NULL;	

}
#endif

PassRefPtr<WebCLEvent>  WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, unsigned int work_dim, unsigned int global_work_offset, 
		Int32Array* global_work_size, Int32Array* local_work_size, int event_wait_list, ExceptionCode& ec)
{

	cl_int err = 0;
	cl_kernel cl_kernel_id = NULL;
	cl_event cl_event_id = NULL;

	size_t g_work_offset = global_work_offset;
	size_t *g_work_size = NULL;
	size_t *l_work_size = NULL;
	//size_t g1_work_size;
	//size_t l1_work_size;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (kernel != NULL) {
		cl_kernel_id = kernel->getCLKernel();
		if (cl_kernel_id == NULL) {
			printf("Error: cl_kernel_id null\n");
			printf("WebCLCommandQueue::enqueueNDRangeKernel(Int32Array) event_wait_list=%d\n", event_wait_list);
			return NULL;
		}
	}

	g_work_size = (size_t*)malloc(global_work_size->length() * sizeof(size_t));
	for (unsigned int i = 0; i < global_work_size->length(); i++) {
		g_work_size[i] = global_work_size->item(i);
	}
	l_work_size = (size_t*)malloc(local_work_size->length() * sizeof(size_t));
	for (unsigned int i = 0; i < local_work_size->length(); i++) {
		l_work_size[i] = local_work_size->item(i);
	}

	err = clEnqueueNDRangeKernel(m_cl_command_queue, cl_kernel_id, work_dim, 
			&g_work_offset, g_work_size, l_work_size, event_wait_list, NULL, &cl_event_id);

	if (err != CL_SUCCESS) {
		switch(err) {
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
				ec = WebCLComputeContext::INVALID_PROGRAM_EXECUTABLE;
				break;
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_KERNEL:
				printf("Error: CL_INVALID_KERNEL\n");
				ec = WebCLComputeContext::INVALID_KERNEL;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_KERNEL_ARGS:
				printf("Error: CL_INVALID_KERNEL_ARGS\n");
				ec = WebCLComputeContext::INVALID_KERNEL_ARGS;
				break;
			case CL_INVALID_WORK_DIMENSION:
				printf("Error: CL_INVALID_WORK_DIMENSION\n");
				ec = WebCLComputeContext::INVALID_WORK_DIMENSION;
				break;
				//case CL_INVALID_GLOBAL_WORK_SIZE:
				//	printf("Error: CL_INVALID_GLOBAL_WORK_SIZE\n");
				//	ec = WebCLComputeContext::INVALID_GLOBAL_WORK_SIZE;
				//	break;
			case CL_INVALID_GLOBAL_OFFSET:
				printf("Error: CL_INVALID_GLOBAL_OFFSET\n");
				ec = WebCLComputeContext::INVALID_GLOBAL_OFFSET;
				break;
			case CL_INVALID_WORK_GROUP_SIZE:
				printf("Error: CL_INVALID_WORK_GROUP_SIZE\n");
				ec = WebCLComputeContext::INVALID_WORK_GROUP_SIZE;
				break;
				//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				//	   printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				//	   ec = WebCLComputeContext::MISALIGNED_SUB_BUFFER_OFFSET;
				//	   break;
			case CL_INVALID_WORK_ITEM_SIZE:
				printf("Error: CL_INVALID_WORK_ITEM_SIZE\n");
				ec = WebCLComputeContext::INVALID_WORK_ITEM_SIZE;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
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
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;

	}
	return NULL;	

}

// TODO(siba.samal) Need to change user_data to Object type
void WebCLCommandQueue::finish(PassRefPtr<WebCLFinishCallback> notify, int user_data/*object userData*/, ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		return;
	}
	err = clFinish(m_cl_command_queue);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMAND_QUEUE \n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				printf("Unused Argument %d\n",user_data);
				ec = WebCLComputeContext::FAILURE;
				break;
		}		
	} else {
		m_finishCallback = notify;
		//m_finishCallback->handleEvent(user_data);			
		m_finishCallback->handleEvent(m_context);			
		return;
	}
	return;
}

void WebCLCommandQueue::flush( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clFlush(m_cl_command_queue);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY \n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		return;
	}
	return;
}

void WebCLCommandQueue::retainCLResource( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clRetainCommandQueue(m_cl_command_queue);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
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
		// TODO - Check if has to be really added
		RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, m_cl_command_queue);
		m_commandqueue_list.append(o);
		m_num_commandqueues++;
		return;
	}
	return;
}

void WebCLCommandQueue::releaseCLResource( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clReleaseCommandQueue(m_cl_command_queue);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE  :
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_OUT_OF_RESOURCES   :
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;
			case CL_OUT_OF_HOST_MEMORY    :
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		m_command_queue = NULL;
		return;
	}
	return; 
}

PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteImage(WebCLMem* mem_image, 
		bool blocking_write, 
		Int32Array* origin, 
		Int32Array* region, 
		HTMLCanvasElement* canvasElement, 
		int event_wait_list,
		ExceptionCode& ec)
{

	cl_int err = 0;
	cl_mem cl_mem_image = NULL;
	cl_event cl_event_id = NULL;


	size_t *origin_array = NULL;
	size_t *region_array = NULL;


	SharedBuffer* sharedBuffer = NULL;

	Image* image = NULL;
	const char* image_data = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}

	if (mem_image != NULL) {
		cl_mem_image = mem_image->getCLMem();
		if (cl_mem_image == NULL) {
			printf("Error: cl_mem_image null\n");
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}

	unsigned origin_array_length = origin->length();
	origin_array = (size_t*)malloc(origin_array_length*sizeof(size_t));
	for (unsigned int i = 0; i < origin_array_length; i++) {
		origin_array[i] = origin->item(i);
	}
	unsigned  region_array_length = region->length();
	region_array = (size_t*)malloc(region_array_length*sizeof(size_t));
	for (unsigned int i = 0; i < region_array_length; i++) {
		region_array[i] = region->item(i);
	}

	if (canvasElement != NULL) {
		image = canvasElement->copiedImage();
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
	}

	switch (blocking_write) {
		case true:
			err = clEnqueueWriteImage(m_cl_command_queue, cl_mem_image, CL_TRUE, 
					origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);
			break;
		case false:
			err = clEnqueueWriteImage(m_cl_command_queue, cl_mem_image, CL_FALSE, 
					origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);
			break;
	}
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteImage\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
			case CL_INVALID_IMAGE_SIZE:
				printf("Error: CL_INVALID_IMAGE_SIZE\n");
				ec = WebCLComputeContext::INVALID_IMAGE_SIZE;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec = WebCLComputeContext::INVALID_OPERATION;
				break;
				// CHECK(won.jeon) - from spec 1.1?
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	break;
				//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
				//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
				//	ec = EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
				//	break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
				printf("WebCLCommandQueue::enqueueWriteImage width=%d height=%d event_wait_list=%d\n", 
						canvasElement->width(), canvasElement->height(), event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

void WebCLCommandQueue::enqueueAcquireGLObjects(WebCLMem* mem_objects, int event_wait_list, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_mem cl_mem_ids = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	if ((mem_objects != NULL) && (mem_objects->isShared() == true)) {
		cl_mem_ids = mem_objects->getCLMem();
		if (cl_mem_ids == NULL) {
			printf("Error: cl_mem_ids null\n");
			ec = WebCLComputeContext::FAILURE;
			return;
		}
	}

	// TODO(won.jeon) - currently event-related arguments are ignored
	err = clEnqueueAcquireGLObjects(m_cl_command_queue, 1, &cl_mem_ids, 0, 0, 0);
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueAcquireGLObjects\n");
		switch (err) {
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_GL_OBJECT:
				printf("Error: CL_INVALID_GL_OBJECT\n");
				ec = WebCLComputeContext::INVALID_GL_OBJECT;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
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
				//TODO (siba samal) Handle Event 
				printf("WebCLCommandQueue::enqueueAcquireGLObjects event_wait_list=%d\n",
						event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		return;
	}
	return;
}

void WebCLCommandQueue::enqueueReleaseGLObjects(WebCLMem* mem_objects, int event_wait_list, ExceptionCode& ec)
{
	cl_mem cl_mem_ids = NULL;
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	if ((mem_objects != NULL) && (mem_objects->isShared())) {
		cl_mem_ids = mem_objects->getCLMem();
		if (cl_mem_ids == NULL) {
			printf("Error: cl_mem_ids null\n");
			ec = WebCLComputeContext::FAILURE;
			return;
		}
	}

	err = clEnqueueReleaseGLObjects(m_cl_command_queue, 1, &cl_mem_ids, 
			0, 0, 0);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_GL_OBJECT:
				printf("Error: CL_INVALID_GL_OBJECT\n");
				ec = WebCLComputeContext::INVALID_GL_OBJECT;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
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
				// TODO (siba samal) Handle Event related args
				printf("WebCLCommandQueue::enqueueReleaseGLObjects event_wait_list=%d\n",
						event_wait_list);
				ec = WebCLComputeContext::FAILURE;
				break;
		}
	} else {
		unsigned int i;

		for (i = 0; i < m_mem_list.size(); i++) {
			if ((m_mem_list[i].get())->getCLMem() == cl_mem_ids) {
				m_mem_list.remove(i);
				m_num_mems = m_mem_list.size();
				break;
			}
		}
		return;
	}
	return;
}

void WebCLCommandQueue::enqueueCopyBuffer(WebCLMem* src_buffer, WebCLMem* dst_buffer, int cb, ExceptionCode& ec)
{
	cl_mem cl_src_buffer_id = NULL;
	cl_mem cl_dst_buffer_id = NULL;
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	if (src_buffer != NULL) {
		cl_src_buffer_id = src_buffer->getCLMem();
		if (cl_src_buffer_id == NULL) {
			printf("Error: cl_src_buffer_id null\n");
			ec = WebCLComputeContext::FAILURE;
			return;
		}
	}
	if (dst_buffer != NULL) {
		cl_dst_buffer_id = dst_buffer->getCLMem();
		if (cl_dst_buffer_id == NULL) {
			printf("Error: cl_dst_buffer_id null\n");
			ec = WebCLComputeContext::FAILURE;
			return;
		}
	}
	err = clEnqueueCopyBuffer(m_cl_command_queue, cl_src_buffer_id, cl_dst_buffer_id,
			0, 0, cb, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueCopyBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT\n");
				ec = WebCLComputeContext::INVALID_MEM_OBJECT;
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec = WebCLComputeContext::INVALID_VALUE;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
			case CL_MEM_COPY_OVERLAP:
				printf("Error: CL_MEM_COPY_OVERLAP\n");
				ec = WebCLComputeContext::MEM_COPY_OVERLAP;
				break;
				/* CHECK(won.jeon) - defined in 1.1?
				   case CL_MISALIGNED_SUB_BUFFER_OFFSET:
				   printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
				   break;
				 */
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
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
		return;
	}
	return;
}

void WebCLCommandQueue::enqueueBarrier( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clEnqueueBarrier(m_cl_command_queue);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
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
				ec = WebCLComputeContext::FAILURE;
				printf("Error: Invaild Error Type\n");
				break;
		}
	} else {
		return;
	}
	return;
}


void WebCLCommandQueue::enqueueMarker(WebCLEvent* event, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_event cl_event_id = 0;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	if (event != NULL) {
		cl_event_id = event->getCLEvent();
		if (cl_event_id == NULL) {
			printf("cl_event_id null\n");
			ec = WebCLComputeContext::FAILURE;
			return ;
		}
	}

	err = clEnqueueMarker(m_cl_command_queue, &cl_event_id);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
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
				ec = WebCLComputeContext::FAILURE;
				printf("Error: Invaild Error Type\n");
				break;
		}
	} else {
		return;
	}
	return;
}

void WebCLCommandQueue::enqueueWaitForEvents(WebCLEventList* events, ExceptionCode& ec)
{
	cl_int err = 0;
	cl_event* cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	if (events != NULL) {
		cl_event_id = events->getCLEvents();
		if (cl_event_id == NULL) {
			printf("Error: cl_event null\n");
			ec = WebCLComputeContext::FAILURE;
			return;
		}
	}
	err = clEnqueueWaitForEvents(m_cl_command_queue, events->length(), cl_event_id);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_EVENT:
				printf("Error: CL_INVALID_EVENT\n");
				ec = WebCLComputeContext::INVALID_EVENT;
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
				ec = WebCLComputeContext::FAILURE;
				printf("Error: Invaild Error Type\n");
				break;
		}
	} else {
		return;
	}
	return;
}

PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueTask(WebCLKernel* kernel, 
		int event_wait_list, ExceptionCode& ec)
{

	cl_kernel cl_kernel_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;

	if (m_cl_command_queue == NULL) {
		printf("Error: Invalid Command Queue\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	if (kernel != NULL) {
		cl_kernel_id = kernel->getCLKernel();
		if (cl_kernel_id == NULL) {
			printf("Error: cl_kernel_id null\n");
			//TODO (siba samal) Handle enqueueTask  API
			printf("WebCLCommandQueue::enqueueTask event_wait_list=%d\n",
					event_wait_list);
			ec = WebCLComputeContext::FAILURE;
			return NULL;
		}
	}

	err = clEnqueueTask(m_cl_command_queue, cl_kernel_id, 0, NULL,&cl_event_id); 

	if (err != CL_SUCCESS) {
		printf("Error: clEnqueueWriteBuffer\n");
		switch (err) {
			case CL_INVALID_COMMAND_QUEUE:
				printf("Error: CL_INVALID_COMMAND_QUEUE\n");
				ec = WebCLComputeContext::INVALID_COMMAND_QUEUE;
				break;
			case CL_INVALID_CONTEXT:
				printf("Error: CL_INVALID_CONTEXT\n");
				ec = WebCLComputeContext::INVALID_CONTEXT;
				break;
			case CL_INVALID_PROGRAM_EXECUTABLE :
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE \n");
				ec = WebCLComputeContext::INVALID_PROGRAM_EXECUTABLE;
				break;				
			case CL_INVALID_KERNEL :
				printf("Error: CL_INVALID_KERNEL \n");
				ec = WebCLComputeContext::INVALID_KERNEL;
				break;
			case CL_INVALID_EVENT_WAIT_LIST:
				printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
				ec = WebCLComputeContext::INVALID_EVENT_WAIT_LIST;
				break;
			case CL_INVALID_KERNEL_ARGS :
				printf("Error: CL_INVALID_KERNEL_ARGS \n");
				ec = WebCLComputeContext::INVALID_KERNEL_ARGS;
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				break;
			case CL_INVALID_WORK_GROUP_SIZE :
				printf("Error: CL_INVALID_WORK_GROUP_SIZE \n");
				ec = WebCLComputeContext::FAILURE;
				break;		
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES;
				break;			
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE  \n");
				ec = WebCLComputeContext::MEM_OBJECT_ALLOCATION_FAILURE;
				break;	
			default:
				printf("Error: Invaild Error Type\n");
				ec = WebCLComputeContext::FAILURE;
				break;
		}

	} else {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, cl_event_id);
		m_event_list.append(o);
		m_num_events++;
		return o;
	}
	return NULL;
}

cl_command_queue WebCLCommandQueue::getCLCommandQueue()
{
	return m_cl_command_queue;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
