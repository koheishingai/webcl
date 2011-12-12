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

#include "WebCLPlatformID.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLPlatformID::~WebCLPlatformID()
{
}

PassRefPtr<WebCLPlatformID> WebCLPlatformID::create(WebCLComputeContext* context, cl_platform_id platform_id)
{
	return adoptRef(new WebCLPlatformID(context, platform_id)); 
}

WebCLPlatformID::WebCLPlatformID(WebCLComputeContext* context, cl_platform_id platform_id)
 : m_context(context), m_cl_platform_id(platform_id)
{
}

WebCLGetInfo WebCLPlatformID::getPlatformInfo (int platform_info, ExceptionCode& ec)
{
	cl_int err = 0;
	if (m_cl_platform_id == NULL) {
			ec = WebCLComputeContext::INVALID_PLATFORM;
			printf("Error: Invalid Platform ID\n");
			return WebCLGetInfo();
	}

	char platform_string[1024];
	switch(platform_info)
	{
		case WebCLComputeContext::PLATFORM_PROFILE:
			err = clGetPlatformInfo(m_cl_platform_id, CL_PLATFORM_PROFILE, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCLComputeContext::PLATFORM_VERSION:
			err = clGetPlatformInfo(m_cl_platform_id, CL_PLATFORM_VERSION, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCLComputeContext::PLATFORM_NAME:
			err = clGetPlatformInfo(m_cl_platform_id, CL_PLATFORM_NAME, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCLComputeContext::PLATFORM_VENDOR:
			err = clGetPlatformInfo(m_cl_platform_id, CL_PLATFORM_VENDOR, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCLComputeContext::PLATFORM_EXTENSIONS:
			err = clGetPlatformInfo(m_cl_platform_id, CL_PLATFORM_EXTENSIONS, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		default:
			printf("Error: Unsupported Platform Info type = %d ",platform_info);
			return WebCLGetInfo();
	}

	if(err != CL_SUCCESS)
	{
		switch (err) {
			case CL_INVALID_PLATFORM:
				ec = WebCLComputeContext::INVALID_PLATFORM;
				printf("Error: CL_INVALID_PLATFORM  \n");
				break;
			case CL_INVALID_VALUE:
				ec = WebCLComputeContext::INVALID_VALUE;
				printf("Error: CL_INVALID_VALUE\n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				ec = WebCLComputeContext::OUT_OF_HOST_MEMORY;
				printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
				break;
			default:
				ec = WebCLComputeContext::FAILURE;
				printf("Invaild Error Type\n");
				break;
		}
	}
	return WebCLGetInfo();
}

PassRefPtr<WebCLDeviceIDList> WebCLPlatformID::getDeviceIDs(int device_type, ExceptionCode& ec)
{
	if (m_cl_platform_id == NULL) {
		printf("Error: Invalid Platform ID\n");
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
	RefPtr<WebCLDeviceIDList> o = WebCLDeviceIDList::create(m_context, m_cl_platform_id, 
			device_type);
	if (o != NULL) {
		//TODO (siba samal) Check if its needed
		//m_device_id = o;
		return o;
	} else {
		ec = WebCLComputeContext::FAILURE;
		return NULL;
	}
}

cl_platform_id WebCLPlatformID::getCLPlatformID()
{
	return m_cl_platform_id;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
