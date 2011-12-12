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

#ifndef WebCLCommandQueue_h
#define WebCLCommandQueue_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

#include "WebCLGetInfo.h"
#include "WebCLFinishCallback.h"
#include "WebCLProgram.h"
#include "WebCLKernel.h"
#include "WebCLMem.h"
#include "WebCLEvent.h"
#include "WebCLSampler.h"
#include "ImageData.h"
#include "HTMLCanvasElement.h"

namespace WebCore {

class WebCLComputeContext;
class WebCLEventList;

class WebCLCommandQueue : public RefCounted<WebCLCommandQueue> {
public:
	virtual ~WebCLCommandQueue();
	static PassRefPtr<WebCLCommandQueue> create(WebCLComputeContext*, cl_command_queue);
	WebCLGetInfo getCommandQueueInfo(int, ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueWriteBuffer(WebCLMem*, bool, int, int, 
		Float32Array*, int, ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueWriteBuffer(WebCLMem*, bool, int, int, 
		Int32Array*, int, ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueWriteBuffer(WebCLMem*, bool, int, int, 
		Uint8Array*, int, ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueWriteBuffer(WebCLMem*, bool, int, int,
        ImageData* , int , ExceptionCode&);
	PassRefPtr<WebCLEvent>  enqueueReadBuffer(WebCLMem*, bool, int, int, 
		Float32Array*, int, ExceptionCode&);
	//PassRefPtr<WebCLEvent>  enqueueReadBuffer(WebCLMem*, bool, int, int,
	//		HTMLCanvasElement*, int, ExceptionCode&);
	PassRefPtr<WebCLEvent>  enqueueReadBuffer(WebCLMem*, bool, int, int, 
		Int32Array*, int, ExceptionCode&);
	PassRefPtr<WebCLEvent>  enqueueReadBuffer(WebCLMem*, bool, int, int,
        ImageData* , int , ExceptionCode&);
	PassRefPtr<WebCLEvent>  enqueueReadBuffer(WebCLMem*, bool, int, int, 
		Uint8Array*, int, ExceptionCode&);	
	//PassRefPtr<WebCLEvent>  enqueueNDRangeKernel(WebCLKernel*, unsigned int, 
	//		unsigned int, unsigned int, unsigned int, int, ExceptionCode&);
	PassRefPtr<WebCLEvent>  enqueueNDRangeKernel(WebCLKernel*, unsigned int,
		unsigned int, Int32Array*, Int32Array*, int, ExceptionCode&);
	void finish(PassRefPtr<WebCLFinishCallback>, int /*object userData*/, ExceptionCode&);
	void flush( ExceptionCode&);
	void retainCLResource( ExceptionCode&);
	void releaseCLResource( ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueWriteImage(WebCLMem*, bool, Int32Array*, 
		Int32Array*, HTMLCanvasElement*, int, ExceptionCode&);
	//long enqueueReadImage(WebCLMem*, bool, Int32Array*, 
	//		Int32Array*, HTMLCanvasElement*, int, ExceptionCode&);
	void enqueueAcquireGLObjects(WebCLMem*, int, ExceptionCode&);
	void enqueueReleaseGLObjects(WebCLMem*, int, ExceptionCode&);
	void enqueueCopyBuffer(WebCLMem*, WebCLMem*, int, ExceptionCode&);
	void enqueueBarrier( ExceptionCode&);
	void enqueueMarker(WebCLEvent*, ExceptionCode&);
	void enqueueWaitForEvents(WebCLEventList*, ExceptionCode&);
	PassRefPtr<WebCLEvent> enqueueTask( WebCLKernel* ,int, ExceptionCode&);
	cl_command_queue getCLCommandQueue();	
private:
	WebCLCommandQueue(WebCLComputeContext*, cl_command_queue);	
	WebCLComputeContext* m_context;
	cl_command_queue m_cl_command_queue;
	RefPtr<WebCLFinishCallback> m_finishCallback;
	RefPtr<WebCLCommandQueue> m_command_queue;
	
	
	long m_num_events;
	long m_num_commandqueues;
	long m_num_mems;
	Vector<RefPtr<WebCLEvent> > m_event_list;
	Vector<RefPtr<WebCLCommandQueue> > m_commandqueue_list;
	Vector<RefPtr<WebCLMem> > m_mem_list;
};

} // namespace WebCore
#endif // WebCLCommandQueue_h
