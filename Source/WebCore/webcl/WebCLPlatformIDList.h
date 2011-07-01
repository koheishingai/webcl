/*
 *  WebCLPlatformIDList.h
 *  WebCore
 *
 *  Created by Won Jeon on 2/23/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#ifndef WebCLPlatformIDList_h
#define WebCLPlatformIDList_h

#include <Opencl/opencl.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

#include "WebCLPlatformID.h"

namespace WebCore {

class WebCLComputeContext;

class WebCLPlatformIDList : public RefCounted<WebCLPlatformIDList> {

public:
	virtual ~WebCLPlatformIDList();
	static PassRefPtr<WebCLPlatformIDList> create(WebCLComputeContext*);
	WebCLPlatformIDList();
	cl_platform_id getCLPlatformIDs();
	
	unsigned length() const;
	WebCLPlatformID* item(unsigned index);
	
private:
	WebCLPlatformIDList(WebCLComputeContext*);

	WebCLComputeContext* m_context;

	Vector<RefPtr<WebCLPlatformID> > m_platform_id_list;
	cl_platform_id* m_cl_platforms;
	cl_uint m_num_platforms;
	
};

} // namespace WebCore

#endif // WebCLPlatformIDList_h
