/*
 *  WebCLPlatformID.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/24/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"
#include "WebCLPlatformID.h"

namespace WebCore {

WebCLPlatformID::~WebCLPlatformID()
{
}

PassRefPtr<WebCLPlatformID> WebCLPlatformID::create(WebCLComputeContext* context, cl_platform_id platform_id)
{
	printf("WebCLPlatformID::create\n");
	return adoptRef(new WebCLPlatformID(context, platform_id)); 
}

WebCLPlatformID::WebCLPlatformID(WebCLComputeContext* context, cl_platform_id platform_id)
 : m_context(context), m_cl_platform_id(platform_id)
{
	printf("WebCLPlatformID::WebCLPlatformID\n");
}

cl_platform_id WebCLPlatformID::getCLPlatformID()
{
	return m_cl_platform_id;
}

} // namespace WebCore
