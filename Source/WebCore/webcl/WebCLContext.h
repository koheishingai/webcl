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

#ifndef WebCLContext_h
#define WebCLContext_h

#include "ScriptObject.h"
#include "ScriptState.h"

#include "ActiveDOMObject.h"
#include <wtf/Float32Array.h>
#include <wtf/Int32Array.h>
#include "ImageData.h"
#include "HTMLCanvasElement.h"
#include "HTMLImageElement.h"
#include "HTMLVideoElement.h"
#include "GraphicsContext3D.h"
#include "WebGLBuffer.h"
#include "WebGLRenderingContext.h"
#include "WebCLGetInfo.h"
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


#include <wtf/OwnPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/Vector.h>
#include <PlatformString.h>
#include <OpenCL/opencl.h>
#include <wtf/ArrayBuffer.h>

namespace WebCore {

class WebCLComputeContext;
class WebCLCommandQueue;
class WebCLProgram;
class WebCLMem;
class WebCLSampler;
class WebCLEvent;
class ImageData;
class ImageBuffer;
class IntSize;

class WebCLContext : public RefCounted<WebCLContext> {
public:
	virtual ~WebCLContext();
	static PassRefPtr<WebCLContext> create(WebCLComputeContext*, cl_context);
	
	WebCLGetInfo getContextInfo(int, ExceptionCode&);
	PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDeviceIDList*, int, ExceptionCode&);
	PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDeviceID*, int, ExceptionCode&);
	PassRefPtr<WebCLProgram> createProgramWithSource(const String&, ExceptionCode&);
	PassRefPtr<WebCLProgram> createProgramWithBinary(WebCLDeviceIDList* ,const String&, ExceptionCode&);
	PassRefPtr<WebCLMem> createBuffer(int, int, int, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLCanvasElement*, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLImageElement*, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLVideoElement*, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage2D(int, ImageData*, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage2D(int,unsigned int,unsigned int, ArrayBuffer*, ExceptionCode&);
	PassRefPtr<WebCLMem> createImage3D(int, unsigned int, unsigned int, unsigned int,ArrayBuffer*, ExceptionCode&);
	PassRefPtr<WebCLSampler> createSampler(bool, int, int, ExceptionCode&);
	PassRefPtr<WebCLMem> createFromGLBuffer(int, WebGLBuffer*, ExceptionCode&);
	PassRefPtr<WebCLMem> createFromGLTexture2D(int, GC3Denum,GC3Dint, GC3Duint, ExceptionCode&);
	PassRefPtr<WebCLEvent> createUserEvent(ExceptionCode&);	
	void releaseCLResource( ExceptionCode&);
	void retainCLResource( ExceptionCode&);
	void setDeviceID(RefPtr<WebCLDeviceID>);
	cl_context getCLContext();
	// Fixed-size cache of reusable image buffers for video texImage2D calls.
	class LRUImageBufferCache {
	public:
		LRUImageBufferCache(int capacity);
		// The pointer returned is owned by the image buffer map.
		ImageBuffer* imageBuffer(const IntSize& size);
	private:
		void bubbleToFront(int idx);
		OwnArrayPtr<OwnPtr<ImageBuffer> > m_buffers;
		int m_capacity;
	};
	LRUImageBufferCache m_videoCache;

	private:
	WebCLContext(WebCLComputeContext*, cl_context);
	WebCLComputeContext* m_context;
	cl_context m_cl_context;
	RefPtr<WebCLDeviceID> m_device_id;
	
	Vector<RefPtr<WebCLProgram> > m_program_list;
	long m_num_programs;
	Vector<RefPtr<WebCLMem> > m_mem_list;
	long m_num_mems;
	Vector<RefPtr<WebCLEvent> > m_event_list;
	long m_num_events;
	Vector<RefPtr<WebCLSampler> > m_sampler_list;
	long m_num_samplers;
	Vector<RefPtr<WebCLContext> > m_context_list;
	long m_num_contexts;
	RefPtr<WebCLCommandQueue> m_command_queue;
	
	PassRefPtr<Image> videoFrameToImage(HTMLVideoElement*);

};



} // namespace WebCore

#endif // WebCLContext_h
