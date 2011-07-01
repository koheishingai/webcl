/*
 *  WebCLComputeContext.cpp
 *  WebCore
 *
 *  Created by Won Jeon on 2/15/11.
 *  Changed by Siba Samal on 25/05/11
 *  Copyright 2011 Samsung. All rights reserved.
 *
 */

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLComputeContext.h"

#include "ScriptExecutionContext.h"
#include "Document.h"
#include "DOMWindow.h"
#include "Image.h"
#include "SharedBuffer.h"
#include "CanvasRenderingContext2D.h"
#include <CanvasRenderingContext.h>
#include "ImageBuffer.h"
#include <stdio.h>
#include "CachedImage.h"
#include "ArrayBuffer.h"
#include <wtf/ByteArray.h>
#include "CanvasPixelArray.h"
#include "HTMLCanvasElement.h"

class CanvasRenderingContext;

using namespace JSC;

namespace WebCore 
{

	WebCLComputeContext::WebCLComputeContext(ScriptExecutionContext* context) : ActiveDOMObject(context, this)
										    , m_videoCache(4)
	{
		printf("WebCLComputeContext::WebCLComputeContext\n");
		m_error_state = SUCCESS;
		m_num_mems = 0;
		m_num_programs = 0;
		m_num_kernels = 0;
		m_num_events = 0;
		m_num_samplers = 0;
	}

	PassRefPtr<WebCLComputeContext> WebCLComputeContext::create(ScriptExecutionContext* context)
	{
		printf("WebCLComputeContext::create\n");

		return adoptRef(new WebCLComputeContext(context));
	}

	WebCLComputeContext::~WebCLComputeContext()
	{
	}

	void WebCLComputeContext::hello()
	{
		printf("WebCLComputeContext::hello\n");
	}

	PassRefPtr<WebCLPlatformIDList> WebCLComputeContext::getPlatformIDs()
	{
		printf("WebCLComputeContext::getPlatformIDs\n");

		RefPtr<WebCLPlatformIDList> o = WebCLPlatformIDList::create(this);
		if (o != NULL) {
			m_error_state = SUCCESS;
			m_platform_id = o;
			return o;
		} else {
			m_error_state = FAILURE;
			return NULL;
		}
	}

	PassRefPtr<WebCLDeviceIDList> WebCLComputeContext::getDeviceIDs(WebCLPlatformID* webcl_platform_id, 
			int device_type)
	{
		cl_platform_id platform_id;

		printf("WebCLComputeContext::getDeviceIDs device_type %d\n", device_type);

		if (webcl_platform_id != NULL) {
			platform_id = webcl_platform_id->getCLPlatformID();
			if (platform_id == NULL) {
				printf("Error: platform_id null\n");
				return NULL;
			}
		} else {
			printf("webcl_platform_id null\n");
			return NULL;
		}

		RefPtr<WebCLDeviceIDList> o = WebCLDeviceIDList::create(this, platform_id, 
				device_type);
		if (o != NULL) {
			m_error_state = SUCCESS;
			m_device_id = o;
			return o;
		} else {
			m_error_state = FAILURE;
			return NULL;
		}

		return NULL;
	}

