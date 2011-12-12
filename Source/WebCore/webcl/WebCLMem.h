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

#ifndef WebCLMem_h
#define WebCLMem_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

#include "WebCLGetInfo.h"

namespace WebCore {

class WebCLComputeContext;

class WebCLMem : public RefCounted<WebCLMem> {
public:
	virtual ~WebCLMem();
	static PassRefPtr<WebCLMem> create(WebCLComputeContext*, cl_mem, bool);
	WebCLGetInfo getMemObjectInfo(int, ExceptionCode&);
	void retainCLResource( ExceptionCode&);
	void releaseCLResource( ExceptionCode&);
	
	cl_mem getCLMem();
	bool isShared() { return m_shared; }
	

private:
	WebCLMem(WebCLComputeContext*, cl_mem, bool);
	WebCLComputeContext* m_context;
	cl_mem m_cl_mem;
	bool m_shared;
	long m_num_mems;
	Vector<RefPtr<WebCLMem> > m_mem_list;
};

} // namespace WebCore

#endif // WebCLMem_h
