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

#include "JSDOMGlobalObject.h"
#include "DOMWindow.h"
#include "JSDOMWindow.h"
#include <runtime/JSFunction.h>
#include "JSWebCLComputeContext.h"
#include "JSDOMBinding.h"
#include "WebCLComputeContext.h"
#include "JSImageData.h"
#include "JSOESStandardDerivatives.h"
#include "JSOESTextureFloat.h"
#include "JSFloat32Array.h"
#include "JSInt32Array.h"
#include "JSWebCLContext.h"
#include "JSUint8Array.h"
#include "JSWebKitCSSMatrix.h"
#include "NotImplemented.h"
#include "OESStandardDerivatives.h"
#include "OESTextureFloat.h"
#include <wtf/Float32Array.h>
#include "WebCLGetInfo.h"
#include <wtf/Int32Array.h>
#include <runtime/Error.h>
#include <runtime/JSArray.h>
#include <wtf/FastMalloc.h>
#include "JSWebCLPlatformID.h"
#include "JSWebCLDeviceID.h"
#include "JSWebCLKernel.h"
#include "JSWebCLProgram.h"
#include "JSWebCLCommandQueue.h"
#include "JSWebCLContext.h"
#include "JSWebCLMem.h"
#include "JSWebCLEvent.h"
#include "JSWebCLSampler.h"
#include "JSWebCLImage.h"
#include "WebCLPlatformID.h"
#include "WebCLKernel.h"  //not needeed
#include "WebCLProgram.h"
#include "WebCLDeviceID.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLMem.h"
#include "WebCLEvent.h"
#include "WebCLSampler.h"
#include "WebCLImage.h"
#include "WebCLBuffer.h"
#include "WebCLKernelTypes.h" //not needeed
#include "ScriptValue.h"  //not needeed
#include "JSWebCLComputeContextCustom.h"
#include <stdio.h>

using namespace JSC;
using namespace std;

namespace WebCore { 

//static JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, int user_data)
//{
//	return getDOMObjectWrapper<JSWebCLComputeContext>(exec, globalObject, object);
//}


JSValue JSWebCLComputeContext::getImageInfo(JSC::ExecState* exec)
{
	if (exec->argumentCount() != 2)
		return throwSyntaxError(exec);

	ExceptionCode ec = 0;
	WebCLComputeContext* context = static_cast<WebCLComputeContext*>(impl());
	WebCLImage* wimage  = toWebCLImage(exec->argument(0));
	if (exec->hadException())
		return jsUndefined();
	unsigned image_info  = exec->argument(1).toInt32(exec);
	if (exec->hadException())
		return jsUndefined();
	WebCLGetInfo info = context->getImageInfo(wimage,image_info,ec);
	if (ec) {
		setDOMException(exec, ec);
		return jsUndefined();
	}
	return toJS(exec, globalObject(), info);
}
EncodedJSValue JSC_HOST_CALL JSWebCLComputeContextConstructor::constructJSWebCLComputeContext(ExecState* exec)
{
	JSWebCLComputeContextConstructor* jsConstructor = static_cast<JSWebCLComputeContextConstructor*>(exec->callee());
	ScriptExecutionContext* context = jsConstructor->scriptExecutionContext();
	RefPtr<WebCLComputeContext> webCLComputeContext = WebCLComputeContext::create(context);

	//DOMWindow* window = asJSDOMWindow(exec->lexicalGlobalObject())->impl();
	//RefPtr<WebCLComputeContext> webCLComputeContext = WebCLComputeContext::create(window->document());
	//return JSValue::encode(asObject(toJS(exec, jsConstructor->globalObject())));
	return JSValue::encode(CREATE_DOM_WRAPPER(exec, jsConstructor->globalObject(), WebCLComputeContext, webCLComputeContext.get()));
}
} // namespace WebCore

#endif // ENABLE(WEBCL)
