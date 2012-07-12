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
#include "WebCL.h"
#include <wtf/ArrayBuffer.h>
#include <wtf/ByteArray.h>
#include "CanvasPixelArray.h"
#include "WebCLException.h"

namespace WebCore {

WebCLCommandQueue::~WebCLCommandQueue()
{
}

PassRefPtr<WebCLCommandQueue> WebCLCommandQueue::create(WebCL* compute_context, cl_command_queue command_queue)
{
		return adoptRef(new WebCLCommandQueue(compute_context, command_queue));
}

WebCLCommandQueue::WebCLCommandQueue(WebCL* compute_context, cl_command_queue command_queue) : m_context(compute_context), m_cl_command_queue(command_queue)
{
}

WebCLGetInfo WebCLCommandQueue:: getInfo(int param_name, ExceptionCode& ec)
{
		cl_int err = 0;
		cl_uint uint_units = 0;
		cl_context cl_context_id = NULL;
		cl_device_id cl_device = NULL;
		cl_command_queue_properties queue_properties = NULL;
		RefPtr<WebCLContext> contextObj = NULL;
		RefPtr<WebCLDevice> deviceObj = NULL;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return WebCLGetInfo();
		}		
		switch(param_name)
		{

				case WebCL::QUEUE_REFERENCE_COUNT:
						err=clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
						if (err == CL_SUCCESS)
								return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
						break;
				case WebCL::QUEUE_CONTEXT:
						clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
						contextObj = WebCLContext::create(m_context, cl_context_id);
						if (err == CL_SUCCESS)
								return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
						break;
				case WebCL::QUEUE_DEVICE:
						clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_DEVICE, sizeof(cl_device_id), &cl_device, NULL);
						deviceObj = WebCLDevice::create(m_context, cl_device);
						if (err == CL_SUCCESS)
								return WebCLGetInfo(PassRefPtr<WebCLDevice>(deviceObj));
						break;
				case WebCL::QUEUE_PROPERTIES:
						   clGetCommandQueueInfo(m_cl_command_queue, CL_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties), &queue_properties, NULL);
						   return WebCLGetInfo(static_cast<unsigned int>(queue_properties));
							break;

				default:
						printf("Error: Unsupported Commans Queue Info type\n");
						ec = WebCLException::FAILURE;
						return WebCLGetInfo();
		}
		switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
						printf("Error: CL_INVALID_COMMAND_QUEUE \n");
						ec = WebCLException::INVALID_COMMAND_QUEUE;
						break;
				case CL_INVALID_VALUE:
						printf("Error: CL_INVALID_VALUE \n");
						ec = WebCLException::INVALID_VALUE;
						break;
				case CL_OUT_OF_RESOURCES:
						printf("Error: CL_OUT_OF_RESOURCES \n");
						ec = WebCLException::OUT_OF_RESOURCES;
						break;
				case CL_OUT_OF_HOST_MEMORY:
						printf("Error: CL_OUT_OF_HOST_MEMORY \n");
						ec = WebCLException::OUT_OF_HOST_MEMORY;
						break;
				default:
						printf("Error: Invaild Error Type\n");
						ec = WebCLException::FAILURE;
						break;
		}				
		return WebCLGetInfo();
}



