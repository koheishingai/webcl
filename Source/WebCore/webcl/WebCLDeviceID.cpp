/*
 *  WebCLDeviceID.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 3/11/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"
#include "WebCLDeviceID.h"

namespace WebCore {

WebCLDeviceID::~WebCLDeviceID()
{
}

PassRefPtr<WebCLDeviceID> WebCLDeviceID::create(WebCLComputeContext* context, cl_device_id device_id)
{
	printf("WebCLDeviceID::create\n");
	return adoptRef(new WebCLDeviceID(context, device_id));
}

WebCLDeviceID::WebCLDeviceID(WebCLComputeContext* context, cl_device_id device_id)
	: m_context(context), m_cl_device_id(device_id)
{
	printf("WebCLDeviceID::WebCLDeviceID\n");
}

cl_device_id WebCLDeviceID::getCLDeviceID()
{
	return m_cl_device_id;
}

} // namespace WebCore
