/*
 *  WebCLBuffer.h
 *  WebCore
 *
 *
 */

#ifndef WebCLBuffer_h
#define WebCLBuffer_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLBuffer : public RefCounted<WebCLBuffer> {
public:
	virtual ~WebCLBuffer();
	static PassRefPtr<WebCLBuffer> create(WebCLComputeContext*, cl_mem, bool);
	cl_mem getCLBuffer();
	bool isShared() { return m_shared; }

private:
	WebCLBuffer(WebCLComputeContext*, cl_mem, bool);
	
	WebCLComputeContext* m_context;
	cl_mem m_cl_mem;
	bool m_shared;
};

} // namespace WebCore

#endif // WebCLBuffer_h