void  WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write,
				int offset, int buffer_size, ArrayBufferView* ptr, WebCLEventList* events,
                                WebCLEvent* event, ExceptionCode& ec)
{
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		int eventsLength = 0;
		cl_event *cl_event_wait_lists = NULL;
        cl_event cl_event_id = NULL;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}	

		if (mem != NULL) {
				cl_mem_id = mem->getCLMem();
				if (cl_mem_id == NULL) {
						ec = WebCLException::INVALID_MEM_OBJECT;
						printf("Error: cl_mem_id null\n");
						return;
				}
		}

		if(ptr == NULL) {
				printf("Error: Invalid Buffer\n");
				ec = WebCLException::INVALID_MEM_OBJECT;
				return;
		}

		if (events != NULL) {
                cl_event_wait_lists = events->getCLEvents();
                eventsLength = events->length();
        }
        if (event != NULL) {
                cl_event_id = event->getCLEvent();
        }

		// TODO(siba samal) - NULL parameters need to be addressed later
		if(blocking_write)
				err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, offset, 
								buffer_size, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);
		else
				err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, offset, 
								buffer_size, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueWriteBuffer\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
								//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
								//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
								//	ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
								//	break;
								//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
								//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
								//	ec = WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
								//	break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;

				}

		} 
		return;

}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size,ImageData* ptr, WebCLEventList* events,WebCLEvent* event, ExceptionCode& ec)
{
    
    //printf(" Inside enqueueWriteBuffer image data new implementation \n ");
    
    
    
    cl_mem cl_mem_id = NULL;
    cl_int err = 0;
    //cl_event cl_event_id = NULL;
    //float* imageData = NULL; 
    
    int eventsLength = 0;
    cl_event *cl_event_wait_lists = NULL;
    cl_event cl_event_id = NULL;
    
    if (m_cl_command_queue == NULL) {
        printf("Error: Invalid Command Queue\n");
        ec = WebCL::FAILURE;
        return ;
    }   
    if (mem != NULL) {
        cl_mem_id = mem->getCLMem();
        //cl_mem cl_mem_id = (m_mem_list[0].get())->getCLMem();
        if (cl_mem_id == NULL) {
            printf("Error: cl_mem_id null\n");
            return ;
        }
    }
    
    if (events != NULL) {
        cl_event_wait_lists = events->getCLEvents();
        eventsLength = events->length();
    }
    if (event != NULL) {
        cl_event_id = event->getCLEvent();
    }
    
    
    
    unsigned char* buffer;
    if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()->data()!=NULL )
    {
        buffer = ptr->data()->data()->data();
        
        //printf( " buffer = %s ",buffer);
        
        buffer_size =  ptr->data()->data()->length();
        
        //printf(" buffer_size = %d \n ",buffer_size);
        
    }
    else
    {
        printf("Error: Invalid ImageData\n");
        ec = WebCL::FAILURE;
        return;
        
    }
    
    //Copy ImageData to a float array
    //	imageData = (float*) malloc(sizeof(float) * ptr->data()->length());
    //for(int i=0; i <  (int)ptr->data()->length(); i++)
    //		imageData[i] = ptr->data()->get(i);
    
    
    // TODO(won.jeon) - NULL parameters need to be addressed later
    if(blocking_write) 
    {
        err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
                                   offset, buffer_size, buffer, eventsLength, cl_event_wait_lists, &cl_event_id);
        //		err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
        //						offset, buffer_size, imageData, event_wait_list, NULL, &cl_event_id);
    }
    else
    {
        err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
                                   offset, buffer_size, buffer, eventsLength, cl_event_wait_lists, &cl_event_id);
        //		err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
        //						offset, buffer_size, imageData, event_wait_list, NULL, &cl_event_id);
    }
    
    if (err != CL_SUCCESS) {
        printf("Error: clEnqueueWriteBuffer\n");
        switch (err) {
            case CL_INVALID_COMMAND_QUEUE:
                ec = WebCL::INVALID_COMMAND_QUEUE;
                printf("Error: CL_INVALID_COMMAND_QUEUE\n");
                break;
            case CL_INVALID_CONTEXT:
                ec = WebCL::INVALID_CONTEXT;
                printf("Error: CL_INVALID_CONTEXT\n");
                break;
            case CL_INVALID_MEM_OBJECT:
                ec = WebCL::INVALID_MEM_OBJECT;
                printf("Error: CL_INVALID_MEM_OBJECT\n");
                break;
                
            case CL_INVALID_VALUE:
                ec = WebCL::INVALID_VALUE;
                printf("Error: CL_INVALID_VALUE\n");
                break;
            case CL_INVALID_EVENT_WAIT_LIST:
                ec = WebCL::INVALID_EVENT_WAIT_LIST;
                printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                ec = WebCL::MEM_OBJECT_ALLOCATION_FAILURE;
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                break;
            case CL_OUT_OF_HOST_MEMORY:
                ec = WebCL::OUT_OF_HOST_MEMORY;
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                break;
            default:
                ec = WebCL::FAILURE;
                printf("Error: Invaild Error Type\n");
                break;
        }
        
    } 
    return ;

    
        
}

/* PassRefPtr<WebCLEvent> WebCLCommandQueue::enqueueWriteBuffer(WebCLMem* mem, bool blocking_write, int offset, int buffer_size,
				ImageData* ptr, int event_wait_list, ExceptionCode& ec)
{
		
    
        printf(" Inside old implementation  ");
    
        printf("WebCLCommandQueue::enqueueWriteBuffer(ImageData) offset=%d buffer_size=%d event_wait_list=%d\n",
						offset, buffer_size, event_wait_list);

		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;
		//float* imageData = NULL; 

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCL::FAILURE;
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

        unsigned char* buffer;
        if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()->data()!=NULL )
        {
            buffer = ptr->data()->data()->data();
            buffer_size =  ptr->data()->data()->length();

        }
        else
        {
            printf("Error: Invalid ImageData\n");
            ec = WebCL::FAILURE;
            return NULL;
        
        }
        
		//Copy ImageData to a float array
		//	imageData = (float*) malloc(sizeof(float) * ptr->data()->length());
		//for(int i=0; i <  (int)ptr->data()->length(); i++)
		//		imageData[i] = ptr->data()->get(i);


		// TODO(won.jeon) - NULL parameters need to be addressed later
		if(blocking_write) 
				err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
								offset, buffer_size, buffer, event_wait_list, NULL, &cl_event_id);
		//		err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
		//						offset, buffer_size, imageData, event_wait_list, NULL, &cl_event_id);
		else
				err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
								offset, buffer_size, buffer, event_wait_list, NULL, &cl_event_id);
		//		err = clEnqueueWriteBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
		//						offset, buffer_size, imageData, event_wait_list, NULL, &cl_event_id);

		//if(imageData)
		//		free(imageData);

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueWriteBuffer\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								ec = WebCL::INVALID_COMMAND_QUEUE;
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								break;
						case CL_INVALID_CONTEXT:
								ec = WebCL::INVALID_CONTEXT;
								printf("Error: CL_INVALID_CONTEXT\n");
								break;
						case CL_INVALID_MEM_OBJECT:
								ec = WebCL::INVALID_MEM_OBJECT;
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								break;

						case CL_INVALID_VALUE:
								ec = WebCL::INVALID_VALUE;
								printf("Error: CL_INVALID_VALUE\n");
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								ec = WebCL::INVALID_EVENT_WAIT_LIST;
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								ec = WebCL::MEM_OBJECT_ALLOCATION_FAILURE;
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								ec = WebCL::OUT_OF_HOST_MEMORY;
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								break;
						default:
								ec = WebCL::FAILURE;
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
*/

