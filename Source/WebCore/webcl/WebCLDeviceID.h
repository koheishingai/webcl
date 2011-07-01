/*
 *  WebCLDeviceID.h
 *  WebCore
 *
 *  Created by Won Jeon on 3/11/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLDeviceID_h
#define WebCLDeviceID_h

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <OpenCL/opencl.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLDeviceID : public RefCounted<WebCLDeviceID> {
public:
	virtual ~WebCLDeviceID();
	static PassRefPtr<WebCLDeviceID> create(WebCLComputeContext* context, cl_device_id device_id);
	cl_device_id getCLDeviceID();
	
private:
	WebCLDeviceID(WebCLComputeContext* context, cl_device_id device_id);
	
	WebCLComputeContext* m_context;
	cl_device_id m_cl_device_id;
};

} // namespace WebCore

#endif // WebCLDeviceID_h
