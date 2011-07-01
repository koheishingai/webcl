/*
 *  WebCLPlatformIDList.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/23/11.
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLPlatformIDList.h"
#include "WebCLComputeContext.h"

namespace WebCore {

WebCLPlatformIDList::WebCLPlatformIDList()
{
}

WebCLPlatformIDList::~WebCLPlatformIDList()
{
}

PassRefPtr<WebCLPlatformIDList> WebCLPlatformIDList::create(WebCLComputeContext* ctx)
{
	printf("WebCLPlatformIDList::create\n");
	
	return adoptRef(new WebCLPlatformIDList(ctx));
}

WebCLPlatformIDList::WebCLPlatformIDList(WebCLComputeContext* ctx) : m_context(ctx)
{
	printf("WebCLPlatformIDList::WebCLPlatformIDList\n");
	
	cl_int err;
	
	err = clGetPlatformIDs(0, NULL, &m_num_platforms);
	if (err != CL_SUCCESS) {
		printf("Error: Failed to get platform IDs\n");
	}
	
	m_cl_platforms = new cl_platform_id[m_num_platforms];
	err = clGetPlatformIDs(m_num_platforms, m_cl_platforms, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: Failed to create a platform\n");
	}
	
	for (unsigned int i = 0 ; i < m_num_platforms; i++) {
		RefPtr<WebCLPlatformID> o = WebCLPlatformID::create(m_context, m_cl_platforms[i]);
		if (o != NULL) {
			m_platform_id_list.append(o);
		} else {
			printf("Error: o null\n");
		}
	}
	
}

cl_platform_id WebCLPlatformIDList::getCLPlatformIDs()
{
	return *m_cl_platforms;
}

unsigned WebCLPlatformIDList::length() const
{
	//return m_list.size();
	return m_num_platforms;
}

WebCLPlatformID* WebCLPlatformIDList::item(unsigned index)
{
	printf("WebCLPlatformIDList::item index %d\n", index);
	
	if (index >= m_num_platforms) {
		return 0;
	}
	//return (WebCLPlatformID*)(m_cl_platform_id[index]);
	WebCLPlatformID* ret = (m_platform_id_list[index]).get();
	return ret;
}


} // namespace WebCore

#endif // ENABLE(WEBCL)
