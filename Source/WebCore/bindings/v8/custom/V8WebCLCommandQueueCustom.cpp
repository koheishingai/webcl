/*
 * Copyright (C) 2012 Intel Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE
 * INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "V8Binding.h"
#include "V8Proxy.h"
#include "V8WebCLCommandQueue.h"
#include "V8WebCLCustom.h"

namespace WebCore {

v8::Handle<v8::Value> V8WebCLCommandQueue::getInfoCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebCLCommandQueue.getInfo()");

    if (args.Length() != 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());

    ExceptionCode ec = 0;
    WebCLCommandQueue* queue = V8WebCLCommandQueue::toNative(args.Holder()); 
    int queue_index = toInt32(args[0]);
    WebCLGetInfo info = queue->getInfo(queue_index, ec);
    if (ec)
        return V8Proxy::setDOMException(ec, args.GetIsolate());

    return toV8Object(info, args.GetIsolate());
}

v8::Handle<v8::Value> V8WebCLCommandQueue::finishCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebCLCommandQueue.finish()");

    ExceptionCode ec = 0;
    WebCLCommandQueue* queue = V8WebCLCommandQueue::toNative(args.Holder());
    queue->finish(ec);
    if (ec)
        return V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Undefined();
}

} // namespace WebCore

#endif // ENABLE(WEBCL)