/*
 *  WebCLMem.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/28/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLMem_h
#define WebCLMem_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLMem : public RefCounted<WebCLMem> {
public:
	virtual ~WebCLMem();
	static PassRefPtr<WebCLMem> create(WebCLComputeContext*, cl_mem, bool);
	cl_mem getCLMem();
	bool isShared() { return m_shared; }

private:
	WebCLMem(WebCLComputeContext*, cl_mem, bool);
	
	WebCLComputeContext* m_context;
	cl_mem m_cl_mem;
	bool m_shared;
};

} // namespace WebCore

#endif // WebCLMem_h
