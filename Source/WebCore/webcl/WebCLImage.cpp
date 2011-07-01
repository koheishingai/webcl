/*
 *  WebCLImage.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/28/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLImage.h"

namespace WebCore {

WebCLImage::~WebCLImage()
{
}

PassRefPtr<WebCLImage> WebCLImage::create(WebCLComputeContext* compute_context, 
	cl_mem image, bool is_shared = false)
{
	printf("WebCLImage::create\n");
	return adoptRef(new WebCLImage(compute_context, image, is_shared));
}

WebCLImage::WebCLImage(WebCLComputeContext* compute_context, cl_mem image, bool is_shared) 
		: m_context(compute_context), m_cl_mem(image), m_shared(is_shared)
{
	printf("WebCLImage::WebCLImage\n");
}

cl_mem WebCLImage::getCLImage()
{
	return m_cl_mem;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
