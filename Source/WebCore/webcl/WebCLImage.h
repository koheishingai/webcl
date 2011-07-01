/*
 *  WebCLImage.h
 *  WebCore
 *
 *
 */

#ifndef WebCLImage_h
#define WebCLImage_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLImage : public RefCounted<WebCLImage> {
public:
	virtual ~WebCLImage();
	static PassRefPtr<WebCLImage> create(WebCLComputeContext*, cl_mem, bool);
	cl_mem getCLImage();
	bool isShared() { return m_shared; }

private:
	WebCLImage(WebCLComputeContext*, cl_mem, bool);
	
	WebCLComputeContext* m_context;
	cl_mem m_cl_mem;
	bool m_shared;
};

} // namespace WebCore

#endif // WebCLImage_h
