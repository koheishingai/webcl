/*
 *  WebCLFinishCallback.h
 *  WebCore
 *
 *  Created by Siba Samal 6/16/11
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */
#ifndef WebCLFinishCallback_h
#define WebCLFinishCallback_h

//#include <wtf/Platform.h>
#include <wtf/RefCounted.h>
#include "ActiveDOMObject.h"

namespace WebCore {
class WebCLComputeContext;    
    class WebCLFinishCallback : public RefCounted<WebCLFinishCallback>, public ActiveDOMObject {
    public:
		WebCLFinishCallback(ScriptExecutionContext* context) : ActiveDOMObject(context, this) { }
        virtual ~WebCLFinishCallback() { }
        virtual void handleEvent(WebCLComputeContext*) = 0;
    };
    
} // namespace WebCore
#endif // WebCLFinishCallback_H
