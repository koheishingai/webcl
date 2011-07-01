/*
 *  WebCLKernel.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/28/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLKernel.h"

namespace WebCore {

WebCLKernel::~WebCLKernel()
{
}

PassRefPtr<WebCLKernel> WebCLKernel::create(WebCLComputeContext* 
											compute_context, cl_kernel kernel)
{
	printf("WebCLKernel::create\n");
	return adoptRef(new WebCLKernel(compute_context, kernel))
	;
}

WebCLKernel::WebCLKernel(WebCLComputeContext* compute_context, cl_kernel kernel) 
							: m_context(compute_context), m_cl_kernel(kernel)
{
	printf("WebCLKernel::WebCLKernel\n");
}

cl_kernel WebCLKernel::getCLKernel()
{
	return m_cl_kernel;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
