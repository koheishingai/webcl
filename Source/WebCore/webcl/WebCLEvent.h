/*
 *  WebCLEvent.h
 *  WebCore
 *
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLEvent_h
#define WebCLEvent_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLComputeContext;

class WebCLEvent : public RefCounted<WebCLEvent> {
public:
        virtual ~WebCLEvent();
        static PassRefPtr<WebCLEvent> create(WebCLComputeContext*, cl_event);
        cl_event getCLEvent();

private:
        WebCLEvent(WebCLComputeContext*, cl_event);

        WebCLComputeContext* m_context;
        cl_event m_cl_Event;
};

} // namespace WebCore

#endif // WebCLEvent_h

