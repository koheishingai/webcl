/*
 *  WebCLSampler.h
 *  WebCore
 *
 */

#ifndef WebCLSampler_h
#define WebCLSampler_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLSampler : public RefCounted<WebCLSampler> {
public:
	virtual ~WebCLSampler();
	static PassRefPtr<WebCLSampler> create(WebCLComputeContext*, cl_sampler, bool);
	cl_sampler getCLSampler();
	bool isShared() { return m_shared; }

private:
	WebCLSampler(WebCLComputeContext*, cl_sampler, bool);
	
	WebCLComputeContext* m_context;
	cl_sampler m_cl_sampler;
	bool m_shared;
};

} // namespace WebCore

#endif // WebCLSampler_h