	WebCLGetInfo WebCLComputeContext::getPlatformInfo (WebCLPlatformID* webcl_platform_id,
			int platform_info)
	{
		cl_platform_id platform_id;

		printf("WebCLComputeContext::getPlatformInfo platform info %d\n", platform_info);

		if (webcl_platform_id != NULL) {
			platform_id = webcl_platform_id->getCLPlatformID();
			if (platform_id == NULL) {
				printf("Error: platform_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		else {
			printf("webcl_platform_id null\n");
			return WebCLGetInfo();
		}

		char platform_string[1024];
		// TODO (siba) - Remove the duplication of clGetPlatformInfo() cases
		switch(platform_info)
		{
			case PLATFORM_PROFILE:
				clGetPlatformInfo(platform_id, CL_PLATFORM_PROFILE, sizeof(platform_string), &platform_string, NULL);
				printf("SUCCESS: Cl Platform profile = %s\n",platform_string );
				return WebCLGetInfo(String(platform_string));
			case PLATFORM_VERSION:
				clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, sizeof(platform_string), &platform_string, NULL);
				printf("SUCCESS: Cl Platform version = %s\n",platform_string );
				return WebCLGetInfo(String(platform_string));
			case PLATFORM_NAME:
				clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, sizeof(platform_string), &platform_string, NULL);
				printf("SUCCESS: Cl Platform Name = %s\n",platform_string );
				return WebCLGetInfo(String(platform_string));
			case PLATFORM_VENDOR:
				clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, sizeof(platform_string), &platform_string, NULL);
				printf("SUCCESS: Cl Platform Vendor = %s\n",platform_string );
				return WebCLGetInfo(String(platform_string));
			case PLATFORM_EXTENSIONS:
				clGetPlatformInfo(platform_id, CL_PLATFORM_EXTENSIONS, sizeof(platform_string), &platform_string, NULL);
				printf("SUCCESS: Cl Platform Extensions = %s\n",platform_string );
				return WebCLGetInfo(String(platform_string));
			default:
				m_error_state = FAILURE;
				printf("ERROR: UNSUPPORTED Platform Info type = %d ",platform_info);
				strcpy(platform_string,"UNSUPPORTED PLATFORM INFO TYPE");
				return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo  WebCLComputeContext::getDeviceInfo(WebCLDeviceID*   webcl_device_id,  
			int device_type)
	{
		cl_device_id cl_device;
		printf("WebCLComputeContext::getDeviceInfo device_type %d\n", device_type);
		cl_device = webcl_device_id->getCLDeviceID();
		if (cl_device == NULL) {
			m_error_state = FAILURE;
			printf("Error: devices null\n");
			return WebCLGetInfo();
		}

		switch(device_type)
		{
			char device_string[1024];
			// Handling string cases
			case DEVICE_EXTENSIONS:
			clGetDeviceInfo(cl_device, CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Device Extensions = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			case DEVICE_NAME:
			clGetDeviceInfo(cl_device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Device Name = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			case DEVICE_PROFILE:
			clGetDeviceInfo(cl_device, CL_DEVICE_PROFILE, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Device Profile = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			case DEVICE_VENDOR:
			clGetDeviceInfo(cl_device, CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Device Vendor = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			case DEVICE_VERSION:
			clGetDeviceInfo(cl_device, CL_DEVICE_VERSION, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Device Version = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			case DRIVER_VERSION:
			clGetDeviceInfo(cl_device, CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
			printf("SUCCESS: Cl Driver Version = %s\n",device_string );
			return WebCLGetInfo(String(device_string));	

			// Handling uint cases
			{
				cl_uint uint_units;
				case DEVICE_ADDRESS_BITS:
				clGetDeviceInfo(cl_device, CL_DEVICE_ADDRESS_BITS , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Device Address Bits = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Global Mem Cacheline Size = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_MAX_CLOCK_FREQUENCY:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Max Clock Frequency = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case DEVICE_MAX_CONSTANT_ARGS:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Max Constant Args = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_MAX_READ_IMAGE_ARGS:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Max Read Image Args = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case DEVICE_MAX_SAMPLERS:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_SAMPLERS, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Max Samplers = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case DEVICE_MAX_WRITE_IMAGE_ARGS:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Max Write Image Args = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_MEM_BASE_ADDR_ALIGN:
				clGetDeviceInfo(cl_device, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Mem Base Addr Align = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Min Data Type Align Size = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_VENDOR_ID:
				clGetDeviceInfo(cl_device, CL_DEVICE_VENDOR_ID, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device Vendor Id = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Char = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Short = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_INT:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Int = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Long = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Float = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
				clGetDeviceInfo(cl_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &uint_units,NULL);
				printf("SUCCESS: Cl Device prefered Vector Width Double = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));

				case DEVICE_MAX_COMPUTE_UNITS:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_COMPUTE_UNITS , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Device Max Compute units =  %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));


			}
			// Handling size_t cases
			{
				size_t sizet_units;

				case DEVICE_IMAGE2D_MAX_HEIGHT:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Imaged2d Max Height = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_IMAGE2D_MAX_WIDTH:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Imaged2d Max Width = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_IMAGE3D_MAX_DEPTH:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Imaged3d Max Depth = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_IMAGE3D_MAX_HEIGHT:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Imaged3d Max Height = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case DEVICE_IMAGE3D_MAX_WIDTH:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Imaged3d Max Width = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_MAX_PARAMETER_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Max Parameter  Size = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_MAX_WORK_GROUP_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Max Work Group SIze = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_MAX_WORK_ITEM_SIZES:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Max Work Item Sizes = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_PROFILING_TIMER_RESOLUTION:
				clGetDeviceInfo(cl_device, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Profiling Timer Resolution = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));

				case DEVICE_MAX_WORK_ITEM_DIMENSIONS:
				clGetDeviceInfo(cl_device, DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Device Max work item diemnsions = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));		
			}
			// Handling ulong cases
			{
				cl_ulong  ulong_units;

				case DEVICE_LOCAL_MEM_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Device Local Mem Size = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
				case DEVICE_MAX_CONSTANT_BUFFER_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Device Max Constant Buffer Size = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
				case DEVICE_MAX_MEM_ALLOC_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Device Max Mem Alloc  Size = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
				case DEVICE_GLOBAL_MEM_CACHE_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Device Global Mem Cache Size = %u\n",(unsigned int)ulong_units );
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
				case DEVICE_GLOBAL_MEM_SIZE:
				clGetDeviceInfo(cl_device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Device Global Mem Size  = %u\n",(unsigned int)ulong_units );
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));

			}
			{
				cl_bool bool_units;
				case DEVICE_AVAILABLE:
				clGetDeviceInfo(cl_device, CL_DEVICE_AVAILABLE , sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Device Availablie = %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));

				case DEVICE_COMPILER_AVAILABLE:
				clGetDeviceInfo(cl_device, CL_DEVICE_COMPILER_AVAILABLE , sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Device compiler available = %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));

				case DEVICE_ENDIAN_LITTLE:
				clGetDeviceInfo(cl_device, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Device Endian Little = %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));

				case DEVICE_ERROR_CORRECTION_SUPPORT:
				clGetDeviceInfo(cl_device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Cl Device Error Correction Support= %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));

				case DEVICE_IMAGE_SUPPORT:
				clGetDeviceInfo(cl_device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Cl Device Image Support = %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));
			}
			// Handling other cases 
			{
				cl_device_type type;
				case DEVICE_TYPE:
				clGetDeviceInfo(cl_device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
				if( type & CL_DEVICE_TYPE_CPU )
					strcpy(device_string, "CL_DEVICE_TYPE_CPU");
				else if ( type & CL_DEVICE_TYPE_GPU )
					strcpy(device_string, "CL_DEVICE_TYPE_GPU");
				else if ( type & CL_DEVICE_TYPE_ACCELERATOR)
					strcpy(device_string, "CL_DEVICE_TYPE_ACCELERATOR");
				else if ( type & CL_DEVICE_TYPE_DEFAULT)
					strcpy(device_string, "CL_DEVICE_TYPE_DEFAULT");
				else 
					strcpy(device_string, "CL_DEVICE_TYPE_ERROR");
				printf("SUCCESS: Cl Device Type = %s\n",device_string );
				printf("SUCCESS: Cl Device  = %u\n",(unsigned int)type) ;
				return WebCLGetInfo(static_cast<unsigned int>(type));
			} 
			{
				cl_command_queue_properties queue_properties;	
				case DEVICE_QUEUE_PROPERTIES:
				clGetDeviceInfo(cl_device, CL_DEVICE_QUEUE_PROPERTIES, 
						sizeof(cl_command_queue_properties), &queue_properties, NULL);
				if( queue_properties  & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE)
					strcpy(device_string, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
				else if ( queue_properties & CL_QUEUE_PROFILING_ENABLE)
					strcpy(device_string, "CL_QUEUE_PROFILING_ENABLE");
				else 
					strcpy(device_string, "CL_QUEUE_ERROR");
				printf("SUCCESS: Cl Device Queue Properties = %s\n",device_string );
				printf("SUCCESS: Cl Device Queue  = %u\n",(unsigned int)queue_properties );
				return WebCLGetInfo(static_cast<unsigned int>(queue_properties));
			} 
			{

				//cl_device_fp_config fp_config;
				// Part of cl_ext.h (which isn't available in Khronos)
				//case DEVICE_DOUBLE_FP_CONFIG:
				//clGetDeviceInfo(cl_device, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
				//case DEVICE_HALF_FP_CONFIG:
				//clGetDeviceInfo(cl_device, CL_DEVICE_HALF_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
				//case DEVICE_SINGLE_FP_CONFIG:
				//clGetDeviceInfo(cl_device, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
				//return WebCLGetInfo(static_cast<unsigned int>(fp_config));

			}
			//Platform ID is not Supported
			//case DEVICE_PLATFORM:
			//cl_platform_id platform_id;
			//clGetDeviceInfo(cl_device, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform_id, NULL);
			//return WebCLGetInfo(static_cast<unsigned int>(platform_id));

			// TODO : Siba Bitfield Support
			case DEVICE_EXECUTION_CAPABILITIES:
			cl_device_exec_capabilities exec;
			clGetDeviceInfo(cl_device, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(cl_device_exec_capabilities), &exec, NULL);
			if( exec  & CL_EXEC_KERNEL )
				strcpy(device_string, "CL_EXEC_KERNEL");
			else if ( exec & CL_EXEC_NATIVE_KERNEL )
				strcpy(device_string, "CL_EXEC_NATIVE_KERNEL");
			else 
				strcpy(device_string, "CL_EXEC_CAPABILTY__ERROR");
			printf("SUCCESS: Cl Device Execution Capabilities = %s\n",device_string );
			printf("SUCCESS: Cl Device Execution Capabilities  = %u\n",(unsigned int)exec);
			return WebCLGetInfo(static_cast<unsigned int>(exec));


			case DEVICE_GLOBAL_MEM_CACHE_TYPE:
			cl_device_mem_cache_type global_type;
			clGetDeviceInfo(cl_device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &global_type, NULL);
			if( global_type  & CL_NONE )
				strcpy(device_string, "CL_NONE");
			else if ( global_type & CL_READ_ONLY_CACHE )
				strcpy(device_string, "CL_READ_ONLY_CACHE");
			else if ( global_type & CL_READ_WRITE_CACHE )
				strcpy(device_string, "CL_READ_WRITE_CACHE");
			else
				strcpy(device_string, "GLOBAL_MEM_CACHE_TYPE_ERROR");
			printf("SUCCESS: Cl Device Global Mem Cache Type = %s\n",device_string );
			printf("SUCCESS: Cl Device Global Mem Cache Type  = %u\n",(unsigned int)global_type);
			return WebCLGetInfo(static_cast<unsigned int>(global_type));

			case DEVICE_LOCAL_MEM_TYPE:
			cl_device_local_mem_type local_type;
			clGetDeviceInfo(cl_device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &local_type, NULL);
			if( local_type  & CL_LOCAL  )
				strcpy(device_string, "CL_LOCAL ");
			else if (local_type & CL_GLOBAL)
				strcpy(device_string, "CL_GLOBAL");
			else
				strcpy(device_string, "LOCLA_MEM_TYPE_ERROR");
			printf("SUCCESS: Cl Device Local Mem Type = %s\n",device_string );
			printf("SUCCESS: Cl Device Local Mem Cache Type  = %u\n",(unsigned int)local_type);
			return WebCLGetInfo(static_cast<unsigned int>(local_type));

			default:
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED DEVICE TYPE = %d ",device_type);
			return WebCLGetInfo(String("UNSUPPORTED DEVICE TYPE"));	

		}

		m_error_state = FAILURE;
		return  WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getKernelInfo (WebCLKernel* kernel, int kernel_info)
	{
		printf("WebCLComputeContext::getKernelInfo kernel info %d\n", kernel_info);
		cl_kernel cl_kernel_id = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}

		switch(kernel_info)
		{
			case KERNEL_FUNCTION_NAME:
				char function_name[1024];
				clGetKernelInfo(cl_kernel_id, CL_KERNEL_FUNCTION_NAME, sizeof(function_name), &function_name, NULL);
				printf("SUCCESS: Cl function_name = %s\n",function_name );
				return WebCLGetInfo(String(function_name));
				{
					cl_uint uint_units;
					case KERNEL_NUM_ARGS:
					clGetKernelInfo(cl_kernel_id, CL_KERNEL_NUM_ARGS , sizeof(cl_uint), &uint_units, NULL);
					printf("SUCCESS: Kernel Num Args = %u\n",uint_units );
					return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
					case KERNEL_REFERENCE_COUNT:
					clGetKernelInfo(cl_kernel_id, CL_KERNEL_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
					printf("SUCCESS: Device Address Bits = %u\n",uint_units );
					return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				}
				{
					case KERNEL_PROGRAM:
						cl_program cl_program_id;
						clGetKernelInfo(cl_kernel_id, CL_KERNEL_PROGRAM, sizeof(cl_program_id), &cl_program_id, NULL);
						RefPtr<WebCLProgram> obj = WebCLProgram::create(this, cl_program_id);
						return WebCLGetInfo(PassRefPtr<WebCLProgram>(obj));
				}
				{
					case KERNEL_CONTEXT:
						cl_context cl_context_id;
						clGetKernelInfo(cl_kernel_id, CL_KERNEL_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
						RefPtr<WebCLContext> obj = WebCLContext::create(this, cl_context_id);
						return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
				}
			default:
				char platform_string[1024];
				m_error_state = FAILURE;
				printf("ERROR: UNSUPPORTED kernel Info type = %d ",kernel_info);
				strcpy(platform_string,"UNSUPPORTED KERNEL INFO TYPE");
				return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getProgramInfo (WebCLProgram* program, int param_name)
	{
		printf("WebCLComputeContext::getProgramInfo program info %d\n", param_name);
		cl_program cl_program_id = NULL;

		if (program != NULL) {
			cl_program_id = program->getCLProgram();
			if (cl_program_id == NULL) {
				printf("Error: cl_program_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		switch(param_name)
		{   
			{
				cl_uint uint_units;
				case PROGRAM_REFERENCE_COUNT:
				clGetProgramInfo(cl_program_id, CL_PROGRAM_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Program Reference Count = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case PROGRAM_NUM_DEVICES:
				clGetProgramInfo(cl_program_id, CL_PROGRAM_NUM_DEVICES , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Program Num Devices = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}			
			{
				case PROGRAM_BINARY_SIZES:
					size_t sizet_units;
					clGetProgramInfo(cl_program_id, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &sizet_units, NULL);
					printf("SUCCESS: Cl Program Binary Sizes = %lu\n",(unsigned long)sizet_units);
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			}
			{
				// Handling string cases
				char program_string[4096];
				case PROGRAM_SOURCE:
				clGetProgramInfo(cl_program_id, CL_PROGRAM_SOURCE, sizeof(program_string), &program_string, NULL);
				printf("SUCCESS: Cl Dprogram source = %s\n",program_string );
				return WebCLGetInfo(String(program_string));	

				//TODO - Check for unsigbed char* - Add in WebGLGetInfo??
				case PROGRAM_BINARIES:
				clGetProgramInfo(cl_program_id, CL_PROGRAM_BINARIES, sizeof(program_string), &program_string, NULL);
				printf("SUCCESS: Cl Dprogram binary = %s\n",program_string );
				return WebCLGetInfo(String(program_string));	

			}
			{
				case PROGRAM_CONTEXT:
					cl_context cl_context_id;
					clGetProgramInfo(cl_program_id, CL_PROGRAM_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
					RefPtr<WebCLContext> obj = WebCLContext::create(this, cl_context_id);
					if(obj != NULL)
						printf("SUCCESS: CL program context not NULL\n");
					return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));

			}
			{
				// TODO- Handle Array of cl_device_id ??
				/*case PROGRAM_DEVICES:
				  size_t numDevices;
				  clGetProgramInfo( cl_program_id, CL_PROGRAM_DEVICES, 0, 0, &numDevices );
				  cl_device_id *devices = new cl_device_id[numDevices];
				  clGetProgramInfo( cl_program_id, CL_PROGRAM_DEVICES, numDevices, devices, &numDevices );
				  return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
				 */
			}
			default:
			char program_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED program Info type = %d ",param_name);
			strcpy(program_string,"UNSUPPORTED PROGRAM INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getProgramBuildInfo(WebCLProgram* program, WebCLDeviceID* device, 
			int param_name)
	{
		printf("WebCLComputeContext::getProgramBuildInfo param_name=%d\n", param_name);

		cl_program program_id = NULL;
		cl_device_id device_id = NULL;
		cl_uint err = 0;

		if (program != NULL) {
			program_id = program->getCLProgram();
			if (program_id == NULL) {
				printf("Error: program_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		if (device != NULL) {
			device_id = device->getCLDeviceID();
			if (device_id == NULL) {
				printf("Error: device_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}

		size_t len;

		switch (param_name) {
			{
				char buffer[8192];
				case PROGRAM_BUILD_LOG:
				err = clGetProgramBuildInfo(program_id, device_id, CL_PROGRAM_BUILD_LOG, 
						sizeof(buffer), buffer, &len);
				if (err != CL_SUCCESS) {
					switch (err) {
						case CL_SUCCESS:
						case CL_INVALID_DEVICE:
							printf("clGetProgramBuildInfo CL_INVALID_DEVICE\n");
							break;
						case CL_INVALID_VALUE:
							printf("clGetProgramBuildInfo CL_INVALID_VALUE\n");
							break;
						case CL_INVALID_PROGRAM:
							printf("clGetProgramBuildInfo CL_INVALID_PROGRAM\n");
							break;
					}
				} else {
					printf("clGetProgramBuildInfo CL_SUCCESS\n");
					printf("buffer %s\n", buffer);
					return WebCLGetInfo(String(buffer));
					break;
				}

				case PROGRAM_BUILD_OPTIONS:
				clGetProgramBuildInfo(program_id, device_id, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), &buffer, NULL);
				printf("SUCCESS: Cl Program Build Options = %s\n",buffer );
				return WebCLGetInfo(String(buffer));
			}
			{
				case PROGRAM_BUILD_STATUS:
					cl_build_status build_status;
					clGetProgramBuildInfo(program_id, device_id, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
					printf("SUCCESS: Cl program Build Status  = %u\n",(unsigned int)build_status);
					return WebCLGetInfo(static_cast<unsigned int>(build_status));				
			}
			default:
			char programbuild_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED Program Build Info   Type = %d ",param_name);
			strcpy(programbuild_string,"UNSUPPORTED PROGRAM BUILD INFO TYPE");
			return WebCLGetInfo();			
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();

	}

	WebCLGetInfo WebCLComputeContext:: getCommandQueueInfo(WebCLCommandQueue* command_queue, int param_name)
	{
		printf("WebCLComputeContext::getCommandQueueInfo param name %d\n", param_name);
		cl_command_queue cl_command_queue_id = NULL;
		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}			
		switch(param_name)
		{
			{
				case QUEUE_REFERENCE_COUNT:
					cl_uint uint_units;
					clGetCommandQueueInfo(cl_command_queue_id, CL_QUEUE_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
					printf("SUCCESS:cl Queue Reference Count = %u\n",uint_units );
					return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}
			{
				case QUEUE_CONTEXT:
					cl_context cl_context_id;
					clGetCommandQueueInfo(cl_command_queue_id, CL_QUEUE_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
					RefPtr<WebCLContext> obj = WebCLContext::create(this, cl_context_id);
					if(obj != NULL)
						printf("SUCCESS: CL program context not NULL\n");
					return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));

			}
			/*
			   {
			   case QUEUE_DEVICE:
			   cl_device_id device_id;
			   clGetCommandQueueInfo(cl_command_queue_id, CL_QUEUE_DEVICE, sizeof(cl_device_id), &device_id, NULL);
			   printf("SUCCESS: Cl Device ID  = %u\n",(unsigned int)device_id );
			   return WebCLGetInfo(static_cast<unsigned int>(device_id));							
			   }
			   {
			   case QUEUE_PROPERTIES:
			   cl_command_queue_properties queue_properties;
			   clGetCommandQueueInfo(cl_command_queue_id, CL_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties), &queue_properties, NULL);
			   printf("SUCCESS: Cl Queue Properties = %u\n",(unsigned int)queue_properties );
			   return WebCLGetInfo(static_cast<unsigned int>(queue_properties));

			   }
			 */
			default:
			char commandqueue_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED Command Queue Info type = %d ",param_name);
			strcpy(commandqueue_string,"UNSUPPORTED COMMAND QUEUE INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getContextInfo(WebCLContext* context_id, int param_name)
	{
		printf("WebCLComputeContext::getContextInfo param name%d\n", param_name);

		cl_context cl_context_id = NULL;
		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}

		switch(param_name)
		{	
			{
				cl_uint uint_units;
				case CONTEXT_REFERENCE_COUNT:
				clGetContextInfo(cl_context_id, CL_CONTEXT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS:cl Context Reference Count = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}
			/*
			   return_type cl_device_id[];
			   case CONTEXT_DEVICES:
			 */
			/*
			   return_type cl_context_properties[]	;
			   case CONTEXT_PROPERTIES:
			 */			
			default:
			char context_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED context Info type = %d ",param_name);
			strcpy(context_string,"UNSUPPORTED CONTEXT INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getKernelWorkGroupInfo(WebCLKernel* kernel, WebCLDeviceID* device, int param_name)
	{
		printf("WebCLComputeContext::getKernelWorkGroupInfo\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;
		cl_device_id cl_device = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		if (device != NULL) {
			cl_device = device->getCLDeviceID();
			if (cl_device == NULL) {
				printf("Error: cl_device null\n");
				m_error_state = FAILURE;
				return  WebCLGetInfo();
			}
		}
		switch (param_name) {
			{
				size_t sizet_units;
				case KERNEL_WORK_GROUP_SIZE:
				err = clGetKernelWorkGroupInfo(cl_kernel_id, cl_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &sizet_units, NULL);
				printf("sizeof(ret)=%ld ret=%ld\n", sizeof(sizet_units), sizet_units);
				if (err != CL_SUCCESS) {
					printf("Error: clGetKerelWorkGroupInfo\n");
					switch (err) {
						case CL_INVALID_DEVICE:
							printf("CL_INVALID_DEVICE\n");
							break;
						case CL_INVALID_VALUE:
							printf("CL_INVALID_VALUE\n");
							break;
						case CL_INVALID_KERNEL:
							printf("CL_INVALID_KERNEL\n");
							break;
						default:
							printf("What else?\n");
							break;
					}
					m_error_state = FAILURE;
					return WebCLGetInfo();
				} else {
					printf("Success: clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE = %u \n",(unsigned int)sizet_units );
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				}

				case KERNEL_COMPILE_WORK_GROUP_SIZE:
				clGetKernelWorkGroupInfo(cl_kernel_id, cl_device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Kernel Compile Work Group Size = %u\n",(unsigned int)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			}
			{
				case KERNEL_LOCAL_MEM_SIZE:
					cl_ulong  ulong_units;
					clGetKernelWorkGroupInfo(cl_kernel_id, cl_device, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
					printf("SUCCESS: Cl Kernel Local Mem Size = %lu\n",(unsigned long)ulong_units );
					return WebCLGetInfo(static_cast<unsigned long>(ulong_units));
			}
			default:
			char  kernel_work_gruop_info_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED Event Info type = %d ",param_name);
			strcpy(kernel_work_gruop_info_string,"UNSUPPORTED KERNEL WORK GROUP INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getMemObjectInfo(WebCLMem* memobj,int param_name)
	{
		printf("WebCLComputeContext::getMemObjectInfo param name %d\n", param_name);
		cl_mem cl_mem_id = NULL;
		if (memobj != NULL) {
			cl_mem_id = memobj->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		switch(param_name)
		{   	
			{
				cl_uint uint_units;
				case MEM_MAP_COUNT:
				clGetMemObjectInfo(cl_mem_id, CL_MEM_MAP_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: cl Mem map Count = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case MEM_REFERENCE_COUNT:
				clGetMemObjectInfo(cl_mem_id, CL_MEM_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: cl Mem Reference Count = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}	

			{

				case MEM_SIZE:
					size_t sizet_units;
					clGetMemObjectInfo(cl_mem_id, CL_MEM_SIZE, sizeof(size_t), &sizet_units, NULL);
					printf("SUCCESS: Cl Mem Size = %u\n",(unsigned int)sizet_units);
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			}
			{
				case MEM_TYPE:			
					cl_mem_object_type mem_type;
					clGetMemObjectInfo(cl_mem_id, CL_MEM_TYPE, sizeof(cl_mem_object_type), &mem_type, NULL);
					printf("SUCCESS: Cl Mem Object type  = %u\n",(unsigned int)mem_type);
					return WebCLGetInfo(static_cast<unsigned int>(mem_type));	
			}
			{
				case MEM_CONTEXT:			
					cl_context cl_context_id;
					clGetMemObjectInfo(cl_mem_id, CL_MEM_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
					RefPtr<WebCLContext> obj = WebCLContext::create(this, cl_context_id);
					if(obj != NULL)
						printf("SUCCESS: CL Mem context not NULL\n");
					return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
			}
			{
				case MEM_HOST_PTR:			
					void* mem_ptr;
					clGetMemObjectInfo(cl_mem_id, CL_MEM_HOST_PTR, sizeof(mem_ptr), &mem_ptr, NULL);
					printf("SUCCESS: Cl Mem Host Pointer \n");
					return WebCLGetInfo(mem_ptr);
			}
			default:
			char memobjectinfo_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED mem object  Info type = %d ",param_name);
			strcpy(memobjectinfo_string,"UNSUPPORTED MEM OBJECT INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getEventInfo(WebCLEvent* event, int param_name)
	{   
		printf("WebCLComputeContext::getEventInfo param name %d\n", param_name);
		cl_event cl_Event_id = NULL;
		if (event != NULL) {
			cl_Event_id = event->getCLEvent();
			if (cl_Event_id == NULL) {
				printf("Error: cl_Event_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		switch(param_name)
		{   
			{
				cl_uint uint_units;

				case EVENT_COMMAND_EXECUTION_STATUS :
				clGetEventInfo(cl_Event_id, CL_EVENT_COMMAND_EXECUTION_STATUS  , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Complete = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	

				case EVENT_REFERENCE_COUNT:
				clGetEventInfo(cl_Event_id, CL_EVENT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Event Reference Count  = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}
			{
				case EVENT_COMMAND_TYPE:			
					cl_command_type command_type;
					clGetEventInfo(cl_Event_id, CL_EVENT_COMMAND_TYPE , sizeof(cl_command_type), &command_type, NULL);;
					printf("SUCCESS: Cl Event Command type  = %u\n",(unsigned int)command_type);
					return WebCLGetInfo(static_cast<unsigned int>(command_type));	
			}
			{
				case EVENT_COMMAND_QUEUE:			
					cl_command_queue command_queue;
					clGetEventInfo(cl_Event_id, CL_EVENT_COMMAND_QUEUE , sizeof(cl_command_queue), &command_queue, NULL);;
					RefPtr<WebCLCommandQueue> obj = WebCLCommandQueue::create(this, command_queue);
					if(obj != NULL)
						printf("SUCCESS: Cl Event Command Queue\n");
					return WebCLGetInfo(PassRefPtr<WebCLCommandQueue>(obj));


			}			
			default:
			char  event_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED Event Info type = %d ",param_name);
			strcpy(event_string,"UNSUPPORTED EVENT INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext::getEventProfilingInfo(WebCLEvent* event, int param_name)
	{

		printf("WebCLComputeContext::getEventProfilingInfo param name %d\n", param_name);
		cl_event cl_event_id = NULL;
		if (event != NULL) {
			cl_event_id = event->getCLEvent();
			if (cl_event_id == NULL) {
				printf("Error: cl_event_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}
		switch(param_name)
		{   
			{
				cl_ulong  ulong_units;

				case PROFILING_COMMAND_QUEUED:
				clGetEventProfilingInfo(cl_event_id, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Profiling Command Queued = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));

				case PROFILING_COMMAND_SUBMIT:
				clGetEventProfilingInfo(cl_event_id, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Profiling Command Submit = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));

				case PROFILING_COMMAND_START:
				clGetEventProfilingInfo(cl_event_id, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Profiling Command Start = %lu\n",(unsigned long)ulong_units );
				return WebCLGetInfo(static_cast<unsigned long>(ulong_units));

				case PROFILING_COMMAND_END:
				clGetEventProfilingInfo(cl_event_id, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ulong_units, NULL);
				printf("SUCCESS: Cl Profiling Command End = %u\n",(unsigned int)ulong_units );
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));


			}
			default:
			char eventprofiling_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED event profillinf type = %d ",param_name);
			strcpy(eventprofiling_string,"UNSUPPORTED EVENT PROFILLING INFO");
			return WebCLGetInfo();

		}

		m_error_state = FAILURE;
		return  WebCLGetInfo();
	}

	WebCLGetInfo WebCLComputeContext:: getSamplerInfo(WebCLSampler* sampler, cl_sampler_info param_name)
	{
		printf("WebCLComputeContext::getSamplerInfo Image info %d\n", param_name);
		cl_sampler cl_sampler_id= NULL;

		if (sampler != NULL) {
			cl_sampler_id = sampler->getCLSampler();
			if (cl_sampler_id == NULL) {
				printf("Error: cl_sampler_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();

			}
		}
		switch(param_name)
		{   
			{
				cl_uint uint_units;
				case SAMPLER_REFERENCE_COUNT:
				clGetSamplerInfo (cl_sampler_id, CL_SAMPLER_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
				printf("SUCCESS: Sampler Reference Count = %u\n",uint_units );
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			}
			{
				cl_bool bool_units;
				case SAMPLER_NORMALIZED_COORDS:
				clGetSamplerInfo(cl_sampler_id, CL_SAMPLER_NORMALIZED_COORDS , sizeof(cl_bool), &bool_units, NULL);
				printf("SUCCESS: Sampler Normalized Coords = %s\n",(bool_units == CL_TRUE)? "yes" : "NO");
				return WebCLGetInfo(static_cast<bool>(bool_units));
			}
			{
				cl_context cl_context_id;
				case SAMPLER_CONTEXT:
				clGetSamplerInfo(cl_sampler_id, CL_SAMPLER_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
				RefPtr<WebCLContext> obj = WebCLContext::create(this, cl_context_id);
				return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
			}
			/*
			   {
			   return_type cl_addressing_mode;
			   case SAMPLER_ADDRESSING_MODE:
			   }
			   {
			   return_type cl_filter_mode;
			   case SAMPLER_FILTER_MODE:
			   }
			 */
			default:
			char Sampler_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED SamplerInfo type = %d ",param_name);
			strcpy(Sampler_string,"UNSUPPORTED SAMPLER INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();

	}
	WebCLGetInfo WebCLComputeContext::getImageInfo(WebCLImage* image, cl_image_info param_name) 	
	{
		printf("WebCLComputeContext::getImageInfo Image info %d\n", param_name);
		cl_mem cl_Image_id = NULL;

		if (image != NULL) {
			cl_Image_id = image->getCLImage();
			if (cl_Image_id == NULL) {
				printf("Error: cl_Image_id null\n");
				m_error_state = FAILURE;
				return WebCLGetInfo();
			}
		}

		switch(param_name)
		{   
			{
				size_t sizet_units;
				case IMAGE_ELEMENT_SIZE:
				clGetImageInfo(cl_Image_id, CL_IMAGE_ELEMENT_SIZE, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image Element Size = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case IMAGE_ROW_PITCH:
				clGetImageInfo(cl_Image_id, CL_IMAGE_ROW_PITCH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image Row Pitch = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case IMAGE_SLICE_PITCH:
				clGetImageInfo(cl_Image_id, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image Slice Pitch = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case IMAGE_WIDTH:
				clGetImageInfo(cl_Image_id, CL_IMAGE_WIDTH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image width = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case IMAGE_HEIGHT:
				clGetImageInfo(cl_Image_id, CL_IMAGE_HEIGHT, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image Height = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				case IMAGE_DEPTH:
				clGetImageInfo(cl_Image_id, CL_IMAGE_DEPTH, sizeof(size_t), &sizet_units, NULL);
				printf("SUCCESS: Cl Image depth = %lu\n",(unsigned long)sizet_units);
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			}
			/*	
				{
				return_type cl_image_format;
				case IMAGE_FORMAT:
				}
				{
				return_type	ID3D10Resource *;		
				case CL_IMAGE_D3D10_SUBRESOURCE_KHR:
				}
			 */
			default:
			char Image_string[1024];
			m_error_state = FAILURE;
			printf("ERROR: UNSUPPORTED Image Info type = %d ",param_name);
			strcpy(Image_string,"UNSUPPORTED IMAGE INFO TYPE");
			return WebCLGetInfo();
		}
		m_error_state = FAILURE;
		return WebCLGetInfo();
	}

	long WebCLComputeContext::getError()
	{
		//printf("WebCLComputeContext::getError\n");
		return m_error_state;
	}

	// Present in OpenCL 1.1	
	/*
	   PassRefPtr<WebCLEvent> WebCLComputeContext::createUserEvent(WebCLContext* context_id)
	   {
	   printf("WebCLComputeContext::createUserEvent\n");

	   cl_int err = 0;
	   cl_context cl_context_id = NULL;

	   if (context_id != NULL) {
	   cl_context_id = context_id->getCLContext();
	   if (cl_context_id == NULL) {
	   printf("Error: cl_context_id null\n");
	   return NULL;
	   }
	   }

	   cl_event event = clCreateUserEvent(cl_context_id, &err);

	   if (err != CL_SUCCESS) {
	   printf("Error: clCreateUserEvent\n");
	   switch (err) {
	   case CL_INVALID_CONTEXT :
	   printf("CL_INVALID_CONTEXT \n");
	   break;
	   case CL_OUT_OF_RESOURCES :
	   printf("CL_OUT_OF_RESOURCES \n");
	   break;
	   case CL_OUT_OF_HOST_MEMORY :
	   printf("CL_OUT_OF_HOST_MEMORY \n");
	   break;
	   default:
	   printf("what else?\n");
	   break;
	   }
	   m_error_state = FAILURE;
	   return NULL;
	   } else {
	   printf("Success: clCreateUserEvent\n");
	   RefPtr<WebCLEvent> o = WebCLEvent::create(this, event);
	   m_event_list.append(o);
	   m_num_events++;
	   printf("m_num_events=%ld\n", m_num_events);
	   m_error_state = SUCCESS;
	   return o;
	   }
	   return NULL;
	   }
	 */

	PassRefPtr<WebCLContext> WebCLComputeContext::createContext(int contextProperties, 
			WebCLDeviceIDList* devices, int pfn_notify, int user_data)
	{
		printf("WebCLComputeContext::createContext prop=%d pfn_notify=%d user_data=%d\n", 
				contextProperties, pfn_notify, user_data);

		cl_int err = 0;
		cl_context cl_context_id = 0;
		cl_device_id cl_device;

		if (devices != NULL) {
			cl_device = devices->getCLDeviceIDs();
			if (cl_device == NULL) {
				printf("Error: devices null\n");
				return NULL;
			}
		} else {
			printf("Error: webcl_devices null\n");
			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = clCreateContext(NULL, 1, &cl_device, NULL, NULL, &err);

		if (err != CL_SUCCESS) {
			printf("Error: clCreateContext\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateContext\n");
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_error_state = SUCCESS;
				m_context = o;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLContext>WebCLComputeContext::createSharedContext(int device_type, 
			int pfn_notify, int user_data)
	{
		printf("WebCLComputeContext::createSharedContext device_type=%d pfn_notify=%d user_data=%d\n",
				device_type, pfn_notify, user_data);

		cl_int err = 0;
		cl_context cl_context_id = NULL;

		CGLContextObj kCGLContext = CGLGetCurrentContext();
		CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);

		cl_context_properties properties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)kCGLShareGroup, 0
		};
		cl_context_id = clCreateContext(properties, 0, 0, 0, 0, &err);

		if (err != CL_SUCCESS) {
			printf("Error: clCreateContext\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateContext\n");
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_error_state = SUCCESS;
				m_context = o;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}


	PassRefPtr<WebCLContext> WebCLComputeContext::createContext(int contextProperties, 
			WebCLDeviceID* device, int pfn_notify, int user_data)
	{
		printf("WebCLComputeContext::createContext prop=%d pfn_notify=%d user_data=%d\n", 
				contextProperties, pfn_notify, user_data);

		cl_int err = 0;
		cl_context cl_context_id;
		cl_device_id cl_device;

		if (device != NULL) {
			cl_device = device->getCLDeviceID();
			if (cl_device == NULL) {
				printf("Error: devices null\n");
				return NULL;
			}
			m_device_id_ = device;
		} else {
			printf("Error: webcl_devices null\n");
			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = clCreateContext(NULL, 1, &cl_device, NULL, NULL, &err);
		if (err != CL_SUCCESS) {
			printf("Error: clCreateContext\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateContext\n");
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_error_state = SUCCESS;
				m_context = o;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLCommandQueue> WebCLComputeContext::createCommandQueue(WebCLContext* context_id, 
			WebCLDeviceIDList* devices, int prop)
	{
		printf("WebCLComputeContext::createCommandQueue prop=%d\n", prop);

		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_device_id cl_device = NULL;
		cl_command_queue cl_command_queue_id;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				return NULL;
			}
		}
		if (devices != NULL) {
			cl_device = devices->getCLDeviceIDs();
			if (cl_device == NULL) {
				printf("Error: cl_device null\n");
				return NULL;
			}
		}

		// TODO(won.jeon) - prop needs to be addressed later
		cl_command_queue_id = clCreateCommandQueue(cl_context_id, cl_device, 0, &err);
		if (err != CL_SUCCESS) {
			printf("Error: clCreateCommandQueue\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateCommandQueue\n");
			RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(this, cl_command_queue_id);
			if (o != NULL) {
				m_error_state = SUCCESS;
				m_command_queue = o;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLCommandQueue> WebCLComputeContext::createCommandQueue(WebCLContext* context_id, 
			WebCLDeviceID* device, int prop)
	{
		printf("WebCLComputeContext::createCommandQueue prop=%d\n", prop);

		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_device_id cl_device = NULL;
		cl_command_queue cl_command_queue_id;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				return NULL;
			}
		}
		if (device != NULL) {
			cl_device = device->getCLDeviceID();
			if (cl_device == NULL) {
				printf("Error: cl_device null\n");
				return NULL;
			}
		}

		// TODO(won.jeon) - prop needs to be addressed later
		cl_command_queue_id = clCreateCommandQueue(cl_context_id, cl_device, 0, &err);
		if (err != CL_SUCCESS) {
			printf("Error: clCreateCommandQueue\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateCommandQueue\n");
			RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(this, cl_command_queue_id);
			if (o != NULL) {
				m_error_state = SUCCESS;
				m_command_queue = o;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLProgram> WebCLComputeContext::createProgramWithSource(WebCLContext* context_id, 
			const String& kernelSource)
	{
		printf("WebCLComputeContext::createProgramWithSource length=%d\n", 
				kernelSource.length());

		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_program cl_program_id;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				return NULL;
			}
		}

		const char* source = strdup(kernelSource.utf8().data());
		printf("source %s\n", source);
		cl_program_id = clCreateProgramWithSource(cl_context_id, 1, (const char**)&source, 
				NULL, &err);
		// TODO(won.jeon) - the second and fourth arguments need to be addressed later
		if (err != CL_SUCCESS) {
			printf("Error: clCreateProgramWithSource\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateProgramWithSource\n");
			RefPtr<WebCLProgram> o = WebCLProgram::create(this, cl_program_id);
			if (o != NULL) {
				//m_program = o;
				m_program_list.append(o);
				m_num_programs++;
				printf("m_num_programs=%ld\n", m_num_programs);
				m_error_state = SUCCESS;
				return o;
			} else {
				m_error_state = FAILURE;
				return NULL;
			}
		}
		return NULL;
	}

	long WebCLComputeContext::buildProgram(WebCLProgram* program, int options, 
			int pfn_notify, int user_data)
	{
		printf("WebCLComputeContext::buildProgram options=%d pfn_notify=%d user_data=%d\n", 
				options, pfn_notify, user_data);
		cl_int err = 0;
		cl_program program_id = NULL;

		if (program != NULL) {
			program_id = program->getCLProgram();
			if (program_id == NULL) {
				printf("Error: program_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}

		// TODO(won.jeon) - needs to be addressed later
		err = clBuildProgram(program_id, 0, NULL, NULL, NULL, NULL);

		if (err != CL_SUCCESS) {
			printf("Error: clBuildProgram\n");

			m_error_state = FAILURE;

			switch (err) {
				case CL_INVALID_PROGRAM:
					printf("CL_INVALID_PROGRAM\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_DEVICE:
					printf("CL_INVALID_DEVICE\n");
					break;
				case CL_INVALID_BINARY:
					printf("CL_INVALID_BINARY\n");
					break;
				case CL_INVALID_OPERATION:
					printf("CL_INVALID_OPERATION\n");
					break;
				case CL_COMPILER_NOT_AVAILABLE:
					printf("CL_COMPILER_NOT_AVAILABLE\n");
					break;
				case CL_BUILD_PROGRAM_FAILURE:
					printf("CL_BUILD_PROGRAM_FAILURE\n");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("CL_OUT_OF_RESOURCES\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
			}

			return m_error_state;
		} else {
			printf("Success: clBuildProgram\n");
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return m_error_state;
	}

	String WebCLComputeContext::getProgramBuildInfo(WebCLProgram* program, WebCLDeviceIDList* device, 
			int param_name)
	{
		printf("WebCLComputeContext::getProgramBuildInfo param_name=%d\n", param_name);

		cl_program program_id = NULL;
		cl_device_id device_id = NULL;
		cl_uint err = 0;

		if (program != NULL) {
			program_id = program->getCLProgram();
			if (program_id == NULL) {
				printf("Error: program_id null\n");
				m_error_state = FAILURE;
			}
		}
		if (device != NULL) {
			device_id = device->getCLDeviceIDs();
			if (device_id == NULL) {
				printf("Error: device_id null\n");
				m_error_state = FAILURE;
			}
		}

		size_t len;
		char buffer[2048];
		switch (param_name) {
			case PROGRAM_BUILD_LOG:
				err = clGetProgramBuildInfo(program_id, device_id, CL_PROGRAM_BUILD_LOG, 
						sizeof(buffer), buffer, &len);
				switch (err)
				{
					case CL_SUCCESS:
						printf("clGetProgramBuildInfo CL_SUCCESS\n");
						printf("buffer %s\n", buffer);
						return String(buffer);
						break;
					case CL_INVALID_DEVICE:
						printf("clGetProgramBuildInfo CL_INVALID_DEVICE\n");
						break;
					case CL_INVALID_VALUE:
						printf("clGetProgramBuildInfo CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_PROGRAM:
						printf("clGetProgramBuildInfo CL_INVALID_PROGRAM\n");
						break;
				}
		}
		return String();

	}

	PassRefPtr<WebCLKernel> WebCLComputeContext::createKernel(WebCLProgram* program, 
			const String& kernel_name)
	{
		printf("WebCLComputeContext::createKernel\n");

		cl_int err = 0;
		cl_program cl_program_id = NULL;
		cl_kernel cl_kernel_id;

		if (program != NULL) {
			cl_program_id = program->getCLProgram();
			if (cl_program_id == NULL) {
				printf("Error: cl_program_id null\n");
				return NULL;
			}
		}

		const char* kernel_name_str = strdup(kernel_name.utf8().data());
		cl_kernel_id = clCreateKernel(cl_program_id, kernel_name_str, &err);
		// TODO(won.jeon) - more detailed error code need to be addressed later
		if (err != CL_SUCCESS) {
			printf("Error: clCreateKernel\n");
			switch (err) {
				case CL_INVALID_PROGRAM:
					printf("CL_INVALID_PROGRAM\n");
					break;
				case CL_INVALID_PROGRAM_EXECUTABLE:
					printf("CL_INVALID_PROGRAM_EXECUTABLE\n");
					break;
				case CL_INVALID_KERNEL_NAME:
					printf("CL_INVALID_KERNEL_NAME\n");
					break;
				case CL_INVALID_KERNEL_DEFINITION:
					printf("CL_INVALID_KERNEL_DEFINITION\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("CL_OUT_OF_RESOURCES\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clCreateKernel\n");
			RefPtr<WebCLKernel> o = WebCLKernel::create(this, cl_kernel_id);
			m_kernel_list.append(o);
			m_num_kernels++;
			printf("m_num_kernels=%ld\n", m_num_kernels);
			m_error_state = SUCCESS;

			return o;
		}
		return NULL;
	}

	PassRefPtr<WebCLMem> WebCLComputeContext::createBuffer(WebCLContext* context_id, 
			int flags, int size, int host_ptr)
	{
		printf("WebCLComputeContext::createBuffer size=%d host_ptr=%d\n", size, host_ptr);

		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_mem cl_mem_id = NULL;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				return NULL;
			}
		}
		// TODO(won.jeon) - NULL parameter needs to be addressed later
		switch (flags)
		{
			case MEM_READ_ONLY:
				cl_mem_id = clCreateBuffer(cl_context_id, CL_MEM_READ_ONLY, size, NULL, &err);
				break;
			case MEM_WRITE_ONLY:
				cl_mem_id = clCreateBuffer(cl_context_id, CL_MEM_WRITE_ONLY, size, NULL, &err);
				break;
			case MEM_READ_WRITE:
				cl_mem_id = clCreateBuffer(cl_context_id, CL_MEM_READ_WRITE, size, NULL, &err);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clCreateBuffer\n");
			switch(err) {
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_BUFFER_SIZE:
					printf("CL_INVALID_BUFFER_SIZE\n");
					break;
				case CL_INVALID_HOST_PTR:
					printf("CL_INVALID_HOST_PTR\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;
			return NULL;

		} else {
			printf("Success: clCreateBuffer\n");
			RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_id, false);
			m_mem_list.append(o);
			m_num_mems++;
			printf("m_num_mems=%ld\n", m_num_mems);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}

	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueWriteBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
			Float32Array* ptr, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}
		}
		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_write) {
			case true:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, offset, 
						buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, offset, 
						buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueWriteBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueWriteBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}

#if 0
	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueWriteBuffer(WebCLCommandQueue* command_queue,
			WebCLMem* mem, bool blocking_write, int offset, int buffer_size,
			ImageData* ptr, int event_wait_list)
	{
		printf("WebCLComputeContext::enqueueWriteBuffer(ImageData) offset=%d buffer_size=%d event_wait_list=%d\n",
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			//cl_mem cl_mem_id = (m_mem_list[0].get())->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}
		}

		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_write) {
			case true:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
						offset, buffer_size, ptr->data()->data()->data(), event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, ptr->data()->data()->data(), event_wait_list, NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueWriteBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueWriteBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}
#endif

	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueWriteBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
			Int32Array* ptr, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}


		}

		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_write) {
			case true:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
						offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, ptr->data(), event_wait_list ,NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueWriteBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueWriteBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}

	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueWriteBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_write, int offset, int buffer_size, 
			Uint8Array* ptr, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueWriteBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}


		}

		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_write) {
			case true:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
						offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueWriteBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, ptr->data(),event_wait_list, NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueWriteBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueWriteBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}


	PassRefPtr<WebCLEvent>  WebCLComputeContext::enqueueReadBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
			Float32Array* ptr, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueReadBuffer(Float32Array) offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}


		}
		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_read) {
			case true:
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
						offset, buffer_size, ptr->data(),event_wait_list, NULL,&cl_event_id);
				break;
			case false:
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, ptr->data(),event_wait_list, NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueReadBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueReadBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}

#if 0
	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueReadBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
			HTMLCanvasElement* canvas_element, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueReadBuffer(HTMLCanvasElement) offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;
		CanvasRenderingContext2D* rendering_context = NULL;
		RefPtr<ImageData> image_data = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}


		}

		if (canvas_element != NULL) {
			//graphics_context = canvas_element->drawingContext();
			rendering_context = static_cast<CanvasRenderingContext2D*>(canvas_element->renderingContext());

		} else {
			printf("canvas_element == NULL\n");
			return NULL;
		}

		unsigned char* tmp = (unsigned char*)malloc(buffer_size*sizeof(unsigned char));
		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_read) {
			case true:
				//err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
				//		offset, buffer_size, image_data->data()->data()->data(), 0, NULL, NULL);
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE,
						offset, buffer_size, tmp,event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, image_data->data()->data()->data(),event_wait_list, NULL, &cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueReadBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("CL_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("Success: clEnqueueReadBuffer\n");

			for (int i = 0; i < buffer_size; i++) {
				//printf("%u ", image_data->data()->data()->get(i));
				printf("%u ", tmp[i]);
				if ((i+1) % 4 == 0) {
					printf("\n");
				}
			}
			printf("\n");


			//ExceptionCode ec = 0;
			RefPtr<ByteArray> array = ByteArray::create(buffer_size);
			for (int i = 0; i < buffer_size; i++) {
				//array->set(i, (unsigned char)tmp[i]);
				array->set(i, (unsigned char)128);
				if ((i+1) % 4 == 0)
					array->set(i, (unsigned char)127);
			}
			image_data = ImageData::create(IntSize(canvas_element->width(), canvas_element->height()), array);	
			//rendering_context->drawImage(image_data.get(), 0.0, 0.0, ec);
			//rendering_context->paintRenderingResultsToCanvas();
			//GraphicsContext* graphics_context = canvas_element->drawingContext();
			printf("Success: clEnqueueReadBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}
#endif

	PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueReadBuffer(WebCLCommandQueue* command_queue, 
			WebCLMem* mem, bool blocking_read, int offset, int buffer_size, 
			Int32Array* ptr, int event_wait_list)
	{
		// TODO(won.jeon) - return type temporarily set to void (instead of WebCLEvent)
		printf("WebCLComputeContext::enqueueReadBuffer offset=%d buffer_size=%d event_wait_list=%d\n", 
				offset, buffer_size, event_wait_list);

		cl_command_queue cl_command_queue_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		cl_event cl_event_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return NULL;
			}
		}
		if (mem != NULL) {
			cl_mem_id = mem->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				return NULL;
			}


		}
		// TODO(won.jeon) - NULL parameters need to be addressed later
		switch(blocking_read) {
			case true:
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_TRUE, 
						offset, buffer_size, ptr->data(), event_wait_list, NULL, &cl_event_id);
				break;
			case false:
				err = clEnqueueReadBuffer(cl_command_queue_id, cl_mem_id, CL_FALSE, 
						offset, buffer_size, ptr->data(), event_wait_list, NULL,&cl_event_id);
				break;
		}
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueReadBuffer\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_MEM_OBJECT:
					printf("C_INVALID_MEM_OBJECT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			m_error_state = FAILURE;

			return NULL;
		} else {
			printf("ptr->data() [%d][%d][%d][%d][%d]\n", (ptr->data())[0], (ptr->data())[1], 
					(ptr->data())[2], (ptr->data())[3], (ptr->data())[4]);
			printf("Success: clEnqueueReadBuffer\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;
	}

	long WebCLComputeContext::setKernelArg(WebCLKernel* kernel, unsigned int arg_index, int arg_value)
	{
		printf("WebCLComputeContext::setKernelArg for int\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clSetKernelArg(cl_kernel_id, arg_index, sizeof(cl_int), &arg_value);
		if (err != CL_SUCCESS) {
			printf("Error: clSetKernelArg\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clSetKernelArg\n");
			m_error_state = SUCCESS;
			return m_error_state;
		}
	}

	long WebCLComputeContext::setKernelArgFloat(WebCLKernel* kernel, unsigned int arg_index, float arg_value)
	{
		printf("WebCLComputeContext::setKernelArg for float\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clSetKernelArg(cl_kernel_id, arg_index, sizeof(cl_float), &arg_value);
		if (err != CL_SUCCESS) {
			printf("Error: clSetKernelArg\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clSetKErnelArg\n");
			m_error_state = SUCCESS;
			return m_error_state;
		}
	}
	//TODO (siba samal) Change back after Object support	
	//long WebCLComputeContext::setKernelArgGlobal(WebCLKernel* kernel, unsigned int arg_index, WebCLMem* arg_value)
	long WebCLComputeContext::setKernelArg(WebCLKernel* kernel, unsigned int arg_index, WebCLMem* arg_value)
	{
		printf("WebCLComputeContext::setKernelArgGlobal\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;
		cl_mem cl_mem_id = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		if (arg_value != NULL) {
			cl_mem_id = arg_value->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clSetKernelArg(cl_kernel_id, arg_index, sizeof(cl_mem), &cl_mem_id);
		if (err != CL_SUCCESS) {
			printf("Error: clSetKernelArg\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clSetKernelArg\n");
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}

	long WebCLComputeContext::setKernelArgConstant(WebCLKernel* kernel, unsigned int arg_index, WebCLMem* arg_value)
	{
		printf("WebCLComputeContext::setKernelArgConstant\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;
		cl_mem cl_mem_id = NULL;
		cl_device_id cl_device = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		if (arg_value != NULL) {
			cl_mem_id = arg_value->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		if (m_device_id_ != NULL) {
			cl_device = m_device_id_->getCLDeviceID();
			cl_ulong max_buffer_size = 0;
			clGetDeviceInfo(cl_device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &max_buffer_size, NULL);
			cl_uint max_args = 0;
			clGetDeviceInfo(cl_device, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &max_args,NULL);
			// Check for __constant qualifier restrictions
			if ( (sizeof(cl_mem) <= max_buffer_size)  && (arg_index <= max_args))
			{
				err = clSetKernelArg(cl_kernel_id, arg_index, sizeof(cl_mem), &cl_mem_id);
				if (err != CL_SUCCESS) {
					printf("Error: clSetKernelArg\n");
					m_error_state = FAILURE;
					return m_error_state;
				} else {
					printf("Success: clSetKernelArg\n");
					m_error_state = SUCCESS;
					return m_error_state;
				}
			}
		}
		return FAILURE;
	}
	long WebCLComputeContext::setKernelArgLocal(WebCLKernel* kernel, unsigned int arg_index, 
			unsigned int arg_size)
	{
		//printf("WebCLComputeContext::setKernelArg for __local arg_value=%d\n", arg_value);

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}

		err = clSetKernelArg(cl_kernel_id, arg_index, arg_size, NULL);
		if (err != CL_SUCCESS) {
			printf("Error: clSetKernelArg\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clSetKernelArg\n");
			m_error_state = SUCCESS;
			return m_error_state;
		}
	}


	unsigned long WebCLComputeContext::getKernelWorkGroupInfo(WebCLKernel* kernel, WebCLDeviceIDList* devices, int param_name)
	{
		printf("WebCLComputeContext::getKernelWorkGroupInfo\n");

		cl_int err = 0;
		cl_kernel cl_kernel_id = NULL;
		cl_device_id cl_device = NULL;
		size_t ret;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				return NULL;
			}
		}
		if (devices != NULL) {
			cl_device = devices->getCLDeviceIDs();
			if (cl_device == NULL) {
				printf("Error: cl_device null\n");
				return NULL;
			}
		}
		switch (param_name) {
			case KERNEL_WORK_GROUP_SIZE:
				err = clGetKernelWorkGroupInfo(cl_kernel_id, cl_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(ret), &ret, NULL);
				printf("sizeof(ret)=%ld ret=%ld\n", sizeof(ret), ret);
				if (err != CL_SUCCESS) {
					printf("Error: clGetKerelWorkGroupInfo\n");
					switch (err) {
						case CL_INVALID_DEVICE:
							printf("CL_INVALID_DEVICE\n");
							break;
						case CL_INVALID_VALUE:
							printf("CL_INVALID_VALUE\n");
							break;
						case CL_INVALID_KERNEL:
							printf("CL_INVALID_KERNEL\n");
							break;
						default:
							printf("What else?\n");
							break;
					}
					m_error_state = FAILURE;
					return NULL;
				} else {
					printf("Success: clGetKernelWorkGroupInfo\n");
					m_error_state = SUCCESS;

					return ret;
				}
				break;
			default:
				printf("what else?\n");
				break;
		}
		return NULL;
	}
#if 0
	// NOTE(won.jeon)
	// it only works with integer-type of global/local workgroup size
	// it does not work with vector-type of global/local workgroup size
	// it will be deprecated
	PassRefPtr<WebCLEvent>  WebCLComputeContext::enqueueNDRangeKernel(WebCLCommandQueue* command_queue, 
			WebCLKernel* kernel, unsigned int work_dim, unsigned int global_work_offset, 
			unsigned int global_work_size, unsigned int local_work_size, int event_wait_list)
	{
		printf("WebCLComputeContext::enqueueNDRangeKernel(int) event_wait_list=%d\n", event_wait_list);

		cl_int err = 0;
		cl_command_queue cl_command_queue_id = NULL;
		cl_kernel cl_kernel_id = NULL;
		cl_event cl_event_id = NULL;

		size_t g_work_offset = global_work_offset;
		size_t g_work_size = global_work_size;
		size_t l_work_size = local_work_size;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_comamnd_queue_id null\n");
				return NULL;
			}
		}
		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				return NULL;
			}
		}

		err = clEnqueueNDRangeKernel(cl_command_queue_id, cl_kernel_id, work_dim, 
				&g_work_offset, &g_work_size, &l_work_size, event_wait_list, NULL, &cl_event_id);
		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueNDRangeKernel\n");
			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clEnqueueNDRangeKernel\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;
		}
		return NULL;	

	}
#endif

	PassRefPtr<WebCLEvent>  WebCLComputeContext::enqueueNDRangeKernel(WebCLCommandQueue* command_queue, 
			WebCLKernel* kernel, unsigned int work_dim, unsigned int global_work_offset, 
			Int32Array* global_work_size, Int32Array* local_work_size, int event_wait_list)
	{
		printf("WebCLComputeContext::enqueueNDRangeKernel(Int32Array) event_wait_list=%d\n", event_wait_list);

		cl_int err = 0;
		cl_command_queue cl_command_queue_id = NULL;
		cl_kernel cl_kernel_id = NULL;
		cl_event cl_event_id = NULL;

		size_t g_work_offset = global_work_offset;

		size_t *g_work_size = NULL;
		size_t *l_work_size = NULL;
		//size_t g1_work_size;
		//size_t l1_work_size;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_comamnd_queue_id null\n");
				return NULL;
			}
		}
		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				return NULL;
			}
		}

		g_work_size = (size_t*)malloc(global_work_size->length() * sizeof(size_t));
		for (unsigned int i = 0; i < global_work_size->length(); i++) {
			g_work_size[i] = global_work_size->item(i);
		}
		l_work_size = (size_t*)malloc(local_work_size->length() * sizeof(size_t));
		for (unsigned int i = 0; i < local_work_size->length(); i++) {
			l_work_size[i] = local_work_size->item(i);
		}

		err = clEnqueueNDRangeKernel(cl_command_queue_id, cl_kernel_id, work_dim, 
				&g_work_offset, g_work_size, l_work_size, event_wait_list, NULL,  &cl_event_id);

		if (err != CL_SUCCESS) {
			printf("Error: clEnqueueNDRangeKernel\n");

			switch(err) {
				case CL_INVALID_PROGRAM_EXECUTABLE:
					printf("CL_INVALID_PROGRAM_EXECUTABLE\n");
					break;
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMMAND_QUEUE\n");
					break;
				case CL_INVALID_KERNEL:
					printf("CL_INVALID_KERNEL\n");
					break;
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_KERNEL_ARGS:
					printf("CL_INVALID_KERNEL_ARGS\n");
					break;
				case CL_INVALID_WORK_DIMENSION:
					printf("CL_INVALID_WORK_DIMENSION\n");
					break;
					//case CL_INVALID_GLOBAL_WORK_SIZE:
					//	printf("CL_INVALID_GLOBAL_WORK_SIZE\n");
					//	break;
				case CL_INVALID_GLOBAL_OFFSET:
					printf("CL_INVALID_GLOBAL_OFFSET\n");
					break;
				case CL_INVALID_WORK_GROUP_SIZE:
					printf("CL_INVALID_WORK_GROUP_SIZE\n");
					break;
					/* only for spec 1.1?
					   case CL_MISALIGNED_SUB_BUFFER_OFFSET:
					   printf("CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
					   break;
					 */
				case CL_INVALID_IMAGE_SIZE:
					printf("CL_INVALID_IMAGE_SIZE\n");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("CL_OUT_OF_RESOURCES\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_INVALID_EVENT_WAIT_LIST:
					printf("CL_INVALID_EVENT_WAIT_LIST\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}

			m_error_state = FAILURE;
			return NULL;
		} else {
			printf("Success: clEnqueueNDRangeKernel\n");
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			printf("m_num_events=%ld\n", m_num_events);
			m_error_state = SUCCESS;
			return o;

		}
		return NULL;	

	}

#if 0
	long WebCLComputeContext::finish(WebCLCommandQueue* command_queue, int data, int cb)
	{
		printf("WebCLComputeContext::finish data=%d cb=%d\n", data, cb);

		cl_int err = 0;
		cl_command_queue cl_command_queue_id = NULL;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				return FAILURE;
			}
		}

		err = clFinish(cl_command_queue_id);
		if (err != CL_SUCCESS) {
			printf("Error: clFinish\n");
			m_error_state = FAILURE;
		} else {
			printf("Success: clFinish\n");
			m_error_state = SUCCESS;

		}
		return FAILURE;
	}
#endif
	// TODO(siba.samal) Need to change user_data to Object type
	void WebCLComputeContext::finish(WebCLCommandQueue* command_queue, PassRefPtr<WebCLFinishCallback> notify, int user_data/*object userData*/)
	{
		printf("WebCLComputeContext::finish user_data=%d\n", user_data);
		cl_command_queue cl_command_queue_id = NULL;
		cl_int err = 0;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				m_error_state = FAILURE;
				return;
			}
		}
		err = clFinish(cl_command_queue_id);
		printf("cl_finish Call is Over!!\n");
		if (err != CL_SUCCESS) {
			printf("Error: finish\n");
			switch (err) {
				case CL_INVALID_COMMAND_QUEUE:
					printf("CL_INVALID_COMAND_QUEUE\n");
					break;
				case CL_OUT_OF_HOST_MEMORY :
					printf("CL_OUT_OF_HOST_MEMORY \n");
					break;				
				default:
					printf("what else?\n");
					break;
			}
		} else {
			printf("Success: clFinish\n");
			m_error_state = SUCCESS;
			// Call the Callback
			m_finishCallback = notify;
			//m_finishCallback->handleEvent(user_data);			
			printf("Calling the callback !!\n");
			m_finishCallback->handleEvent(this);			
			return;
		}
		m_error_state = FAILURE;
		return;
	}

	long WebCLComputeContext::releaseMemObject(WebCLMem* memobj)
	{
		printf("WebCLComputecontext::releaseMemObject\n");
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;

		if (memobj != NULL) {
			cl_mem_id = memobj->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseMemObject(cl_mem_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseMemObject\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseMemObject\n");

			unsigned int i;

			for (i = 0; i < m_mem_list.size(); i++) {
				if ((m_mem_list[i].get())->getCLMem() == cl_mem_id) {
					printf("found cl_mem_id\n");
					m_mem_list.remove(i);
					m_num_mems = m_mem_list.size();
					break;
				}
			}
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}

	long WebCLComputeContext::releaseProgram(WebCLProgram* program)
	{
		printf("WebCLComputeContext::releaseProgram\n");
		cl_program cl_program_id = NULL;
		cl_int err = 0;

		if (program != NULL) {
			cl_program_id = program->getCLProgram();
			if (cl_program_id == NULL) {
				printf("Error: cl_program_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseProgram(cl_program_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseProgram\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseProgram\n");

			int i;
			for (i = 0; i < m_num_programs; i++) {
				if ((m_program_list[i].get())->getCLProgram() == cl_program_id) {
					printf("found cl_program_id\n");
					m_program_list.remove(i);
					m_num_programs = m_program_list.size();
					break;
				}
			}
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}

	long WebCLComputeContext::releaseKernel(WebCLKernel* kernel)
	{
		printf("WebCLComputeContext::releaseKernel\n");
		cl_kernel cl_kernel_id = NULL;
		cl_int err = 0;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseKernel(cl_kernel_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReaseKernel\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseKernel\n");

			int i;
			for (i = 0; i < m_num_kernels; i++) {
				if ((m_kernel_list[i].get())->getCLKernel() == cl_kernel_id) {
					m_kernel_list.remove(i);
					m_num_kernels = m_kernel_list.size();
					break;
				}
			}
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainKernel(WebCLKernel* kernel)
	{
		printf("WebCLComputeContext::retainKernel\n");
		cl_kernel cl_kernel_id = NULL;
		cl_int err = 0;

		if (kernel != NULL) {
			cl_kernel_id = kernel->getCLKernel();
			if (cl_kernel_id == NULL) {
				printf("Error: cl_kernel_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainKernel(cl_kernel_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainKernel\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainKernel\n");
			// TODO - Check if has to be really added
			RefPtr<WebCLKernel> o = WebCLKernel::create(this, cl_kernel_id);
			m_kernel_list.append(o);
			m_num_kernels++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainProgram(WebCLProgram* program)
	{
		printf("WebCLComputeContext::retainProgram\n");
		cl_program cl_program_id = NULL;
		cl_int err = 0;

		if (program != NULL) {
			cl_program_id = program->getCLProgram();
			if (cl_program_id == NULL) {
				printf("Error: cl_program_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainProgram(cl_program_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainProgram\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {

			printf("Success: clRetainProgram\n");
			// TODO - Check if has to be really added
			RefPtr<WebCLProgram> o = WebCLProgram::create(this, cl_program_id);
			m_program_list.append(o);
			m_num_programs++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainEvent(WebCLEvent* event)
	{
		printf("WebCLComputeContext::retainEvent\n");
		cl_event cl_event_id = NULL;
		cl_int err = 0;

		if (event != NULL) {
			cl_event_id = event->getCLEvent();
			if (cl_event_id == NULL) {
				printf("Error: cl_event_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainEvent(cl_event_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainEvent\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainEvent\n");
			// TODO - Check if has to be really added
			RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
			m_event_list.append(o);
			m_num_events++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainContext(WebCLContext* context_id)
	{
		printf("WebCLComputeContext::retainContext\n");
		cl_context cl_context_id = NULL;
		cl_int err = 0;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainContext(cl_context_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainContext\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainContext\n");
			// TODO - Check if has to be really added
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			m_context_list.append(o);
			m_num_contexts++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainCommandQueue(WebCLCommandQueue* command_queue)
	{
		printf("WebCLComputeContext::retainCommandQueue\n");
		cl_command_queue cl_command_queue_id = NULL;
		cl_int err = 0;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainCommandQueue(cl_command_queue_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainCommandQueue\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainCommandQueue\n");
			// TODO - Check if has to be really added
			RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(this, cl_command_queue_id);
			m_commandqueue_list.append(o);
			m_num_commandqueues++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainSampler(WebCLSampler* sampler)
	{
		printf("WebCLComputeContext::retainSampler\n");
		cl_sampler cl_sampler_id= NULL;
		cl_int err = 0;

		if (sampler != NULL) {
			cl_sampler_id = sampler->getCLSampler();
			if (cl_sampler_id == NULL) {
				printf("Error: cl_sampler_id null\n");
				m_error_state = FAILURE;
				return m_error_state;


			}
		}
		err = clRetainSampler(cl_sampler_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainSampler\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainSampler\n");

			// TODO - Check if has to be really added
			RefPtr<WebCLSampler> o = WebCLSampler::create(this, cl_sampler_id,false);
			m_sampler_list.append(o);
			m_num_samplers++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::retainMemObject(WebCLMem* memobj)
	{
		printf("WebCLComputecontext::retainMemObject\n");
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;

		if (memobj != NULL) {
			cl_mem_id = memobj->getCLMem();
			if (cl_mem_id == NULL) {
				printf("Error: cl_mem_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clRetainMemObject(cl_mem_id);
		if (err != CL_SUCCESS) {
			printf("Error: clRetainMemObject\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clRetainMemObject\n");

			// TODO - Check if has to be really added
			RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_id,false);
			m_mem_list.append(o);
			m_num_mems++;
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}



	//sujit added 23-6-2011
	long WebCLComputeContext::releaseEvent(WebCLEvent* event)
	{
		printf("WebCLComputeContext::releaseEvent\n");
		cl_event cl_event_id = NULL;
		cl_int err = 0;

		if (event != NULL) {
			cl_event_id = event->getCLEvent();
			if (cl_event_id == NULL) {
				printf("Error: cl_event_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseEvent(cl_event_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseEvent\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseEvent\n");

			int i;
			for (i = 0; i < m_num_events; i++) {
				if ((m_event_list[i].get())->getCLEvent() == cl_event_id) {
					m_event_list.remove(i);
					m_num_events = m_event_list.size();
					break;
				}
			}
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}
	//sujit added 23-6-2011
	long WebCLComputeContext::releaseSampler(WebCLSampler* sampler)
	{
		printf("WebCLComputeContext::releaseSampler\n");
		cl_sampler cl_sampler_id= NULL;
		cl_int err = 0;

		if (sampler != NULL) {
			cl_sampler_id = sampler->getCLSampler();
			if (cl_sampler_id == NULL) {
				printf("Error: cl_sampler_id null\n");
				m_error_state = FAILURE;
				return m_error_state;


			}
		}
		err = clReleaseSampler(cl_sampler_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseSampler\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseSamplers\n");

			int i;
			for (i = 0; i < m_num_samplers; i++) {
				if ((m_sampler_list[i].get())->getCLSampler() == cl_sampler_id) {
					m_sampler_list.remove(i);
					m_num_samplers = m_sampler_list.size();
					break;
				}
			}
			m_error_state = SUCCESS;
			return m_error_state;
		}
		return FAILURE;
	}


	long WebCLComputeContext::releaseCommandQueue(WebCLCommandQueue* command_queue)
	{
		printf("WebCLComputeContext::releaseCommandQueue\n");
		cl_command_queue cl_command_queue_id = NULL;
		cl_int err = 0;

		if (command_queue != NULL) {
			cl_command_queue_id = command_queue->getCLCommandQueue();
			if (cl_command_queue_id == NULL) {
				printf("Error: cl_command_queue_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseCommandQueue(cl_command_queue_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseCommandQueue\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseCommandQueue\n");
			m_error_state = SUCCESS;
			m_command_queue = NULL;
			return m_error_state;
		}
		return FAILURE;
	}

	long WebCLComputeContext::releaseContext(WebCLContext* context_id)
	{
		printf("WebCLComputeContext::releaseContext\n");
		cl_context cl_context_id = NULL;
		cl_int err = 0;

		if (context_id != NULL) {
			cl_context_id = context_id->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				m_error_state = FAILURE;
				return m_error_state;
			}
		}
		err = clReleaseContext(cl_context_id);
		if (err != CL_SUCCESS) {
			printf("Error: clReleaseContext\n");
			m_error_state = FAILURE;
			return m_error_state;
		} else {
			printf("Success: clReleaseContext\n");
			m_error_state = SUCCESS;
			m_context = NULL;
			return m_error_state;
		}
		return FAILURE;
	}

	PassRefPtr<WebCLMem> WebCLComputeContext::createImage2D(WebCLContext* context, 
			int flags, 
			HTMLCanvasElement* canvasElement)
	{
		printf("WebCLComputeContext::createImage2D canvas element\n");

		cl_context cl_context_id = NULL;
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format = {CL_RGBA, CL_UNSIGNED_INT32};
		cl_uint width = 0;
		cl_uint height = 0;

		ImageBuffer* imageBuffer = NULL;
		PassRefPtr<ByteArray> bytearray = NULL;
		if (context != NULL) {
			cl_context_id = context->getCLContext();
			if (cl_context_id == NULL) {
				printf("Error: cl_context_id null\n");
				m_error_state = FAILURE;
				return NULL;
			}
		}

		if (canvasElement != NULL) {
			image_format.image_channel_data_type = CL_UNSIGNED_INT8;
			image_format.image_channel_order = CL_RGBA;
			width = (cl_uint) canvasElement->width();
			height = (cl_uint) canvasElement->height();

			imageBuffer = canvasElement->buffer();
			if ( imageBuffer == NULL)
				printf("image is null\n");
			bytearray = imageBuffer->getUnmultipliedImageData(IntRect(0,0,width,height));

			if ( bytearray == NULL)
				printf("bytearray is null\n");
			printf("image width=%d height=%d \n", width,height);
			printf("image buffer width =%d \n", imageBuffer->width());
			printf("image buffer height =%d \n", imageBuffer->height());
		}

		if(bytearray->data() == NULL)
			printf("bytearray->data() is null\n");

		printf("after bytearray size = %d \n", bytearray->length());
		void* image = (void*) bytearray->data();
		if(image == NULL)
			printf("image is null\n");

		switch (flags) {
			case MEM_READ_ONLY:
				printf("inside MEM_READ_ONLY\n");
				cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR , 
						&image_format, width, height,0, image, &err);
				break;
			case MEM_WRITE_ONLY:
				cl_mem_image = clCreateImage2D(cl_context_id, (CL_MEM_READ_ONLY || CL_MEM_USE_HOST_PTR ||CL_MEM_COPY_HOST_PTR), 
						&image_format, width, height, 0, bytearray->data(), &err);
				break;
		}
		if (cl_mem_image == NULL) {
			printf("Error: clCreateImage2D- Canvas\n");
			switch (err) {
				case CL_INVALID_CONTEXT:
					printf("CL_INVALID_CONTEXT\n");
					break;
				case CL_INVALID_VALUE:
					printf("CL_INVALID_VALUE\n");
					break;
				case CL_INVALID_IMAGE_SIZE:
					printf("CL_INVALID_IMAGE_SIZE\n");
					break;
				case CL_INVALID_HOST_PTR:
					printf("CL_INVALID_HOST_PTR\n");
					break;
				case CL_IMAGE_FORMAT_NOT_SUPPORTED:
					printf("CL_INVALID_FORMAT_NOT_SUPPORTED\n");
					break;
				case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
					break;
				case CL_INVALID_OPERATION:
					printf("CL_INVALID_OPERATION\n");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("CL_OUT_OF_RESOURCES\n");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("CL_OUT_OF_HOST_MEMORY\n");
					break;
				default:
					printf("what else?\n");
					break;
			}
			return NULL;
		} else {
			printf("Success: clCreateImage2D\n");
			RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_image,false);
			m_mem_list.append(o);
			m_num_mems++;
			printf("m_num_mems=%ld\n", m_num_mems);
			m_error_state = SUCCESS;
			return o;
		}
		}	
		PassRefPtr<WebCLMem> WebCLComputeContext::createImage2D(WebCLContext* context, int flags, HTMLImageElement* image)
		{
			printf("WebCLComputeContext::createImage2D image element\n");

			cl_context cl_context_id = NULL;
			cl_int err = 0;
			cl_mem cl_mem_image = NULL;
			cl_image_format image_format;
			cl_uint width;
			cl_uint height;

			Image* imagebuf;
			CachedImage* cachedImage; 
			if (context != NULL) {
				cl_context_id = context->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}
			if (image != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				cachedImage = image->cachedImage();
				if (cachedImage == NULL) {
					printf("Error: image null\n");
					return NULL;
				} 
				else {
					width = (cl_uint) image->width();
					height = (cl_uint) image->height();
					imagebuf = cachedImage->image();
					if(imagebuf == NULL)
					{
						printf("Error: imagebuf null\n");
						return NULL;
					}
				}
			} 
			else {
				printf("Error: imageElement null\n");
				m_error_state = FAILURE;
				return NULL;
			}
			const char* image1  = (const char*)cachedImage->image()->data()->data() ;
			if(image1 == NULL)
				printf("image data is null \n");
			printf("Image Extension =  %s \n", (char*)(cachedImage->image()->filenameExtension().characters()));
			printf("Image  Decode Size =  %u \n", (cachedImage->image()->decodedSize()));
			printf("Size Of Image = %d\n",cachedImage->image()->data()->size());
			printf("Size Of Image = %s\n", image->src().string().utf8().data());
			switch (flags) {
				case MEM_READ_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
							&image_format, width, height, 0, (void*)image1, &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_WRITE_ONLY, 
							&image_format, width, height, 0, (void*)image1, &err);
					break;
			}
			if (cl_mem_image == NULL) {
				printf("Error: clCreateImage2D - HTMLImageElement\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_IMAGE_SIZE:
						printf("CL_INVALID_IMAGE_SIZE\n");
						break;
					case CL_INVALID_HOST_PTR:
						printf("CL_INVALID_HOST_PTR\n");
						break;
					case CL_IMAGE_FORMAT_NOT_SUPPORTED:
						printf("CL_INVALID_FORMAT_NOT_SUPPORTED\n");
						break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				return NULL;
			} else {
				printf("Success: clCreateImage2D\n");
				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				printf("m_num_mems=%ld\n", m_num_mems);
				m_error_state = SUCCESS;
				return o;
			}
		}
		PassRefPtr<WebCLMem> WebCLComputeContext::createImage2D(WebCLContext* context, int flags, HTMLVideoElement* video)
		{
			printf("WebCLComputeContext::createImage2D video element\n");

			cl_context cl_context_id = NULL;
			cl_int err = 0;
			cl_mem cl_mem_image = NULL;
			cl_image_format image_format;
			cl_uint width;
			cl_uint height;

			PassRefPtr<Image> image;
			SharedBuffer* sharedBuffer = NULL;
			const char* image_data = NULL;
			if (context != NULL) {
				cl_context_id = context->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}
			if (video != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				width = (cl_uint) video->width();
				height = (cl_uint) video->height();
				image = videoFrameToImage(video);
				sharedBuffer = image->data();
				if (sharedBuffer == NULL) {
					printf("sharedBuffer null\n");
					m_error_state = FAILURE;
					return NULL;
				} else {
					image_data = sharedBuffer->data();

					if (image_data == NULL) {
						printf("Error: image_data null\n");
						m_error_state = FAILURE;
						//return m_error_state;
						return NULL;
					}
				}

			} else {
				printf("Error: canvasElement null\n");
				m_error_state = FAILURE;
				return NULL;
			}

			switch (flags) {
				case MEM_READ_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
							&image_format, width, height, 0, (void *)image_data, &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_WRITE_ONLY, 
							&image_format, width, height, 0, NULL/*(void *)image_data*/, &err);
					break;
			}
			if (cl_mem_image == NULL) {
				printf("Error: clCreateImage2D- HTMLVideoElement\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_IMAGE_SIZE:
						printf("CL_INVALID_IMAGE_SIZE\n");
						break;
					case CL_INVALID_HOST_PTR:
						printf("CL_INVALID_HOST_PTR\n");
						break;
					case CL_IMAGE_FORMAT_NOT_SUPPORTED:
						printf("CL_INVALID_FORMAT_NOT_SUPPORTED\n");
						break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				return NULL;
			} else {
				printf("Success: clCreateImage2D\n");
				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				printf("m_num_mems=%ld\n", m_num_mems);
				m_error_state = SUCCESS;
				return o;
			}
		}
		PassRefPtr<WebCLMem> WebCLComputeContext::createImage2D(WebCLContext* context, int flags, ImageData* data)
		{
			printf("WebCLComputeContext::createImage2D image data\n");

			cl_context cl_context_id = NULL;
			cl_int err = 0;
			cl_mem cl_mem_image = NULL;
			cl_image_format image_format;
			cl_uint width;
			cl_uint height;

			//Image* image;
			CanvasPixelArray* pixelarray = NULL;
			ByteArray* bytearray = NULL;
			if (context != NULL) {
				cl_context_id = context->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}
			if (data != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				pixelarray = data->data();
				if(pixelarray != NULL)
				{
					bytearray = pixelarray->data();
					if(bytearray == NULL)
						return NULL;
				}			
				width = (cl_uint) data->width();
				height = (cl_uint) data->height();
			} else {
				printf("Error: canvasElement null\n");
				m_error_state = FAILURE;
				return NULL;
			}

			switch (flags) {
				case MEM_READ_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
							&image_format, width, height, 0, (void*)bytearray, &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_WRITE_ONLY, 
							&image_format, width, height, 0, NULL/*(void*)bytearray*/, &err);
					break;
			}
			if (cl_mem_image == NULL) {
				printf("Error: clCreateImage2D - ImageData\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_IMAGE_SIZE:
						printf("CL_INVALID_IMAGE_SIZE\n");
						break;
					case CL_INVALID_HOST_PTR:
						printf("CL_INVALID_HOST_PTR\n");
						break;
					case CL_IMAGE_FORMAT_NOT_SUPPORTED:
						printf("CL_INVALID_FORMAT_NOT_SUPPORTED\n");
						break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				return NULL;
			} else {
				printf("Success: clCreateImage2D\n");
				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				printf("m_num_mems=%ld\n", m_num_mems);
				m_error_state = SUCCESS;
				return o;
			}
		}
		
		PassRefPtr<WebCLMem> WebCLComputeContext::createImage2D(WebCLContext* context,int flags,unsigned int width, unsigned int height,ArrayBuffer* data)
		{
			printf("WebCLComputeContext::createImage2D array buffer\n");

			cl_context cl_context_id = NULL;
			cl_int err = 0;
			cl_mem cl_mem_image = NULL;
			cl_image_format image_format;
			cl_uint cl_width;
			cl_uint cl_height;
			if (context != NULL) {
				cl_context_id = context->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}
			if (data != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				cl_width = width;
				cl_height = height;
			} else {
				printf("Error: canvasElement null\n");
				m_error_state = FAILURE;
				return NULL;
			}

			switch (flags) {
				case MEM_READ_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
							&image_format, cl_width, cl_height, 0, data->data(), &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_image = clCreateImage2D(cl_context_id, CL_MEM_WRITE_ONLY, 
							&image_format, cl_width, cl_height, 0, NULL/*data->data()*/, &err);
					break;
			}
			if (cl_mem_image == NULL) {
				printf("Error: clCreateImage2D - ArrayBuffer\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_IMAGE_SIZE:
						printf("CL_INVALID_IMAGE_SIZE\n");
						break;
					case CL_INVALID_HOST_PTR:
						printf("CL_INVALID_HOST_PTR\n");
						break;
					case CL_IMAGE_FORMAT_NOT_SUPPORTED:
						printf("CL_INVALID_FORMAT_NOT_SUPPORTED\n");
						break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				return NULL;
			} else {
				printf("Success: clCreateImage2D\n");
				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				printf("m_num_mems=%ld\n", m_num_mems);
				m_error_state = SUCCESS;
				return o;
			}
		}

		PassRefPtr<WebCLEvent> WebCLComputeContext::enqueueWriteImage(WebCLCommandQueue* command_queue, 
				WebCLMem* mem_image, 
				bool blocking_write, 
				Int32Array* origin, 
				Int32Array* region, 
				HTMLCanvasElement* canvasElement, 
				int event_wait_list)
		{
			printf("WebCLComputeContext::enqueueWriteImage width=%d height=%d event_wait_list=%d\n", 
					canvasElement->width(), canvasElement->height(), event_wait_list);

			cl_int err = 0;
			cl_command_queue cl_command_queue_id = NULL;
			cl_mem cl_mem_image = NULL;
			cl_event cl_event_id = NULL;//sujit


			size_t *origin_array = NULL;
			size_t *region_array = NULL;


			SharedBuffer* sharedBuffer;

			Image* image;
			const char* image_data = NULL;

			if (command_queue != NULL) {
				cl_command_queue_id = command_queue->getCLCommandQueue();
				if (cl_command_queue_id == NULL) {
					printf("Error: cl_command_queue_id null\n");
					m_error_state = FAILURE;
					//return m_error_state;
					return NULL;
				}
			}

			if (mem_image != NULL) {
				cl_mem_image = mem_image->getCLMem();
				if (cl_mem_image == NULL) {
					printf("Error: cl_mem_image null\n");
					m_error_state = FAILURE;
					//return m_error_state;
					return NULL;
				}
			}

			unsigned origin_array_length = origin->length();
			origin_array = (size_t*)malloc(origin_array_length*sizeof(size_t));
			for (unsigned int i = 0; i < origin_array_length; i++) {
				origin_array[i] = origin->item(i);
			}
			unsigned  region_array_length = region->length();
			region_array = (size_t*)malloc(region_array_length*sizeof(size_t));
			for (unsigned int i = 0; i < region_array_length; i++) {
				region_array[i] = region->item(i);
			}

			if (canvasElement != NULL) {
				image = canvasElement->copiedImage();
				//printf("image width=%d height=%d size=%d\n", image->width(), image->height(), image->size());
				//sujit   printf("image width=%d height=%d size=%d\n", canvasElement->width(),canvasElement->height(), canvasElement->size());
				sharedBuffer = image->data();
				if (sharedBuffer == NULL) {
					printf("sharedBuffer null\n");
					m_error_state = FAILURE;
					return NULL;
				} else {
					image_data = sharedBuffer->data();

					if (image_data == NULL) {
						printf("Error: image_data null\n");
						m_error_state = FAILURE;
						//return m_error_state;
						return NULL;
					}
				}
			}

			switch (blocking_write) {
				case true:
					err = clEnqueueWriteImage(cl_command_queue_id, cl_mem_image, CL_TRUE, 
							origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);
					break;
				case false:
					err = clEnqueueWriteImage(cl_command_queue_id, cl_mem_image, CL_FALSE, 
							origin_array, region_array, 0, 0, image_data, event_wait_list, NULL, &cl_event_id);
					break;
			}
			if (err != CL_SUCCESS) {
				printf("Error: clEnqueueWriteImage\n");
				switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
						printf("CL_INVALID_COMMAND_QUEUE\n");
						break;
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_MEM_OBJECT:
						printf("CL_INVALID_MEM_OBJECT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_EVENT_WAIT_LIST:
						printf("CL_INVALID_EVENT_WAIT_LIST\n");
						break;
					case CL_INVALID_IMAGE_SIZE:
						printf("CL_INVALID_IMAGE_SIZE\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
						// CHECK(won.jeon) - from spec 1.1?
						//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
						//	printf("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
						//	break;
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				m_error_state = FAILURE;
				//	return m_error_state;
				return NULL;
			} else {
				printf("Success: clEnqueueWriteImage\n");
				RefPtr<WebCLEvent> o = WebCLEvent::create(this, cl_event_id);
				m_event_list.append(o);
				m_num_events++;
				printf("m_num_events=%ld\n", m_num_events);
				m_error_state = SUCCESS;
				return o;
			}
			//return FAILURE;
			return NULL;
		}
		//#endif sujit

		PassRefPtr<WebCLMem> WebCLComputeContext::createFromGLBuffer(WebCLContext* context_id, 
				int flags, WebGLBuffer* bufobj)
		{
			printf("WebCLComputeContext::createFromGLBuffer\n");

			cl_context cl_context_id = NULL;
			cl_mem cl_mem_id = NULL;
			cl_int err = 0;
			Platform3DObject buf_id = 0;

			if (context_id != NULL) {
				cl_context_id = context_id->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}
			if (bufobj != NULL) {
				buf_id = bufobj->object();
				if (buf_id == 0) {
					printf("Error: buf_id null\n");
					m_error_state = FAILURE;
					return NULL;
				}
			}


			switch (flags) {
				case MEM_READ_ONLY:
					cl_mem_id = clCreateFromGLBuffer(cl_context_id, CL_MEM_READ_ONLY, buf_id, &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_id = clCreateFromGLBuffer(cl_context_id, CL_MEM_WRITE_ONLY, buf_id, &err);
					break;
				case MEM_READ_WRITE:
					cl_mem_id = clCreateFromGLBuffer(cl_context_id, CL_MEM_READ_WRITE, buf_id, &err);
					break;

			}
			if (err != CL_SUCCESS) {
				printf("Error: clCreateFromGLBuffer\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_GL_OBJECT:
						printf("CL_INVALID_GL_OBJECT\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				m_error_state = FAILURE;
				return NULL;
			} else {
				printf("Success: clCreateFromGLBuffer\n");

				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_id, true);
				m_mem_list.append(o);
				m_num_mems++;
				printf("m_num_mems=%ld\n", m_num_mems);
				m_error_state = SUCCESS;
				return o;

			}
			return NULL;
		}

		PassRefPtr<WebCLMem> WebCLComputeContext::createFromGLTexture2D(WebCLContext* context_id, 
				int flags, GC3Denum texture_target, GC3Dint miplevel, GC3Duint texture)
		{
			printf("WebCLComputeContext::createFromGLTexture2D\n");

			cl_int err = 0;
			cl_context cl_context_id = NULL;
			cl_mem cl_mem_id = NULL;

			if (context_id != NULL) {
				cl_context_id = context_id->getCLContext();
				if (cl_context_id == NULL) {
					printf("Error: cl_context_id null\n");
					return NULL;
				}
			}
			switch (flags)
			{
				case MEM_READ_ONLY:
					cl_mem_id = clCreateFromGLTexture2D(cl_context_id, CL_MEM_READ_ONLY, 
							texture_target, miplevel, texture, &err);
					break;
				case MEM_WRITE_ONLY:
					cl_mem_id = clCreateFromGLTexture2D(cl_context_id, CL_MEM_WRITE_ONLY,
							texture_target, miplevel, texture, &err);
					break;
				case MEM_READ_WRITE:
					cl_mem_id = clCreateFromGLTexture2D(cl_context_id, CL_MEM_READ_WRITE,
							texture_target, miplevel, texture, &err);
					break;
			}
			if (err != CL_SUCCESS) {
				printf("Error: clCreateFromGLTexture2D\n");
				switch (err) {
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_MIP_LEVEL:
						printf("CL_INVALID_GL_OBJECT\n");
						break;
					case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
						printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
						break;
					case CL_INVALID_OPERATION:
						printf("CL_INVALID_OPERATION\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else\n");
						break;
				}
				m_error_state = FAILURE;
				return NULL;
			} else {
				printf("Success: clCreateFromGLTexture2D\n");
				RefPtr<WebCLMem> o = WebCLMem::create(this, cl_mem_id, true);
				m_mem_list.append(o);
				m_num_mems++;
				m_error_state = SUCCESS;
				return o;
			}
			return NULL;
		}

		long WebCLComputeContext::enqueueAcquireGLObjects(WebCLCommandQueue* command_queue, 
				WebCLMem* mem_objects, int event_wait_list)
		{
			printf("WebCLComputeContext::enqueueAcquireGLObjects event_wait_list=%d\n",
					event_wait_list);

			cl_int err = 0;
			cl_command_queue cl_command_queue_id = NULL;
			cl_mem cl_mem_ids = NULL;

			if (command_queue != NULL) {
				cl_command_queue_id = command_queue->getCLCommandQueue();
				if (cl_command_queue_id == NULL) {
					printf("Error: cl_command_queue_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			if ((mem_objects != NULL) && (mem_objects->isShared() == true)) {
				cl_mem_ids = mem_objects->getCLMem();
				if (cl_mem_ids == NULL) {
					printf("Error: cl_mem_ids null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}

			// TODO(won.jeon) - currently event-related arguments are ignored
			err = clEnqueueAcquireGLObjects(cl_command_queue_id, 1, &cl_mem_ids, 0, 0, 0);
			if (err != CL_SUCCESS) {
				printf("Error: clEnqueueAcquireGLObjects\n");
				m_error_state = FAILURE;
				switch (err) {
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_MEM_OBJECT:
						printf("CL_INVALID_MEM_OBJECT\n");
						break;
					case CL_INVALID_COMMAND_QUEUE:
						printf("CL_INVALID_COMMAND_QUEUE\n");
						break;
					case CL_INVALID_GL_OBJECT:
						printf("CL_INVALID_GL_OBJECT\n");
						break;
					case CL_INVALID_EVENT_WAIT_LIST:
						printf("CL_INVALID_EVENT_WAIT_LIST\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
				return NULL;
			} else {
				printf("Success: clEnqueueAcquireGLObjects\n");

				m_error_state = SUCCESS;
				return m_error_state;
			}
			return FAILURE;
		}

		long WebCLComputeContext::enqueueReleaseGLObjects(WebCLCommandQueue* command_queue, 
				WebCLMem* mem_objects, int event_wait_list)
		{
			printf("WebCLComputeContext::enqueueReleaseGLObjects event_wait_list=%d\n",
					event_wait_list);

			cl_command_queue cl_command_queue_id = NULL;
			cl_mem cl_mem_ids = NULL;
			cl_int err = 0;

			if (command_queue != NULL) {
				cl_command_queue_id = command_queue->getCLCommandQueue();
				if (cl_command_queue_id == NULL) {
					printf("Error: cl_command_queue_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			if ((mem_objects != NULL) && (mem_objects->isShared())) {
				cl_mem_ids = mem_objects->getCLMem();
				if (cl_mem_ids == NULL) {
					printf("Error: cl_mem_ids null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			err = clEnqueueReleaseGLObjects(cl_command_queue_id, 1, &cl_mem_ids, 
					0, 0, 0);
			if (err != CL_SUCCESS) {
				printf("Error: clEnqueueReleaseGLObjects\n");
				switch (err) {
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_MEM_OBJECT:
						printf("CL_INVALID_MEM_OBJECT\n");
						break;
					case CL_INVALID_COMMAND_QUEUE:
						printf("CL_INVALID_COMMAND_QUEUE\n");
						break;
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_GL_OBJECT:
						printf("CL_INVALID_GL_OBJECT\n");
						break;
					case CL_INVALID_EVENT_WAIT_LIST:
						printf("CL_INVALID_EVENT_WAIT_LIST\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
				}
				m_error_state = FAILURE;
				return m_error_state;
			} else {
				printf("Success: clEnqueueReleaseGLObjects\n");
				unsigned int i;

				for (i = 0; i < m_mem_list.size(); i++) {
					if ((m_mem_list[i].get())->getCLMem() == cl_mem_ids) {
						printf("found cl_mem_id\n");
						m_mem_list.remove(i);
						m_num_mems = m_mem_list.size();
						break;
					}
				}
				m_error_state = SUCCESS;
				return m_error_state;
			}
			return FAILURE;
		}

		long WebCLComputeContext::enqueueCopyBuffer(WebCLCommandQueue* command_queue, 
				WebCLMem* src_buffer, WebCLMem* dst_buffer, int cb)
		{
			printf("WebCLComputeContext::enqueueCopyBuffer cb=%d\n", cb);
			cl_command_queue cl_command_queue_id = NULL;
			cl_mem cl_src_buffer_id = NULL;
			cl_mem cl_dst_buffer_id = NULL;
			cl_int err = 0;

			if (command_queue != NULL) {
				cl_command_queue_id = command_queue->getCLCommandQueue();
				if (cl_command_queue_id == NULL) {
					printf("Error: cl_command_queue_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			if (src_buffer != NULL) {
				cl_src_buffer_id = src_buffer->getCLMem();
				if (cl_src_buffer_id == NULL) {
					printf("Error: cl_src_buffer_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			if (dst_buffer != NULL) {
				cl_dst_buffer_id = dst_buffer->getCLMem();
				if (cl_dst_buffer_id == NULL) {
					printf("Error: cl_dst_buffer_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			err = clEnqueueCopyBuffer(cl_command_queue_id, cl_src_buffer_id, cl_dst_buffer_id,
					0, 0, cb, 0, NULL, NULL);
			if (err != CL_SUCCESS) {
				printf("Error: clEnqueueCopyBuffer\n");
				switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
						printf("CL_INVALID_COMAND_QUEUE\n");
						break;
					case CL_INVALID_CONTEXT:
						printf("CL_INVALID_CONTEXT\n");
						break;
					case CL_INVALID_MEM_OBJECT:
						printf("CL_INVALID_MEM_OBJECT\n");
						break;
					case CL_INVALID_VALUE:
						printf("CL_INVALID_VALUE\n");
						break;
					case CL_INVALID_EVENT_WAIT_LIST:
						printf("CL_INVALID_EVENT_WAIT_LIST\n");
						break;
					case CL_MEM_COPY_OVERLAP:
						printf("CL_MEM_COPY_OVERLAP\n");
						break;
						/* CHECK(won.jeon) - defined in 1.1?
						   case CL_MISALIGNED_SUB_BUFFER_OFFSET:
						   printf("CL_MISALIGNED_SUB_BUFFER_OFFSET\n");
						   break;
						 */
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
						printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
						break;
				}
			} else {
				printf("Success: clEnqueueCopyBuffer\n");
				m_error_state = SUCCESS;
				return m_error_state;
			}
			return FAILURE;
		}

		long WebCLComputeContext::enqueueBarrier(WebCLCommandQueue* command_queue)
		{
			cl_int err = 0;
			cl_command_queue cl_command_queue_id = 0;

			if (command_queue != NULL) {
				cl_command_queue_id = command_queue->getCLCommandQueue();
				if (cl_command_queue_id == NULL) {
					printf("cl_command_queue_id null\n");
					m_error_state = FAILURE;
					return m_error_state;
				}
			}
			err = clEnqueueBarrier(cl_command_queue_id);
			if (err != CL_SUCCESS) {
				printf("Error: clEnqueueBarrier\n");
				switch (err) {
					case CL_INVALID_COMMAND_QUEUE:
						printf("CL_INVALID_COMMAND_QUEUE\n");
						break;
					case CL_OUT_OF_RESOURCES:
						printf("CL_OUT_OF_RESOURCES\n");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						printf("CL_OUT_OF_HOST_MEMORY\n");
						break;
					default:
						printf("what else?\n");
				}
				m_error_state = FAILURE;
				return m_error_state;
			} else {
				printf("Success: clEnqueueBarrier\n");
				m_error_state = SUCCESS;
				return m_error_state;
			}
			return FAILURE;
		}


		WebCLComputeContext::LRUImageBufferCache::LRUImageBufferCache(int capacity)
			: m_buffers(adoptArrayPtr(new OwnPtr<ImageBuffer>[capacity]))
			  , m_capacity(capacity)
		{
		}

		ImageBuffer* WebCLComputeContext::LRUImageBufferCache::imageBuffer(const IntSize& size)
		{
			int i;
			for (i = 0; i < m_capacity; ++i) {
				ImageBuffer* buf = m_buffers[i].get();
				if (!buf)
					break;
				if (buf->size() != size)
					continue;
				bubbleToFront(i);
				return buf;
			}

			OwnPtr<ImageBuffer> temp = ImageBuffer::create(size);
			if (!temp)
				return 0;
			i = std::min(m_capacity - 1, i);
			m_buffers[i] = temp.release();

			ImageBuffer* buf = m_buffers[i].get();
			bubbleToFront(i);
			return buf;
		}

		void WebCLComputeContext::LRUImageBufferCache::bubbleToFront(int idx)
		{
			for (int i = idx; i > 0; --i)
				m_buffers[i].swap(m_buffers[i-1]);
		}

		PassRefPtr<Image> WebCLComputeContext::videoFrameToImage(HTMLVideoElement* video)
		{
			if (!video || !video->videoWidth() || !video->videoHeight()) {
				return 0;
			}
			IntSize size(video->videoWidth(), video->videoHeight());
			ImageBuffer* buf = m_videoCache.imageBuffer(size);
			if (!buf) {
				return 0;
			}
			IntRect destRect(0, 0, size.width(), size.height());
			// FIXME: Turn this into a GPU-GPU texture copy instead of CPU readback.
			video->paintCurrentFrameInContext(buf->context(), destRect);
			return buf->copyImage();
		}

	} // namespace WebCore

#endif // ENABLE(WEBCL)


