/*
 *  WebCLKernel.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/28/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLKernel_h
#define WebCLKernel_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLKernel : public RefCounted<WebCLKernel> {
public:
	virtual ~WebCLKernel();
	static PassRefPtr<WebCLKernel> create(WebCLComputeContext*, cl_kernel);
	cl_kernel getCLKernel();

private:
	WebCLKernel(WebCLComputeContext*, cl_kernel);
	
	WebCLComputeContext* m_context;
	cl_kernel m_cl_kernel;
};

} // namespace WebCore

#endif // WebCLKernel_h
