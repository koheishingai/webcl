/*
 *  WebCLCommandQueue.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLCommandQueue.h"

namespace WebCore {

WebCLCommandQueue::~WebCLCommandQueue()
{
}

PassRefPtr<WebCLCommandQueue> WebCLCommandQueue::create(WebCLComputeContext* compute_context, cl_command_queue command_queue)
{
	printf("WebCLCommandQueue::create\n");
	return adoptRef(new WebCLCommandQueue(compute_context, command_queue));
}

WebCLCommandQueue::WebCLCommandQueue(WebCLComputeContext* compute_context, cl_command_queue command_queue) : m_context(compute_context), m_cl_command_queue(command_queue)
{
	printf("WebCLCommandQueue::WebCLCommandQueue\n");
}

cl_command_queue WebCLCommandQueue::getCLCommandQueue()
{
	return m_cl_command_queue;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
