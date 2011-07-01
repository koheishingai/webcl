/*
 *  WebCLSampler.cpp
 *  WebCore
 *
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLSampler.h"

namespace WebCore {

WebCLSampler::~WebCLSampler()
{
}

PassRefPtr<WebCLSampler> WebCLSampler::create(WebCLComputeContext* compute_context, 
	cl_sampler sampler, bool is_shared = false)
{
	printf("WebCLSampler::create\n");
	return adoptRef(new WebCLSampler(compute_context, sampler, is_shared));
}

WebCLSampler::WebCLSampler(WebCLComputeContext* compute_context, cl_sampler sampler, bool is_shared) 
		: m_context(compute_context), m_cl_sampler(sampler), m_shared(is_shared)
{
	printf("WebCLMem::WebCLSampler\n");
}

cl_sampler WebCLSampler::getCLSampler()
{
	return m_cl_sampler;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
