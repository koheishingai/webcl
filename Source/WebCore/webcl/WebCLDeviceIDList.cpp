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

#include "WebCLDeviceIDList.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLDeviceIDList::~WebCLDeviceIDList()
{
}

PassRefPtr<WebCLDeviceIDList> WebCLDeviceIDList::create(WebCLComputeContext* compute_context, 
		cl_platform_id platform_id, int device_type)
{
	return adoptRef(new WebCLDeviceIDList(compute_context, platform_id, device_type));
}

WebCLDeviceIDList::WebCLDeviceIDList(WebCLComputeContext* compute_context,
		cl_platform_id platform_id, int device_type) : m_context(compute_context)
{
	cl_int err = 0;
	cl_uint num_devices;
	
	switch(device_type) {
	case DEVICE_TYPE_GPU:
		err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, NULL, &num_devices);
		break;
	case DEVICE_TYPE_CPU:
		err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, NULL, &num_devices);
		break;
	default:
		// TODO (siba samal) error handling
		break;
	}
	m_num_devices = num_devices;
	
	if (err != CL_SUCCESS)
	{
		// TODO (siba samal) error handling for clGetDeviceIDs
		return;
	} 
	
	m_cl_devices = new cl_device_id[num_devices];
	switch(device_type) {
	case DEVICE_TYPE_GPU:
		err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, m_cl_devices, &num_devices);
		break;
	case DEVICE_TYPE_CPU:
		err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, m_cl_devices, &num_devices);
		break;
	default:
		// TODO (siba samal) error handling
		break;
	}
	for (unsigned int i = 0; i < m_num_devices; i++) {
		RefPtr<WebCLDeviceID> o = WebCLDeviceID::create(m_context, m_cl_devices[i]);
		if (o != NULL) {
			m_device_id_list.append(o);
		} else {
			// TODO (siba samal) error handling
		}
	}
}

cl_device_id WebCLDeviceIDList::getCLDeviceIDs()
{
	return *m_cl_devices;
}

unsigned WebCLDeviceIDList::length() const
{
	return m_num_devices;
}

WebCLDeviceID* WebCLDeviceIDList::item(unsigned index)
{
	if (index >= m_num_devices) {
		return 0;
	}
	WebCLDeviceID* ret = (m_device_id_list[index]).get();
	return ret;

}

} // namespace WebCore

#endif // ENABLE(WEBCL)