/*    
    
PassRefPtr<WebCLEvent>  WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read,
				int offset, int buffer_size, ImageData* ptr, int event_wait_list, ExceptionCode& ec)
{
		printf("WebCLCommandQueue::enqueueWriteBuffer(ImageData) offset=%d buffer_size=%d event_wait_list=%d\n",
						offset, buffer_size, event_wait_list);

		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::FAILURE;
				return NULL;
		}
		if (mem != NULL) {
				cl_mem_id = mem->getCLMem();
				if (cl_mem_id == NULL) {
						printf("Error: cl_mem_id null\n");
						return NULL;
				}


		}

		//printf("\nLength for imageData = %d buffer_size = %d \n", ptr->data()->length(), buffer_size);

		//Copy ImageData to a float array
		//float* imageData = (float*) malloc(sizeof(float) * ptr->data()->length());

		//unsigned char* buffer = ptr->data()->data()->data();
		//buffer_size =  ptr->data()->data()->length();
    
        unsigned char* buffer;
        if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()->data()!=NULL )
        {
            buffer = ptr->data()->data()->data();
            buffer_size =  ptr->data()->data()->length();
        
        }
        else
        {
            printf("Error: Invalid ImageData\n");
            ec = WebCL::FAILURE;
            return NULL;
        
        }



		// TODO(siba samal) - NULL parameters need to be addressed later
		if(blocking_read)
				err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
								offset,buffer_size, buffer, event_wait_list, NULL, &cl_event_id);
		//		err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
		//						offset,buffer_size, imageData, event_wait_list, NULL, &cl_event_id);
		else
				err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
								offset, buffer_size,  buffer, event_wait_list, NULL, &cl_event_id);
		//		err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
		//						offset, buffer_size,  imageData, event_wait_list, NULL, &cl_event_id);


		//for(int i=0; i <  (int)ptr->data()->length(); i++)
		//{
				//printf("%f  %d ...\n", imageData[i],i );		
		//		ptr->data()->set(i, (double)imageData[i]);
		//}

		//printf("End of Copy    ptr->data()->length() = %d buffer_size = %d \n", ptr->data()->length(), buffer_size);

	//	if(imageData)
	//			free(imageData);

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueReadBuffer\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
								//case CL_MISALIGNED_SUB_BUFFER_OFFSET :
								//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
								//	ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
								//	break;
								//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
								//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
								//	ec = WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
								//	break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES \n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								printf("WebCLCommandQueue::enqueueReadBuffer() offset=%d buffer_size=%d event_wait_list=%d\n", offset, buffer_size, event_wait_list);
								ec = WebCLException::FAILURE;
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
 
*/ 
    
void WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read,int offset, int buffer_size, ImageData* ptr, WebCLEventList* events,WebCLEvent* event, ExceptionCode& ec)
{
    //printf(" Inside enqueueReadBuffer image data new implementation \n ");
    
    cl_mem cl_mem_id = NULL;
    cl_int err = 0;
    cl_event cl_event_id = NULL;
    
    int eventsLength = 0;
    cl_event *cl_event_wait_lists = NULL;
    //cl_event cl_event_id = NULL;
    
    if (m_cl_command_queue == NULL) {
        printf("Error: Invalid Command Queue\n");
        ec = WebCLException::FAILURE;
        return ;
    }
    if (mem != NULL) {
        cl_mem_id = mem->getCLMem();
        if (cl_mem_id == NULL) {
            printf("Error: cl_mem_id null\n");
            return ;
        }
        
        
    }
    
    if (events != NULL) {
        cl_event_wait_lists = events->getCLEvents();
        eventsLength = events->length();
    }
    if (event != NULL) {
        cl_event_id = event->getCLEvent();
    }
    
    //printf("\nLength for imageData = %d buffer_size = %d \n", ptr->data()->length(), buffer_size);
    
    //Copy ImageData to a float array
    //float* imageData = (float*) malloc(sizeof(float) * ptr->data()->length());
    
    //unsigned char* buffer = ptr->data()->data()->data();
    //buffer_size =  ptr->data()->data()->length();
    
    unsigned char* buffer;
    if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()->data()!=NULL )
    {
        buffer = ptr->data()->data()->data();
        printf(" buffer => %s ",buffer);
        buffer_size =  ptr->data()->data()->length();
        printf(" buffer_size => %d ",buffer_size);
    }
    else
    {
        printf("Error: Invalid ImageData\n");
        ec = WebCL::FAILURE;
        return ;
        
    }
    
    
    
    // TODO(siba samal) - NULL parameters need to be addressed later
    if(blocking_read)
    {
        err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
                                  offset,buffer_size, buffer, eventsLength, cl_event_wait_lists, &cl_event_id);
    //		err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, 
    //						offset,buffer_size, imageData, event_wait_list, NULL, &cl_event_id);
    }
    else
    {
        err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
                                  offset, buffer_size,  buffer, eventsLength, cl_event_wait_lists, &cl_event_id);
    //		err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, 
    //						offset, buffer_size,  imageData, event_wait_list, NULL, &cl_event_id);
    }
    
    //for(int i=0; i <  (int)ptr->data()->length(); i++)
    //{
    //printf("%f  %d ...\n", imageData[i],i );		
    //		ptr->data()->set(i, (double)imageData[i]);
    //}
    
    //printf("End of Copy    ptr->data()->length() = %d buffer_size = %d \n", ptr->data()->length(), buffer_size);
    
    //	if(imageData)
    //			free(imageData);
    
    if (err != CL_SUCCESS) {
        printf("Error: clEnqueueReadBuffer\n");
        switch (err) {
            case CL_INVALID_COMMAND_QUEUE:
                printf("Error: CL_INVALID_COMMAND_QUEUE\n");
                ec = WebCLException::INVALID_COMMAND_QUEUE;
                break;
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_MEM_OBJECT:
                printf("Error: CL_INVALID_MEM_OBJECT\n");
                ec = WebCLException::INVALID_MEM_OBJECT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_EVENT_WAIT_LIST:
                printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
                ec = WebCLException::INVALID_EVENT_WAIT_LIST;
                break;
                //case CL_MISALIGNED_SUB_BUFFER_OFFSET :
                //	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
                //	ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
                //	break;
                //case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
                //	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
                //	ec = WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
                //	break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES \n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                //printf("WebCLCommandQueue::enqueueReadBuffer() offset=%d buffer_size=%d event_wait_list=%d\n", offset, buffer_size, event_wait_list);
                ec = WebCLException::FAILURE;
                break;
                
        }
        
    } 
    return ;
}





