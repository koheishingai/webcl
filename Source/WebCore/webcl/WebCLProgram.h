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

#ifndef WebCLProgram_h
#define WebCLProgram_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <PlatformString.h>
#include "ExceptionCode.h"

#include "WebCLDeviceID.h"
#include "WebCLDeviceIDList.h"

namespace WebCore {

class WebCLComputeContext;
class WebCLGetInfo;
class WebCLKernel;
class WebCLKernelList;

class WebCLProgram : public RefCounted<WebCLProgram> {
public:
	virtual ~WebCLProgram();
	static PassRefPtr<WebCLProgram> create(WebCLComputeContext*, cl_program);
	WebCLGetInfo getProgramInfo(int, ExceptionCode&);
	WebCLGetInfo getProgramBuildInfo(WebCLDeviceID*, int, ExceptionCode&);
	PassRefPtr<WebCLKernel> createKernel(const String&, ExceptionCode&);
	void buildProgram(int, int, int, ExceptionCode&);
	void buildProgram(WebCLDeviceID*,int, int, int, ExceptionCode&);
	void buildProgram(WebCLDeviceIDList*,int, int, int, ExceptionCode&);
	void releaseCLResource( ExceptionCode&);
	void retainCLResource( ExceptionCode&);
	void setDeviceID(RefPtr<WebCLDeviceID>);
	PassRefPtr<WebCLKernelList> createKernelsInProgram( ExceptionCode&); 
	cl_program getCLProgram();

private:
	WebCLProgram(WebCLComputeContext*, cl_program);
	WebCLComputeContext* m_context;
	cl_program m_cl_program;
	long m_num_programs;
	long m_num_kernels;
	Vector<RefPtr<WebCLProgram> > m_program_list;
	Vector<RefPtr<WebCLKernel> > m_kernel_list;
	RefPtr<WebCLDeviceID> m_device_id;
	
};

} // namespace WebCore

#endif // WebCLProgram_h
