/*
 *  WebCLPlatformID.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/24/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLPlatformID_h
#define WebCLPlatformID_h

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <OpenCL/opencl.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLPlatformID : public RefCounted<WebCLPlatformID> {
public:
	virtual ~WebCLPlatformID();
	static PassRefPtr<WebCLPlatformID> create(WebCLComputeContext* context, cl_platform_id platform_id);
	cl_platform_id getCLPlatformID();
	
private:
	WebCLPlatformID(WebCLComputeContext* context, cl_platform_id platform_id);
	
	WebCLComputeContext* m_context;
	cl_platform_id m_cl_platform_id;
};

} // namespace WebCore

#endif // WebCLPlatformID_h
