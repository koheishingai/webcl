/*
 *  WebCLBuffer.cpp
 *  WebCore
 *
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLBuffer.h"

namespace WebCore {

WebCLBuffer::~WebCLBuffer()
{
}

PassRefPtr<WebCLBuffer> WebCLBuffer::create(WebCLComputeContext* compute_context, 
	cl_mem buffer, bool is_shared = false)
{
	printf("WebCLBuffer::create\n");
	return adoptRef(new WebCLBuffer(compute_context, buffer, is_shared));
}

WebCLBuffer::WebCLBuffer(WebCLComputeContext* compute_context, cl_mem buffer, bool is_shared) 
		: m_context(compute_context), m_cl_mem(buffer), m_shared(is_shared)
{
	printf("WebCLBuffer::WebCLBuffer\n");
}

cl_mem WebCLBuffer::getCLBuffer()
{
	return m_cl_mem;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
