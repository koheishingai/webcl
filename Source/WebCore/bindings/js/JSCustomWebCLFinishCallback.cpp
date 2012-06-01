/*
* Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided the following conditions
* are met:
* 
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
* 
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
* CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
* ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
* NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#if ENABLE(WEBCL)

#include "JSWebCLFinishCallback.h"
#include "Frame.h"
#include "JSWebCL.h"
#include "ScriptExecutionContext.h"
#include <runtime/JSLock.h>


namespace WebCore {

using namespace JSC;
//bool JSWebCLFinishCallback::handleEvent(WebCL* webCLComputeContext )
bool JSWebCLFinishCallback::handleEvent(int data)
{

	if (!m_data || !m_data->globalObject() ||  !(canInvokeCallback()))
        return true;

    RefPtr<JSWebCLFinishCallback> protect(this);
    JSC::JSLock lock(SilenceAssertionsOnly);
	
	ExecState* exec = m_data->globalObject()->globalExec();

    MarkedArgumentBuffer args;
//    args.append(toJS(exec, m_data->globalObject(), webCLComputeContext));
    args.append(jsNumber(data));
	printf("Insiide JSCustomWebCLFinishCallback::handleEvent44\n");
	bool raisedException = false;
	JSValue result = m_data->invokeCallback(args, &raisedException);
	printf("Inside JSCustomWebCLFinishCallback::handleEvent55\n");
    if (raisedException) {
        return true;
    }
	return result.toBoolean(exec);
}
} // namespace WebCore

#endif // ENABLE(WEBCL)
