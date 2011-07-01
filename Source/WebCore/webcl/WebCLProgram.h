/*
 *  WebCLProgram.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLProgram_h
#define WebCLProgram_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLProgram : public RefCounted<WebCLProgram> {
public:
	virtual ~WebCLProgram();
	static PassRefPtr<WebCLProgram> create(WebCLComputeContext*, cl_program);
	cl_program getCLProgram();
	
private:
	WebCLProgram(WebCLComputeContext*, cl_program);
	
	WebCLComputeContext* m_context;
	cl_program m_cl_program;
};

} // namespace WebCore

#endif // WebCLProgram_h
