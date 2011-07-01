/*
 *  WebCLContext.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLContext_h
#define WebCLContext_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLContext : public RefCounted<WebCLContext> {
public:
	virtual ~WebCLContext();
	static PassRefPtr<WebCLContext> create(WebCLComputeContext*, cl_context);
	cl_context getCLContext();
	
private:
	WebCLContext(WebCLComputeContext*, cl_context);
	
	WebCLComputeContext* m_context;
	cl_context m_cl_context;
};

} // namespace WebCore

#endif // WebCLContext_h