void WebCLCommandQueue::enqueueReadImage(WebCLMem* mem, bool blocking_read,
		Int32Array* origin, Int32Array* region,int rowPitch,int slicePitch, ArrayBufferView* ptr, WebCLEventList* events,
                        WebCLEvent* event, ExceptionCode& ec)
{
	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	cl_event cl_event_id = NULL;
	cl_event* cl_event_wait_lists = NULL;
	int eventsLength = 0;

	size_t *origin_array = NULL;
	size_t *region_array = NULL;

	if (m_cl_command_queue == NULL) {
			printf("Error: Invalid Command Queue\n");
			ec = WebCLException::FAILURE;
			return;
	}
	if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
					printf("Error: cl_mem_id null\n");
					return;
			}


	}
	if (events != NULL) {
            cl_event_wait_lists = events->getCLEvents();
            eventsLength = events->length();
    }
    if (event != NULL) {
            cl_event_id = event->getCLEvent();
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

	// TODO(siba samal) - NULL parameters need to be addressed later
	if(blocking_read)
			err = clEnqueueReadImage(m_cl_command_queue, cl_mem_id, CL_TRUE, origin_array, region_array, rowPitch, slicePitch, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);
	else
			err = clEnqueueReadImage(m_cl_command_queue, cl_mem_id, CL_FALSE, origin_array, region_array, rowPitch, slicePitch, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);

	if (err != CL_SUCCESS) {
			printf("Error: clEnqueueReadImage\n");
			switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
							printf("Error: CL_INVALID_COMMAND_QUEUE\n");
							ec = WebCLException::INVALID_COMMAND_QUEUE;
							break;
					case CL_INVALID_CONTEXT:
							printf("Error: CL_INVALID_CONTEXT\n");
							ec = WebCLException::INVALID_CONTEXT;
							break;
					case CL_INVALID_MEM_OBJECT:
							printf("Error: CL_INVALID_MEM_OBJECT\n");
							ec = WebCLException::INVALID_MEM_OBJECT;
							break;
					case CL_INVALID_VALUE:
							printf("Error: CL_INVALID_VALUE\n");
							ec = WebCLException::INVALID_VALUE;
							break;
					case CL_INVALID_EVENT_WAIT_LIST:
							printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
							ec = WebCLException::INVALID_EVENT_WAIT_LIST;
							break;
							//case CL_MISALIGNED_SUB_BUFFER_OFFSET :
							//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
							//	ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
							//	break;
							//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
							//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
							//	ec = WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
							//	break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
							printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
							ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
							break;
					case CL_OUT_OF_RESOURCES:
							printf("Error: CL_OUT_OF_RESOURCES \n");
							ec = WebCLException::OUT_OF_RESOURCES;
							break;
					case CL_OUT_OF_HOST_MEMORY:
							printf("Error: CL_OUT_OF_HOST_MEMORY\n");
							ec = WebCLException::OUT_OF_HOST_MEMORY;
							break;
					default:
							printf("Error: Invaild Error Type\n");
							ec = WebCLException::FAILURE;
							break;

			}
	}
	return ;
}



void  WebCLCommandQueue::enqueueReadBuffer(WebCLMem* mem, bool blocking_read,int offset, int buffer_size, ArrayBufferView* ptr, WebCLEventList* events,WebCLEvent* event, ExceptionCode& ec)
{
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;
		cl_event* cl_event_wait_lists = NULL;
		int eventsLength = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::FAILURE;
				return; 
		}
		if (mem != NULL) {
				cl_mem_id = mem->getCLMem();
				if (cl_mem_id == NULL) {
						printf("Error: cl_mem_id null\n");
						return;
				}


		}
		if (events != NULL) {
                cl_event_wait_lists = events->getCLEvents();
                eventsLength = events->length();
        }
        if (event != NULL) {
                cl_event_id = event->getCLEvent();
        }

		// TODO(siba samal) - NULL parameters need to be addressed later
		if(blocking_read)
				err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_TRUE, offset, buffer_size, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);
		else
				err = clEnqueueReadBuffer(m_cl_command_queue, cl_mem_id, CL_FALSE, offset, buffer_size, ptr->baseAddress(), eventsLength, cl_event_wait_lists, &cl_event_id);

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueReadBuffer\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
								//case CL_MISALIGNED_SUB_BUFFER_OFFSET :
								//	printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET \n");
								//	ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
								//	break;
								//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  :
								//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST  \n");
								//	ec = WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
								//	break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES \n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;

				}
		}
		return ;
}



