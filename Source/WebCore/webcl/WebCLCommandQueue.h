/*
 *  WebCLCommandQueue.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLCommandQueue_h
#define WebCLCommandQueue_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLCommandQueue : public RefCounted<WebCLCommandQueue> {
public:
	virtual ~WebCLCommandQueue();
	static PassRefPtr<WebCLCommandQueue> create(WebCLComputeContext*, cl_command_queue);
	cl_command_queue getCLCommandQueue();
	
private:
	WebCLCommandQueue(WebCLComputeContext*, cl_command_queue);
	
	WebCLComputeContext* m_context;
	cl_command_queue m_cl_command_queue;
};

} // namespace WebCore
#endif // WebCLCommandQueue_h
