/*
 *  WebCLProgram.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLProgram.h"

namespace WebCore {

WebCLProgram::~WebCLProgram()
{
}

PassRefPtr<WebCLProgram> WebCLProgram::create(WebCLComputeContext* 
											compute_context, cl_program program)
{
	printf("WebCLProgram::create\n");
	return adoptRef(new WebCLProgram(compute_context, program));
}

WebCLProgram::WebCLProgram(WebCLComputeContext* compute_context, 
		cl_program program) : m_context(compute_context), m_cl_program(program)
{
	printf("WebCLProgram::WebCLProgram\n");
}

cl_program WebCLProgram::getCLProgram()
{
	return m_cl_program;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
