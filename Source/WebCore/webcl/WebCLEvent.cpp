/*
 *  WebCLEvent.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/25/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLEvent.h"

namespace WebCore {

WebCLEvent::~WebCLEvent()
{
}

PassRefPtr<WebCLEvent> WebCLEvent::create(WebCLComputeContext*
                                                      compute_context, cl_event Event)
{
        printf("WebCLEvent::create\n");
        return adoptRef(new WebCLEvent(compute_context, Event));
}

WebCLEvent::WebCLEvent(WebCLComputeContext* compute_context,
                cl_event Event) : m_context(compute_context), m_cl_Event(Event)
{
        printf("WebCLEvent::WebCLEvent\n");
}

cl_event WebCLEvent::getCLEvent()
{
        return m_cl_Event;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

