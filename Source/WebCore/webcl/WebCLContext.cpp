/*
 *  WebCLContext.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLContext.h"

namespace WebCore {

WebCLContext::~WebCLContext()
{
}

PassRefPtr<WebCLContext> WebCLContext::create(WebCLComputeContext* compute_context, cl_context context_id)
{
	printf("WebCLContext::create\n");
	return adoptRef(new WebCLContext(compute_context, context_id));
}

WebCLContext::WebCLContext(WebCLComputeContext* compute_context, cl_context context_id) : m_context(compute_context), m_cl_context(context_id)
{
	printf("WebCLContext::WebCLContext\n");
}

cl_context WebCLContext::getCLContext()
{
	return m_cl_context;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