void  WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, Int32Array* offsets, 
				Int32Array* global_work_size, Int32Array* local_work_size, WebCLEventList* events,
				WebCLEvent* event, ExceptionCode& ec)
{

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;
		cl_event *cl_event_wait_lists = NULL;
		cl_event cl_event_id = NULL;
		int eventsLength = 0; 

		size_t *g_work_size   = NULL;
		size_t *l_work_size   = NULL;
		size_t *g_work_offset = NULL;
		int work_dim = 0; 

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return ;
		}
		if (kernel != NULL) {
				cl_kernel_id = kernel->getCLKernel();
				if (cl_kernel_id == NULL) {
						ec = WebCLException::INVALID_KERNEL;
						printf("Error: cl_kernel_id null\n");
						return ;
				}
		}

		if (events != NULL) {
				cl_event_wait_lists = events->getCLEvents();
				eventsLength = events->length();
		}
		if (event != NULL) {
				cl_event_id = event->getCLEvent();
		}

		if(global_work_size != NULL) {
				g_work_size = (size_t*)malloc(global_work_size->length() * sizeof(size_t));
				for (unsigned int i = 0; i < global_work_size->length(); i++) {
						g_work_size[i] = global_work_size->item(i);
				}
				work_dim = global_work_size->length();
		}
		if(local_work_size != NULL) {
				l_work_size = (size_t*)malloc(local_work_size->length() * sizeof(size_t));
				for (unsigned int i = 0; i < local_work_size->length(); i++) {
						l_work_size[i] = local_work_size->item(i);
				}
		}
		if(offsets != NULL)
		{
				g_work_offset = (size_t*)malloc(offsets->length() * sizeof(size_t));
				for (unsigned int i = 0; i < offsets->length(); i++) {
						g_work_offset[i] = offsets->item(i);
				}
		}


		err = clEnqueueNDRangeKernel(m_cl_command_queue, cl_kernel_id, work_dim, 
						g_work_offset, g_work_size, l_work_size, eventsLength, 
						cl_event_wait_lists, &cl_event_id);


		// Is free needed ??
		free(g_work_size);
		free(l_work_size);
		free(g_work_offset);

		if (err != CL_SUCCESS) {
				switch(err) {
						case CL_INVALID_PROGRAM_EXECUTABLE:
								printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
								ec = WebCLException::INVALID_PROGRAM_EXECUTABLE;
								break;
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_KERNEL:
								printf("Error: CL_INVALID_KERNEL\n");
								ec = WebCLException::INVALID_KERNEL;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_KERNEL_ARGS:
								printf("Error: CL_INVALID_KERNEL_ARGS\n");
								ec = WebCLException::INVALID_KERNEL_ARGS;
								break;
						case CL_INVALID_WORK_DIMENSION:
								printf("Error: CL_INVALID_WORK_DIMENSION\n");
								ec = WebCLException::INVALID_WORK_DIMENSION;
								break;
								//case CL_INVALID_GLOBAL_WORK_SIZE:
								//	printf("Error: CL_INVALID_GLOBAL_WORK_SIZE\n");
								//	ec = WebCLException::INVALID_GLOBAL_WORK_SIZE;
								//	break;
						case CL_INVALID_GLOBAL_OFFSET:
								printf("Error: CL_INVALID_GLOBAL_OFFSET\n");
								ec = WebCLException::INVALID_GLOBAL_OFFSET;
								break;
						case CL_INVALID_WORK_GROUP_SIZE:
								printf("Error: CL_INVALID_WORK_GROUP_SIZE\n");
								ec = WebCLException::INVALID_WORK_GROUP_SIZE;
								break;
								//case CL_MISALIGNED_SUB_BUFFER_OFFSET:
								//	   printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
								//	   ec = WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
								//	   break;
						case CL_INVALID_WORK_ITEM_SIZE:
								printf("Error: CL_INVALID_WORK_ITEM_SIZE\n");
								ec = WebCLException::INVALID_WORK_ITEM_SIZE;
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec = WebCLException::INVALID_IMAGE_SIZE;
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:	
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;
				}
		} 
		return;	
}
void WebCLCommandQueue::finish(ExceptionCode& ec)
{
		cl_int err = 0;

		if (m_cl_command_queue == NULL) {
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				printf("Error: Invalid Command Queue\n");
				return;
		}
		err = clFinish(m_cl_command_queue);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMAND_QUEUE \n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY \n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES \n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;
				}		
		}
		return;
}
/*
// TODO(siba.samal) Need to change user_data to Object type
void WebCLCommandQueue::finish(PassRefPtr<WebCLFinishCallback> notify, int user_data, ExceptionCode& ec)//object userData
{
cl_int err = 0;

if (m_cl_command_queue == NULL) {
ec = WebCLException::INVALID_COMMAND_QUEUE;
printf("Error: Invalid Command Queue\n");
return;
}
err = clFinish(m_cl_command_queue);
if (err != CL_SUCCESS) {
switch (err) {
case CL_INVALID_COMMAND_QUEUE:
printf("Error: CL_INVALID_COMAND_QUEUE \n");
ec = WebCLException::INVALID_COMMAND_QUEUE;
break;
case CL_OUT_OF_HOST_MEMORY:
printf("Error: CL_OUT_OF_HOST_MEMORY \n");
ec = WebCLException::OUT_OF_HOST_MEMORY;
break;
case CL_OUT_OF_RESOURCES:
printf("Error: CL_OUT_OF_RESOURCES \n");
ec = WebCLException::OUT_OF_RESOURCES;
break;
default:
printf("Error: Invaild Error Type\n");
printf("Unused Argument %d\n",user_data);
ec = WebCLException::FAILURE;
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
 */

void WebCLCommandQueue::flush( ExceptionCode& ec)
{
		cl_int err = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::FAILURE;
				return;
		}
		err = clFlush(m_cl_command_queue);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY \n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES \n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;
				}
		} else {
				return;
		}
		return;
}


