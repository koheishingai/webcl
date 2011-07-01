/*
 *  WebCLDeviceIDList.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/23/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
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
	printf("WebCLDeviceIDList::create\n");
	return adoptRef(new WebCLDeviceIDList(compute_context, platform_id, device_type));
}

WebCLDeviceIDList::WebCLDeviceIDList(WebCLComputeContext* compute_context,
		cl_platform_id platform_id, int device_type) : m_context(compute_context)
{
	printf("WebCLDeviceIDList::WebCLDeviceIDList\n");
	
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
		printf("what else?\n");
		break;
	}
	printf("num_devices=%d\n", num_devices);
	m_num_devices = num_devices;
	
	if (err != CL_SUCCESS)
	{
		printf("Error: getDeviceIDs\n");
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
		printf("what else?\n");
		break;
	}
	for (unsigned int i = 0; i < m_num_devices; i++) {
		RefPtr<WebCLDeviceID> o = WebCLDeviceID::create(m_context, m_cl_devices[i]);
		if (o != NULL) {
			m_device_id_list.append(o);
		} else {
			printf("Error: o null\n");
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
	printf("WebCLDeviceIDList::item index %d\n", index);
	
	if (index >= m_num_devices) {
		return 0;
	}
	WebCLDeviceID* ret = (m_device_id_list[index]).get();
	return ret;

}

} // namespace WebCore

#endif // ENABLE(WEBCL)
