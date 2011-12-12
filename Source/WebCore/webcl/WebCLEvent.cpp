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

#include "WebCLEvent.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLEvent::~WebCLEvent()
{
}

PassRefPtr<WebCLEvent> WebCLEvent::create(WebCLComputeContext*
                                                      compute_context, cl_event Event)
{
        return adoptRef(new WebCLEvent(compute_context, Event));
}

WebCLEvent::WebCLEvent(WebCLComputeContext* compute_context,
                cl_event Event) : m_context(compute_context), m_cl_Event(Event)
{
	m_num_events = 0;
}


WebCLGetInfo WebCLEvent::getEventInfo(int param_name, ExceptionCode& ec)
{   
	cl_int err = 0;
	cl_uint uint_units = 0;
	cl_command_type command_type = 0;
	cl_command_queue command_queue = 0;
	RefPtr<WebCLCommandQueue> cqObj = NULL;
	if (m_cl_Event == NULL) {
		printf("Error: Invalid CL Event\n");
		ec = WebCLComputeContext::INVALID_EVENT;
		return WebCLGetInfo();
	}

	switch(param_name)
	{   
		case WebCLComputeContext::EVENT_COMMAND_EXECUTION_STATUS :
			err=clGetEventInfo(m_cl_Event, CL_EVENT_COMMAND_EXECUTION_STATUS  , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCLComputeContext::EVENT_REFERENCE_COUNT:
			err=clGetEventInfo(m_cl_Event, CL_EVENT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case WebCLComputeContext::EVENT_COMMAND_TYPE:			
			err=clGetEventInfo(m_cl_Event, CL_EVENT_COMMAND_TYPE , sizeof(cl_command_type), &command_type, NULL);;
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(command_type));
			break;
		case WebCLComputeContext::EVENT_COMMAND_QUEUE:			
			err = clGetEventInfo(m_cl_Event, CL_EVENT_COMMAND_QUEUE , sizeof(cl_command_queue), &command_queue, NULL);;
			cqObj = WebCLCommandQueue::create(m_context, command_queue);
			if(cqObj == NULL)
			{
				printf("SUCCESS: Cl Event Command Queue\n");
				return WebCLGetInfo();
			}
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLCommandQueue>(cqObj));
			break;
		default:
			printf("Error: Unsupported Event Info type\n");
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_VALUE:
			ec = WebCLComputeContext::INVALID_VALUE;
			printf("Error: CL_INVALID_VALUE   \n");
			break;
		case CL_INVALID_EVENT:
			ec = WebCLComputeContext::INVALID_EVENT;
			printf("Error: CL_INVALID_EVENT   \n");
			break; 
		case CL_OUT_OF_RESOURCES:
			ec = WebCLComputeContext::OUT_OF_RESOURCES;
			printf("Error: CL_OUT_OF_RESOURCES   \n");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
			printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
			break;
		default:
			printf("Error: Invaild Error Type\n");
			ec = WebCLComputeContext::FAILURE;
			break;
	}				
	return WebCLGetInfo();

}

WebCLGetInfo WebCLEvent::getEventProfilingInfo(int param_name, ExceptionCode& ec)
{
	cl_int err=0;
	cl_ulong  ulong_units = 0;

	if (m_cl_Event == NULL) {
		printf("Error: Invalid CL Event\n");
		ec = WebCLComputeContext::INVALID_EVENT;
		return WebCLGetInfo();
	}
	switch(param_name)
	{   
		case WebCLComputeContext::PROFILING_COMMAND_QUEUED:
			err=clGetEventProfilingInfo(m_cl_Event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
			break;
		case WebCLComputeContext::PROFILING_COMMAND_SUBMIT:
			err=clGetEventProfilingInfo(m_cl_Event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
			break;
		case WebCLComputeContext::PROFILING_COMMAND_START:
			err=clGetEventProfilingInfo(m_cl_Event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
			break;
		case WebCLComputeContext::PROFILING_COMMAND_END:
			err=clGetEventProfilingInfo(m_cl_Event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		default:
			printf("Error: Unsupported Profiling Info type\n");
			return WebCLGetInfo();

	}

	switch (err) {

		case CL_PROFILING_INFO_NOT_AVAILABLE:
			ec = WebCLComputeContext::PROFILING_INFO_NOT_AVAILABLE;
			printf("Error: CL_PROFILING_INFO_NOT_AVAILABLE\n");
			break;
		case CL_INVALID_VALUE:
			ec = WebCLComputeContext::INVALID_VALUE;
			printf("Error: CL_INVALID_VALUE\n");
			break;
		case CL_INVALID_EVENT:
			ec = WebCLComputeContext::INVALID_EVENT;
			printf("Error: CL_INVALID_EVENT \n");
			break; 
		case CL_OUT_OF_RESOURCES:
			ec = WebCLComputeContext::OUT_OF_RESOURCES;
			printf("Error: CL_OUT_OF_RESOURCES\n");
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
	return WebCLGetInfo();

}
void WebCLEvent::setUserEventStatus (int exec_status, ExceptionCode& ec)
{
	cl_int err = -1;
	if (m_cl_Event == NULL) {
		printf("Error: Invalid CL Event\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	// TODO (siba samal) To be uncommented for  OpenCL 1.1	
	// http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clSetUserEventStatus.html
	if(exec_status == WebCLComputeContext::COMPLETE)	{   
		//err = clSetUserEventStatus(m_cl_Event , CL_COMPLETE)
	}
	else if (exec_status < 0) {
		//err = clSetUserEventStatus(m_cl_Event , exec_status)
	}
	else
	{
		printf("Error: Invaild Error Type\n");
		return;
	}
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_EVENT :
				printf("Error: CL_INVALID_EVENT \n");
				ec = WebCLComputeContext::INVALID_EVENT;
				break;
			case CL_INVALID_VALUE :
				printf("Error: CL_INVALID_VALUE  \n");
				ec = WebCLComputeContext::INVALID_VALUE ;
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				ec = WebCLComputeContext::OUT_OF_RESOURCES ;
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION  \n");
				ec = WebCLComputeContext::INVALID_OPERATION ;
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

	} 
	return;
}

void WebCLEvent::retainCLResource( ExceptionCode& ec)
{
	cl_int err = 0;

	if (m_cl_Event == NULL) {
		printf("Error: Invalid CL Event\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}

	err = clRetainEvent(m_cl_Event);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_EVENT:
				printf("Error: CL_INVALID_EVENT\n");
				ec = WebCLComputeContext::INVALID_EVENT;
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
		printf("Success: clRetainEvent\n");
		// TODO - Check if has to be really added
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, m_cl_Event);
		m_event_list.append(o);
		m_num_events++;
		return;
	}
	return;
}

void WebCLEvent::releaseCLResource( ExceptionCode& ec)
{
	cl_int err = 0;
	if (m_cl_Event == NULL) {
		printf("Error: Invalid CL Event\n");
		ec = WebCLComputeContext::FAILURE;
		return;
	}
	err = clReleaseEvent(m_cl_Event);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_EVENT  :
				printf("Error: CL_INVALID_EVENT\n");
				ec = WebCLComputeContext::INVALID_EVENT;
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
		for (int i = 0; i < m_num_events; i++) {
			if ((m_event_list[i].get())->getCLEvent() == m_cl_Event) {
				m_event_list.remove(i);
				m_num_events = m_event_list.size();
				break;
			}
		}
		return;
	}
	return;
}



cl_event WebCLEvent::getCLEvent()
{
        return m_cl_Event;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