void WebCLCommandQueue::releaseCL( ExceptionCode& ec)
{
		cl_int err = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		err = clReleaseCommandQueue(m_cl_command_queue);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE  :
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_OUT_OF_RESOURCES   :
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY    :
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
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
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return NULL;
		}

		if (mem_image != NULL) {
				cl_mem_image = mem_image->getCLMem();
				if (cl_mem_image == NULL) {
						printf("Error: cl_mem_image null\n");
						ec = WebCLException::FAILURE;
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
						ec = WebCLException::FAILURE;
						return NULL;
				} else {
						image_data = sharedBuffer->data();

						if (image_data == NULL) {
								printf("Error: image_data null\n");
								ec = WebCLException::FAILURE;
								return NULL;
						}
				}
		}

		if(blocking_write)
				err = clEnqueueWriteImage(m_cl_command_queue, cl_mem_image, CL_TRUE, 
								origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);
		else
				err = clEnqueueWriteImage(m_cl_command_queue, cl_mem_image, CL_FALSE, 
								origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueWriteImage\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec = WebCLException::INVALID_IMAGE_SIZE;
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec = WebCLException::INVALID_OPERATION;
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
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								printf("WebCLCommandQueue::enqueueWriteImage width=%d height=%d event_wait_list=%d\n", 
												canvasElement->width(), canvasElement->height(), event_wait_list);
								ec = WebCLException::FAILURE;
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

void WebCLCommandQueue::enqueueAcquireGLObjects(WebCLMem* mem_objects, WebCLEventList* events,
					 WebCLEvent* event, ExceptionCode& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_ids = NULL;
		cl_event *cl_event_wait_lists = NULL;
        cl_event cl_event_id = NULL;
        int eventsLength = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		if ((mem_objects != NULL) && (mem_objects->isShared() == true)) {
				cl_mem_ids = mem_objects->getCLMem();
				if (cl_mem_ids == NULL) {
						printf("Error: cl_mem_ids null\n");
						ec = WebCLException::FAILURE;
						return;
				}
		}
		if (events != NULL) {
                cl_event_wait_lists = events->getCLEvents();
                eventsLength = events->length();
        }
        if (event != NULL) {
                cl_event_id = event->getCLEvent();
        }
		err = clEnqueueAcquireGLObjects(m_cl_command_queue, 1, &cl_mem_ids, eventsLength, cl_event_wait_lists, &cl_event_id);
		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueAcquireGLObjects\n");
				switch (err) {
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_GL_OBJECT:
								printf("Error: CL_INVALID_GL_OBJECT\n");
								ec = WebCLException::INVALID_GL_OBJECT;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;
				}
		} else {
				return;
		}
		return;
}

void WebCLCommandQueue::enqueueReleaseGLObjects(WebCLMem* mem_objects, WebCLEventList* events,
                     WebCLEvent* event, ExceptionCode& ec)
{
		cl_mem cl_mem_ids = NULL;
		cl_int err = 0;
		cl_event *cl_event_wait_lists = NULL;
        cl_event cl_event_id = NULL;
        int eventsLength = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		if ((mem_objects != NULL) && (mem_objects->isShared())) {
				cl_mem_ids = mem_objects->getCLMem();
				if (cl_mem_ids == NULL) {
						printf("Error: cl_mem_ids null\n");
						ec = WebCLException::INVALID_MEM_OBJECT;
						return;
				}
		}
		if (events != NULL) {
                cl_event_wait_lists = events->getCLEvents();
                eventsLength = events->length();
        }
        if (event != NULL) {
                cl_event_id = event->getCLEvent();
        }

		err = clEnqueueReleaseGLObjects(m_cl_command_queue, 1, &cl_mem_ids, 
					eventsLength, cl_event_wait_lists, &cl_event_id);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_GL_OBJECT:
								printf("Error: CL_INVALID_GL_OBJECT\n");
								ec = WebCLException::INVALID_GL_OBJECT;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
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
void WebCLCommandQueue::enqueueCopyImage(WebCLMem* src_buffer, WebCLMem* dst_buffer,Int32Array* srcOffset,Int32Array* dstOffset,Int32Array* region, WebCLEventList* events,
		WebCLEvent* event,ExceptionCode& ec)
{
	cl_mem cl_src_buffer_id = NULL;
	cl_mem cl_dst_buffer_id = NULL;
	cl_int err = 0;

	cl_event *cl_event_wait_lists = NULL;
	cl_event cl_event_id = NULL;
	int eventsLength = 0;

	size_t *srcOffset_array = NULL;
	size_t *dstOffset_array = NULL;
	size_t *region_array = NULL;

	if (m_cl_command_queue == NULL) {
			printf("Error: Invalid Command Queue\n");
			ec = WebCLException::INVALID_COMMAND_QUEUE;
			return;
	}
	if (src_buffer != NULL) {
			cl_src_buffer_id = src_buffer->getCLMem();
			if (cl_src_buffer_id == NULL) {
					printf("Error: cl_src_buffer_id null\n");
					ec = WebCLException::INVALID_MEM_OBJECT;
					return;
			}
	}
	if (dst_buffer != NULL) {
			cl_dst_buffer_id = dst_buffer->getCLMem();
			if (cl_dst_buffer_id == NULL) {
					printf("Error: cl_dst_buffer_id null\n");
					ec = WebCLException::FAILURE;
					return;
			}
	}

	unsigned srcOffset_array_length = srcOffset->length();
	srcOffset_array = (size_t*)malloc(srcOffset_array_length*sizeof(size_t));
	for (unsigned int i = 0; i < srcOffset_array_length; i++) {
		srcOffset_array[i] = srcOffset->item(i);
	}

	unsigned dstOffset_array_length = dstOffset->length();
	dstOffset_array = (size_t*)malloc(dstOffset_array_length*sizeof(size_t));
	for (unsigned int i = 0; i < dstOffset_array_length; i++) {
		dstOffset_array[i] = dstOffset->item(i);
	}

	unsigned  region_array_length = region->length();
	region_array = (size_t*)malloc(region_array_length*sizeof(size_t));
	for (unsigned int i = 0; i < region_array_length; i++) {
			region_array[i] = region->item(i);
	}


	if (events != NULL) {
			cl_event_wait_lists = events->getCLEvents();
			eventsLength = events->length();
	}
	if (event != NULL) {
			cl_event_id = event->getCLEvent();
	}

	err = clEnqueueCopyImage(m_cl_command_queue, cl_src_buffer_id, cl_dst_buffer_id,
			srcOffset_array, dstOffset_array, region_array, eventsLength, cl_event_wait_lists, &cl_event_id);

	if (err != CL_SUCCESS) {
			printf("Error: clEnqueueCopyImage\n");
			switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
							printf("Error: CL_INVALID_COMAND_QUEUE\n");
							ec = WebCLException::INVALID_COMMAND_QUEUE;
							break;
					case CL_INVALID_CONTEXT:
							printf("Error: CL_INVALID_CONTEXT\n");
							ec = WebCLException::INVALID_CONTEXT;
							break;
					case CL_INVALID_MEM_OBJECT:
							printf("Error: CL_INVALID_MEM_OBJECT\n");
							ec = WebCLException::INVALID_MEM_OBJECT;
							break;
					case CL_INVALID_VALUE:
							printf("Error: CL_INVALID_VALUE\n");
							ec = WebCLException::INVALID_VALUE;
							break;
					case CL_INVALID_EVENT_WAIT_LIST:
							printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
							ec = WebCLException::INVALID_EVENT_WAIT_LIST;
							break;
					case CL_MEM_COPY_OVERLAP:
							printf("Error: CL_MEM_COPY_OVERLAP\n");
							ec = WebCLException::MEM_COPY_OVERLAP;
							break;
							//CHECK(won.jeon) - defined in 1.1?
							   //case CL_MISALIGNED_SUB_BUFFER_OFFSET:
							   //printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
							   //break;

					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
							printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
							ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
							break;
					case CL_OUT_OF_RESOURCES:
							printf("Error: CL_OUT_OF_RESOURCES\n");
							ec = WebCLException::OUT_OF_RESOURCES;
							break;
					case CL_OUT_OF_HOST_MEMORY:
							printf("Error: CL_OUT_OF_HOST_MEMORY\n");
							ec = WebCLException::OUT_OF_HOST_MEMORY;
							break;
					default:
							printf("Error: Invaild Error Type\n");
							ec = WebCLException::FAILURE;
							break;
			}
	} else {
			return;
	}
	return;}

