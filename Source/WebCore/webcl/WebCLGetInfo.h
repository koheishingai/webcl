#ifndef WebCLGetInfo_h
#define WebCLGetInfo_h

#include "Float32Array.h"
#include "Int32Array.h"
#include "PlatformString.h"
#include "Uint8Array.h"
#include "WebCLProgram.h"
#include "WebCLContext.h"
#include "WebCLCommandQueue.h"

#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

namespace WebCore {

	// A tagged union representing the result of get queries like
	// getParameter (encompassing getBooleanv, getIntegerv, getFloatv) and
	// similar variants. For reference counted types, increments and
	// decrements the reference count of the target object.

	class WebCLGetInfo {
		public:
			enum Type {
				kTypeBool,
				kTypeBoolArray,
				kTypeFloat,
				kTypeInt,
				kTypeNull,
				kTypeString,
				kTypeUnsignedInt,   
				kTypeUnsignedLong,
				kTypeVoidPointer,
				kTypeWebCLFloatArray,       
				kTypeWebCLIntArray,
				kTypeWebCLObjectArray,
				kTypeWebCLProgram,
				kTypeWebCLContext,
				kTypeWebCLCommandQueue,
			};

			WebCLGetInfo(bool value);
			WebCLGetInfo(const bool* value, int size);
			WebCLGetInfo(float value);
			WebCLGetInfo(int value);
			WebCLGetInfo();
			WebCLGetInfo(const String& value);
			WebCLGetInfo(unsigned int value); 
			WebCLGetInfo(unsigned long value);   	
			WebCLGetInfo(void* value);
			WebCLGetInfo(PassRefPtr<Float32Array> value);
			WebCLGetInfo(PassRefPtr<Int32Array> value);
			WebCLGetInfo(PassRefPtr<WebCLProgram> value);
			WebCLGetInfo(PassRefPtr<WebCLContext> value);
			WebCLGetInfo(PassRefPtr<WebCLCommandQueue> value);

			virtual ~WebCLGetInfo();

			Type getType() const;

			bool getBool() const;
			const Vector<bool>& getBoolArray() const;
			float getFloat() const;
			int getInt() const;
			const String& getString() const;
			unsigned int getUnsignedInt() const;
			unsigned long getUnsignedLong() const;
			void* getVoidPointer() const;
			PassRefPtr<Float32Array> getWebCLFloatArray() const;
			PassRefPtr<Int32Array> getWebCLIntArray() const;
			PassRefPtr<WebCLProgram> getWebCLProgram() const;
			PassRefPtr<WebCLContext> getWebCLContext() const;
			PassRefPtr<WebCLCommandQueue> getWebCLCommandQueue() const;

		private:
			Type m_type;
			bool m_bool;
			Vector<bool> m_boolArray;
			float m_float;
			int m_int;
			String m_string;
			unsigned int m_unsignedInt;
			unsigned long m_unsignedLong;
			void* m_voidPointer;
			RefPtr<Float32Array> m_webclFloatArray;
			RefPtr<Int32Array> m_webclIntArray;
			RefPtr<WebCLProgram> m_webclProgram;
			RefPtr<WebCLContext> m_webclContext;
			RefPtr<WebCLCommandQueue> m_webCLCommandQueue;
	};

} // namespace WebCore

#endif // WebCLGetInfo_h
