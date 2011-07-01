/*
 *  WebCLMem.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/28/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLMem.h"

namespace WebCore {

WebCLMem::~WebCLMem()
{
}

PassRefPtr<WebCLMem> WebCLMem::create(WebCLComputeContext* compute_context, 
	cl_mem mem, bool is_shared = false)
{
	printf("WebCLMem::create\n");
	return adoptRef(new WebCLMem(compute_context, mem, is_shared));
}

WebCLMem::WebCLMem(WebCLComputeContext* compute_context, cl_mem mem, bool is_shared) 
		: m_context(compute_context), m_cl_mem(mem), m_shared(is_shared)
{
	printf("WebCLMem::WebCLMem\n");
}

cl_mem WebCLMem::getCLMem()
{
	return m_cl_mem;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
