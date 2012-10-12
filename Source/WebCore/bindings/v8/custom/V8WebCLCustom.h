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

#include "V8Float32Array.h"
#include "V8Int32Array.h"
#include "V8Binding.h"
#include "V8Proxy.h"
#include "V8WebCLCommandQueue.h"
#include "V8WebCLContext.h"
#include "V8WebCLDevice.h"
#include "V8WebCLDeviceList.h"
#include "V8WebCLProgram.h"

#include "WebCLCommandQueue.h"

namespace WebCore {

static v8::Handle<v8::Value> toV8Object(const WebCLGetInfo& info, v8::Isolate* isolate)
{
    switch (info.getType()) {
        case WebCLGetInfo::kTypeBool:
            return v8::Boolean::New(info.getBool());
        case WebCLGetInfo::kTypeBoolArray: {
            const Vector<bool>& value = info.getBoolArray();
            v8::Local<v8::Array> array = v8::Array::New(value.size());
            for (size_t ii = 0; ii < value.size(); ++ii)
                array->Set(v8::Integer::New(ii), v8::Boolean::New(value[ii]));
            return array;
        }
        case WebCLGetInfo::kTypeFloat:
            return v8::Number::New(info.getFloat());
        case WebCLGetInfo::kTypeInt:
            return v8::Integer::New(info.getInt());
        case WebCLGetInfo::kTypeNull:
            return v8::Null(isolate);
        case WebCLGetInfo::kTypeString:
            return v8::String::New(fromWebCoreString(info.getString()), info.getString().length());
        case WebCLGetInfo::kTypeUnsignedInt:
            return v8::Integer::NewFromUnsigned(info.getUnsignedInt());
        case WebCLGetInfo::kTypeUnsignedLong:
            //return v8::Interger::NewFromUnsigned(info.getUnsignedInt());
        case WebCLGetInfo::kTypeWebCLFloatArray:
            return toV8(info.getWebCLFloatArray(), isolate);
        case WebCLGetInfo::kTypeWebCLIntArray:
            return toV8(info.getWebCLIntArray(), isolate);
        case WebCLGetInfo::kTypeWebCLProgram:
            return toV8(info.getWebCLProgram(), isolate);
        case WebCLGetInfo::kTypeWebCLContext:
            return toV8(info.getWebCLContext(), isolate);
        case WebCLGetInfo::kTypeWebCLCommandQueue:
            return toV8(info.getWebCLCommandQueue(), isolate);
        case WebCLGetInfo::kTypeWebCLDevice:
            return toV8(info.getWebCLDevice(), isolate);
        case WebCLGetInfo::kTypeWebCLDeviceList:
            return toV8(info.getWebCLDeviceList(), isolate);
        default:
            notImplemented();
            return v8::Undefined();
    }
}

}

#endif // ENABLE(WEBCL)