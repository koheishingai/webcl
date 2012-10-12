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

#include "V8Proxy.h"
#include "V8WebCLCustom.h"
#include "V8WebCLDevice.h"
#include <wtf/Vector.h>

namespace WebCore {

v8::Handle<v8::Value> V8WebCLDevice::getInfoCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebCLDevice.getInfo()");

    if (args.Length() != 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());

    ExceptionCode ec = 0;
    WebCLDevice* device = V8WebCLDevice::toNative(args.Holder()); 
    int device_index = toInt32(args[0]);
    WebCLGetInfo info = device->getInfo(device_index, ec);
    if (ec)
        return V8Proxy::setDOMException(ec, args.GetIsolate());

    return toV8Object(info, args.GetIsolate());
}

v8::Handle<v8::Value> V8WebCLDevice::getSupportedExtensionsCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.WebCLDevice.getSupportedExtensions()");

    ExceptionCode ec = 0;
    WebCLDevice* device = V8WebCLDevice::toNative(args.Holder());
    Vector<String> extensions = device->getSupportedExtensions(ec);
    if (ec)
        return V8Proxy::setDOMException(ec, args.GetIsolate());

    v8::Local<v8::Array> array = v8::Array::New(extensions.size());
    for (size_t i = 0; i < extensions.size(); i++)
        array->Set(v8::Integer::New(i), v8::String::New(fromWebCoreString(extensions[i]), extensions[i].length()));
    return array;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)