void WebCLCommandQueue::enqueueCopyBuffer(WebCLMem* src_buffer, WebCLMem* dst_buffer, int cb, ExceptionCode& ec)
{
		cl_mem cl_src_buffer_id = NULL;
		cl_mem cl_dst_buffer_id = NULL;
		cl_int err = 0;

		if (m_cl_command_queue == NULL) {
				printf("Error: Invalid Command Queue\n");
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		if (src_buffer != NULL) {
				cl_src_buffer_id = src_buffer->getCLMem();
				if (cl_src_buffer_id == NULL) {
						printf("Error: cl_src_buffer_id null\n");
						ec = WebCLException::INVALID_MEM_OBJECT;
						return;
				}
		}
		if (dst_buffer != NULL) {
				cl_dst_buffer_id = dst_buffer->getCLMem();
				if (cl_dst_buffer_id == NULL) {
						printf("Error: cl_dst_buffer_id null\n");
						ec = WebCLException::FAILURE;
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
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_MEM_OBJECT:
								printf("Error: CL_INVALID_MEM_OBJECT\n");
								ec = WebCLException::INVALID_MEM_OBJECT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_MEM_COPY_OVERLAP:
								printf("Error: CL_MEM_COPY_OVERLAP\n");
								ec = WebCLException::MEM_COPY_OVERLAP;
								break;
								//CHECK(won.jeon) - defined in 1.1?
								   //case CL_MISALIGNED_SUB_BUFFER_OFFSET:
								   //printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
								   //break;

						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
								break;
				}
		} else {
				return;
		}
		return;
}

void WebCLCommandQueue::enqueueCopyBuffer(WebCLMem* src_buffer, WebCLMem* dst_buffer,int srcOffset,int dstOffset,int sizeInBytes, WebCLEventList* events,
		WebCLEvent* event,ExceptionCode& ec)
{
	cl_mem cl_src_buffer_id = NULL;
	cl_mem cl_dst_buffer_id = NULL;
	cl_int err = 0;

	cl_event *cl_event_wait_lists = NULL;
	cl_event cl_event_id = NULL;
	int eventsLength = 0;

	if (m_cl_command_queue == NULL) {
			printf("Error: Invalid Command Queue\n");
			ec = WebCLException::INVALID_COMMAND_QUEUE;
			return;
	}
	if (src_buffer != NULL) {
			cl_src_buffer_id = src_buffer->getCLMem();
			if (cl_src_buffer_id == NULL) {
					printf("Error: cl_src_buffer_id null\n");
					ec = WebCLException::INVALID_MEM_OBJECT;
					return;
			}
	}
	if (dst_buffer != NULL) {
			cl_dst_buffer_id = dst_buffer->getCLMem();
			if (cl_dst_buffer_id == NULL) {
					printf("Error: cl_dst_buffer_id null\n");
					ec = WebCLException::FAILURE;
					return;
			}
	}

	if (events != NULL) {
		cl_event_wait_lists = events->getCLEvents();
		eventsLength = events->length();
	}
	if (event != NULL) {
		cl_event_id = event->getCLEvent();
	}

	err = clEnqueueCopyBuffer(m_cl_command_queue, cl_src_buffer_id, cl_dst_buffer_id,
			srcOffset, dstOffset,sizeInBytes, eventsLength,cl_event_wait_lists, &cl_event_id);
	if (err != CL_SUCCESS) {
			printf("Error: clEnqueueCopyBuffer\n");
			switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
							printf("Error: CL_INVALID_COMAND_QUEUE\n");
							ec = WebCLException::INVALID_COMMAND_QUEUE;
							break;
					case CL_INVALID_CONTEXT:
							printf("Error: CL_INVALID_CONTEXT\n");
							ec = WebCLException::INVALID_CONTEXT;
							break;
					case CL_INVALID_MEM_OBJECT:
							printf("Error: CL_INVALID_MEM_OBJECT\n");
							ec = WebCLException::INVALID_MEM_OBJECT;
							break;
					case CL_INVALID_VALUE:
							printf("Error: CL_INVALID_VALUE\n");
							ec = WebCLException::INVALID_VALUE;
							break;
					case CL_INVALID_EVENT_WAIT_LIST:
							printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
							ec = WebCLException::INVALID_EVENT_WAIT_LIST;
							break;
					case CL_MEM_COPY_OVERLAP:
							printf("Error: CL_MEM_COPY_OVERLAP\n");
							ec = WebCLException::MEM_COPY_OVERLAP;
							break;
							/* CHECK(won.jeon) - defined in 1.1?
							   case CL_MISALIGNED_SUB_BUFFER_OFFSET:
							   printf("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
							   break;
							 */
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
							printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
							ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
							break;
					case CL_OUT_OF_RESOURCES:
							printf("Error: CL_OUT_OF_RESOURCES\n");
							ec = WebCLException::OUT_OF_RESOURCES;
							break;
					case CL_OUT_OF_HOST_MEMORY:
							printf("Error: CL_OUT_OF_HOST_MEMORY\n");
							ec = WebCLException::OUT_OF_HOST_MEMORY;
							break;
					default:
							printf("Error: Invaild Error Type\n");
							ec = WebCLException::FAILURE;
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
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		err = clEnqueueBarrier(m_cl_command_queue);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								ec = WebCLException::FAILURE;
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
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		if (event != NULL) {
				cl_event_id = event->getCLEvent();
				if (cl_event_id == NULL) {
						printf("cl_event_id null\n");
						ec = WebCLException::INVALID_EVENT;
						return ;
				}
		}

		err = clEnqueueMarker(m_cl_command_queue, &cl_event_id);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								ec = WebCLException::FAILURE;
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
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return;
		}
		if (events != NULL) {
				cl_event_id = events->getCLEvents();
				if (cl_event_id == NULL) {
						printf("Error: cl_event null\n");
						ec = WebCLException::INVALID_EVENT;
						return;
				}
		}
		err = clEnqueueWaitForEvents(m_cl_command_queue, events->length(), cl_event_id);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_EVENT:
								printf("Error: CL_INVALID_EVENT\n");
								ec = WebCLException::INVALID_EVENT;
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec = WebCLException::INVALID_VALUE;
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						default:
								ec = WebCLException::FAILURE;
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
				ec = WebCLException::INVALID_COMMAND_QUEUE;
				return NULL;
		}
		if (kernel != NULL) {
				cl_kernel_id = kernel->getCLKernel();
				if (cl_kernel_id == NULL) {
						printf("Error: cl_kernel_id null\n");
						//TODO (siba samal) Handle enqueueTask  API
						printf("WebCLCommandQueue::enqueueTask event_wait_list=%d\n",
										event_wait_list);
						ec = WebCLException::INVALID_KERNEL;
						return NULL;
				}
		}

		err = clEnqueueTask(m_cl_command_queue, cl_kernel_id, 0, NULL,&cl_event_id); 

		if (err != CL_SUCCESS) {
				printf("Error: clEnqueueTask\n");
				switch (err) {
						case CL_INVALID_COMMAND_QUEUE:
								printf("Error: CL_INVALID_COMMAND_QUEUE\n");
								ec = WebCLException::INVALID_COMMAND_QUEUE;
								break;
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec = WebCLException::INVALID_CONTEXT;
								break;
						case CL_INVALID_PROGRAM_EXECUTABLE :
								printf("Error: CL_INVALID_PROGRAM_EXECUTABLE \n");
								ec = WebCLException::INVALID_PROGRAM_EXECUTABLE;
								break;				
						case CL_INVALID_KERNEL :
								printf("Error: CL_INVALID_KERNEL \n");
								ec = WebCLException::INVALID_KERNEL;
								break;
						case CL_INVALID_EVENT_WAIT_LIST:
								printf("Error: CL_INVALID_EVENT_WAIT_LIST\n");
								ec = WebCLException::INVALID_EVENT_WAIT_LIST;
								break;
						case CL_INVALID_KERNEL_ARGS :
								printf("Error: CL_INVALID_KERNEL_ARGS \n");
								ec = WebCLException::INVALID_KERNEL_ARGS;
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec = WebCLException::OUT_OF_HOST_MEMORY;
								break;
						case CL_INVALID_WORK_GROUP_SIZE :
								printf("Error: CL_INVALID_WORK_GROUP_SIZE \n");
								ec = WebCLException::FAILURE;
								break;		
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES  \n");
								ec = WebCLException::OUT_OF_RESOURCES;
								break;			
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE  \n");
								ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
								break;	
						default:
								printf("Error: Invaild Error Type\n");
								ec = WebCLException::FAILURE;
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
