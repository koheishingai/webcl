#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLGetInfo.h"

#include "Float32Array.h"
#include "Int32Array.h"
#include "Uint8Array.h"
#include "WebCLProgram.h"
#include "WebCLContext.h"


namespace WebCore {

WebCLGetInfo::WebCLGetInfo(bool value)
    : m_type(kTypeBool)
    , m_bool(value)
{
}

WebCLGetInfo::WebCLGetInfo(const bool* value, int size)
    : m_type(kTypeBoolArray)
{
    if (!value || size <=0)
        return;
    m_boolArray.resize(size);
    for (int ii = 0; ii < size; ++ii)
        m_boolArray[ii] = value[ii];
}

WebCLGetInfo::WebCLGetInfo(float value)
    : m_type(kTypeFloat)
    , m_float(value)
{
}

WebCLGetInfo::WebCLGetInfo(int value)
    : m_type(kTypeInt)
    , m_int(value)
{
}

WebCLGetInfo::WebCLGetInfo()
    : m_type(kTypeNull)
{
}

WebCLGetInfo::WebCLGetInfo(const String& value)
    : m_type(kTypeString)
    , m_string(value)
{
}

WebCLGetInfo::WebCLGetInfo(unsigned int value)
    : m_type(kTypeUnsignedInt)
    , m_unsignedInt(value)
{
}

WebCLGetInfo::WebCLGetInfo(unsigned long value)
    : m_type(kTypeUnsignedInt)
    , m_unsignedLong(value)
{
}

WebCLGetInfo::WebCLGetInfo(void* value)
    : m_type(kTypeVoidPointer)
    , m_voidPointer(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<Float32Array> value)
    : m_type(kTypeWebCLFloatArray)
    , m_webclFloatArray(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<Int32Array> value)
    : m_type(kTypeWebCLIntArray)
    , m_webclIntArray(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLProgram> value)
    : m_type(kTypeWebCLProgram)
    , m_webclProgram(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLContext> value)
    : m_type(kTypeWebCLContext)
    , m_webclContext(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLCommandQueue> value)
    : m_type(kTypeWebCLCommandQueue)
    , m_webCLCommandQueue(value)
{
}

WebCLGetInfo::~WebCLGetInfo()
{
}

WebCLGetInfo::Type WebCLGetInfo::getType() const
{
    return m_type;
}

bool WebCLGetInfo::getBool() const
{
    ASSERT(getType() == kTypeBool);
    return m_bool;
}

const Vector<bool>& WebCLGetInfo::getBoolArray() const
{
    ASSERT(getType() == kTypeBoolArray);
    return m_boolArray;
}

float WebCLGetInfo::getFloat() const
{
    ASSERT(getType() == kTypeFloat);
    return m_float;
}

int WebCLGetInfo::getInt() const
{
    ASSERT(getType() == kTypeInt);
    return m_int;
}

const String& WebCLGetInfo::getString() const
{
    ASSERT(getType() == kTypeString);
    return m_string;
}

unsigned int WebCLGetInfo::getUnsignedInt() const
{
    ASSERT(getType() == kTypeUnsignedInt);
    return m_unsignedInt;
}

unsigned long WebCLGetInfo::getUnsignedLong() const
{
    ASSERT(getType() == kTypeUnsignedLong);
    return m_unsignedLong;
}

void* WebCLGetInfo::getVoidPointer() const
{
    ASSERT(getType() == kTypeVoidPointer);
    return m_voidPointer;
}

PassRefPtr<Float32Array> WebCLGetInfo::getWebCLFloatArray() const
{
    ASSERT(getType() == kTypeWebCLFloatArray);
    return m_webclFloatArray;
}

PassRefPtr<Int32Array> WebCLGetInfo::getWebCLIntArray() const
{
    ASSERT(getType() == kTypeWebCLIntArray);
    return m_webclIntArray;
}

PassRefPtr<WebCLContext> WebCLGetInfo::getWebCLContext() const
{
    ASSERT(getType() == kTypeWebCLContext);
    return m_webclContext;
}
PassRefPtr<WebCLProgram> WebCLGetInfo::getWebCLProgram() const
{
    ASSERT(getType() == kTypeWebCLProgram);
    return m_webclProgram;
}
PassRefPtr<WebCLCommandQueue> WebCLGetInfo::getWebCLCommandQueue() const
{
    ASSERT(getType() == kTypeWebCLCommandQueue);
    return m_webCLCommandQueue;
}

} // namespace WebCore

#endif // ENABLE(WEBGL)
