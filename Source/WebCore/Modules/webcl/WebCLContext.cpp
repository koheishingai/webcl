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

#include "WebCLContext.h"
#include "WebCLContext.h"
#include "WebCLCommandQueue.h"
#include "WebCLProgram.h"
#include "WebCLKernel.h"
#include "WebCLMem.h"
#include "WebCLEvent.h"
#include "WebCLSampler.h"
#include "WebCL.h"
#include "WebCLException.h"

namespace WebCore {

WebCLContext::~WebCLContext()
{
}

PassRefPtr<WebCLContext> WebCLContext::create(WebCL* compute_context, cl_context context_id)
{
    return adoptRef(new WebCLContext(compute_context, context_id));
}

    WebCLContext::WebCLContext(WebCL* compute_context, cl_context context_id) 
: m_videoCache(4), m_context(compute_context), m_cl_context(context_id)
{
    m_num_programs = 0;
    m_num_mems = 0;
    m_num_events = 0;
    m_num_samplers = 0;
    m_num_contexts = 0;

}

WebCLGetInfo WebCLContext::getInfo(int param_name, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_uint uint_units = 0;
    RefPtr<WebCLDeviceList> deviceList  = NULL;
    size_t szParmDataBytes = 0;
    size_t uint_array[1024] = {0};

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return WebCLGetInfo();
    }

    switch(param_name)
    {	
        case WebCL::CONTEXT_REFERENCE_COUNT:
            err = clGetContextInfo(m_cl_context, CL_CONTEXT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
            if (err == CL_SUCCESS)
                return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
            break;
            /*				case WebCL::CONTEXT_NUM_DEVICES:
                            err = clGetContextInfo(m_cl_context,CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &uint_units, NULL);
                            if (err == CL_SUCCESS)
                            return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
                            break;
                            */                        
        case WebCL::CONTEXT_DEVICES:
            cl_device_id* cdDevices;
            clGetContextInfo(m_cl_context, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);		
            printf(" szParmDataBytes  => %lu ", szParmDataBytes );
            if (err == CL_SUCCESS) {
                int nd = szParmDataBytes / sizeof(cl_device_id);
                cdDevices = (cl_device_id*) malloc(szParmDataBytes);
                clGetContextInfo(m_cl_context, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
                deviceList = WebCLDeviceList::create(m_context, cdDevices, nd);
                printf("Size Vs Size = %lu %d %d \n\n", szParmDataBytes,nd,deviceList->length());
                free(cdDevices);
                return WebCLGetInfo(PassRefPtr<WebCLDeviceList>(deviceList));
            }
            break;	
        case WebCL::CONTEXT_PROPERTIES:
            err = clGetContextInfo(m_cl_context, CL_CONTEXT_PROPERTIES, 0, NULL, &szParmDataBytes);
            if (err == CL_SUCCESS) {
                printf(" szParmDataBytes  => %lu ", szParmDataBytes );
                int nd = szParmDataBytes / sizeof(cl_uint);
                if(nd == 0)	 
                {	
                    printf("No Context Properties defined \n");
                    return WebCLGetInfo();
                }
                err = clGetContextInfo(m_cl_context, CL_CONTEXT_PROPERTIES, szParmDataBytes, &uint_array, &szParmDataBytes);
                if (err == CL_SUCCESS) {
                    // Should int repacle cl_context_properties
                    int values[1024] = {0};
                    for(int i=0; i<((int)nd); i++)
                    {
                        values[i] = (int)uint_array[i];
                        printf("%d\n", values[i]);
                    }
                    return WebCLGetInfo(Int32Array::create(values, nd));
                }	
            }
            break;
        default:
            printf("Error: Unsupported Context Info type\n");
            ec = WebCLException::FAILURE;
            return WebCLGetInfo();
    }
    switch (err) {
        case CL_INVALID_CONTEXT:
            ec = WebCLException::INVALID_CONTEXT;
            printf("Error: CL_INVALID_CONTEXT \n");
            break;
        case CL_INVALID_VALUE:
            ec = WebCLException::INVALID_VALUE;
            printf("Error: CL_INVALID_VALUE\n");
            break;
        case CL_OUT_OF_RESOURCES:
            ec = WebCLException::OUT_OF_RESOURCES;
            printf("Error: CL_OUT_OF_RESOURCES \n");
            break;
        case CL_OUT_OF_HOST_MEMORY:
            ec = WebCLException::OUT_OF_HOST_MEMORY;
            printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
            break;
        default:
            ec = WebCLException::FAILURE;
            printf("Error: Invaild Error Type\n");
            break;
    }				
    return WebCLGetInfo();
}

PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDeviceList* devices, 
        int command_queue_prop, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_device_id* cl_device = NULL;
    cl_command_queue cl_command_queue_id = NULL;

    //cl_int err_dev = 0;
    //cl_uint m_num_platforms = 0;
    //cl_uint num_devices = 0;
    //cl_platform_id* m_cl_platforms = NULL;


    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }
    if (devices != NULL) {
        cl_device = devices->getCLDevices();
        if (cl_device == NULL) {
            ec = WebCLException::INVALID_DEVICE;
            printf("Error: cl_device null\n");
            return NULL;
        }
    }
    else {

        printf(" devices is NULL \n ");
        cl_device = m_context->getcl_device_id();
        //cl_int err_dev = 0;
        //cl_uint m_num_platforms = 0;
        //cl_uint num_devices = 0;
        //cl_platform_id* m_cl_platforms = NULL;


        /*err_dev = clGetPlatformIDs(0, NULL, &m_num_platforms);

          if(err_dev == CL_SUCCESS) {
          m_cl_platforms = new cl_platform_id[m_num_platforms];
          err_dev = clGetPlatformIDs(m_num_platforms, m_cl_platforms, NULL);
          }
          if(err_dev == CL_SUCCESS) {
          err_dev = clGetDeviceIDs(m_cl_platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, NULL, &num_devices);
          }
          if((num_devices != 0) && (err == CL_SUCCESS)) {
          cl_device = new cl_device_id[num_devices];
          err_dev = clGetDeviceIDs(m_cl_platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, cl_device,
          &num_devices);

          }
          else {
          ec = WebCLException::INVALID_DEVICE;
          printf("Error: Device Type Not Supported \n");
          return NULL;
          }*/



        //	Creates a new command queue for the devices in the given array.
        //If devices is null, the WebCL implementation will select any single WebCLDevice that matches the given properties and is covered by this WebCLContext.
        //If properties is omitted, the command queue is created with out-of-order execution disabled and profiling disabled


    }

    switch (command_queue_prop)
    {
        case WebCL::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
            cl_command_queue_id = clCreateCommandQueue(m_cl_context, *cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
            break;
        case WebCL::QUEUE_PROFILING_ENABLE:
            cl_command_queue_id = clCreateCommandQueue(m_cl_context, *cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
            break;
        default:
            cl_command_queue_id = clCreateCommandQueue(m_cl_context, *cl_device,NULL, &err);
            break;
    }
    if (err != CL_SUCCESS  ) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT \n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE \n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_DEVICE:
                printf("Error: CL_INVALID_DEVICE \n");
                ec = WebCLException::INVALID_DEVICE;
                break;
            case CL_INVALID_QUEUE_PROPERTIES:
                printf("Error: CL_INVALID_QUEUE_PROPERTIES \n");
                ec = WebCLException::INVALID_QUEUE_PROPERTIES;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES \n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY \n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
        if (o != NULL) {
            m_command_queue = o;
            return o;
        } else {
            ec = WebCLException::INVALID_COMMAND_QUEUE;
            return NULL;
        }
    }
    return NULL;
}
/*
   PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDevice* device, int command_queue_prop, ExceptionCode& ec)
   {
   cl_int err = 0;
   cl_device_id cl_device = NULL;
   cl_command_queue cl_command_queue_id = NULL;
   if (m_cl_context == NULL) {
   printf("Error: Invalid CL Context\n");
   ec = WebCLException::INVALID_CONTEXT;
   return NULL;
   }
   if (device != NULL) {
   cl_device = device->getCLDevice();
   if (cl_device == NULL) {
   printf("Error: cl_device null\n");
   return NULL;
   }
   }
   switch (command_queue_prop)
   {
   case WebCL::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
   cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
   break;
   case WebCL::QUEUE_PROFILING_ENABLE:
   cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
   break;
   default:
   cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, NULL, &err);
   break;
   }

   if (err != CL_SUCCESS) {
   switch (err) {
   case CL_INVALID_CONTEXT:
   printf("Error: CL_INVALID_CONTEXT \n");
   ec = WebCLException::INVALID_CONTEXT;
   break;
   case CL_INVALID_VALUE:
   printf("Error: CL_INVALID_VALUE \n");
   ec = WebCLException::INVALID_VALUE;
   break;
   case CL_INVALID_DEVICE:
   printf("Error: CL_INVALID_DEVICE  \n");
   ec = WebCLException::INVALID_DEVICE;
   break;
   case CL_INVALID_QUEUE_PROPERTIES:
   printf("Error: CL_INVALID_QUEUE_PROPERTIES  \n");
   ec = WebCLException::INVALID_QUEUE_PROPERTIES;
   break;
   case CL_OUT_OF_RESOURCES:
   printf("Error: CL_OUT_OF_RESOURCES \n");
   ec = WebCLException::OUT_OF_RESOURCES;
   break;
   case CL_OUT_OF_HOST_MEMORY:
   printf("Error: CL_OUT_OF_HOST_MEMORY \n");
   ec = WebCLException::OUT_OF_HOST_MEMORY;
   break;

   default:
   printf("Error: Invaild Error Type\n");
   ec = WebCLException::FAILURE;
   break;
   }

   } else {
   RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
   if (o != NULL) {
   m_command_queue = o;
   return o;
   } else {
   ec = WebCLException::INVALID_COMMAND_QUEUE;
return NULL;
}
}
return NULL;
}
*/

PassRefPtr<WebCLProgram> WebCLContext::createProgram(const String& kernelSource, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_program cl_program_id = NULL;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }
    const char* source = strdup(kernelSource.utf8().data());

    // TODO(won.jeon) - the second and fourth arguments need to be addressed later
    cl_program_id = clCreateProgramWithSource(m_cl_context, 1, (const char**)&source, 
            NULL, &err);

    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT \n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE \n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES \n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY \n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }

    } else {
        RefPtr<WebCLProgram> o = WebCLProgram::create(m_context, cl_program_id);
        o->setDevice(m_device_id);
        if (o != NULL) {
            m_program_list.append(o);
            m_num_programs++;
            return o;
        } else {
            ec = WebCLException::INVALID_PROGRAM;
            return NULL;
        }
    }
    return NULL;
}

/*PassRefPtr<WebCLMem> WebCLContext::createBuffer(int flags, int size, int host_ptr, ExceptionCode& ec)
  {
  cl_int err = 0;	
  cl_mem cl_mem_id = NULL;
  if (m_cl_context == NULL) {
  printf("Error: Invalid CL Context\n");
  ec = WebCLException::INVALID_CONTEXT;
  return NULL;
  }
// TODO(won.jeon) - NULL parameter needs to be addressed later
switch (flags)
{
case WebCL::MEM_READ_ONLY:
cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_ONLY, size, NULL, &err);
break;
case WebCL::MEM_WRITE_ONLY:
cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_WRITE_ONLY, size, NULL, &err);
break;
case WebCL::MEM_READ_WRITE:
cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_WRITE, size, NULL, &err);
break;
default:
printf("Error: Unsupported Mem Flsg\n");
printf("WebCLContext::createBuffer host_ptr = %d\n", host_ptr);
ec = WebCLException::INVALID_CONTEXT;
break;
}
if (err != CL_SUCCESS) {
printf("Error: clCreateBuffer\n");
switch(err) {
case CL_INVALID_CONTEXT:
printf("Error: CL_INVALID_CONTEXT\n");
ec = WebCLException::INVALID_CONTEXT;
break;
case CL_INVALID_VALUE:
printf("Error: CL_INVALID_VALUE\n");
ec = WebCLException::INVALID_VALUE;
break;
case CL_INVALID_BUFFER_SIZE:
printf("Error: CL_INVALID_BUFFER_SIZE\n");
ec = WebCLException::INVALID_BUFFER_SIZE;
break;
case CL_INVALID_HOST_PTR:
printf("Error: CL_INVALID_HOST_PTR\n");
ec = WebCLException::INVALID_HOST_PTR;
break;
case CL_MEM_OBJECT_ALLOCATION_FAILURE:
printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
break;
case CL_OUT_OF_HOST_MEMORY:
printf("Error: CL_OUT_OF_HOST_MEMORY\n");
ec = WebCLException::OUT_OF_HOST_MEMORY;
break;
case CL_OUT_OF_RESOURCES:
printf("Error: CL_OUT_OF_RESOURCES\n");
ec = WebCLException::OUT_OF_RESOURCES;
break;
default:
printf("Error: Invaild Error Type\n");
ec = WebCLException::FAILURE;
break;
}
} else {
RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
m_mem_list.append(o);
m_num_mems++;
return o;
}
return NULL;
}*/



PassRefPtr<WebCLMem> WebCLContext::createBuffer(int flags, int size, ArrayBuffer* data, ExceptionCode& ec)
{
        printf("Sharath : in func createBuffer with params [%d][%d]",flags,size);

    cl_int err = 0;
    cl_mem cl_mem_id = NULL;
    void *vData = NULL;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }


    if (data == NULL) {
        //printf("createBuffer:Error: ArraryBuffer null SIze =%d\n", size);
        //ec = WebCLException::FAILURE;
        //return NULL;
        vData = NULL;
    }
    else {
        //printf(" ArraryBuffer NOT null\n");
        vData = data->data();
    }
    // TODO(won.jeon) - NULL parameter needs to be addressed later
    switch (flags)
    {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_ONLY, size, vData , &err);
            //printf(" WebCL::MEM_READ_ONLY  ");
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_WRITE_ONLY, size, vData , &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_WRITE, size, vData , &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_USE_HOST_PTR, size, vData , &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, size, vData , &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, size, vData , &err);
            break;
            /*case WebCL::MEM_HOST_WRITE_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_WRITE_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_READ_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_READ_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_NO_ACCESS:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_NO_ACCESS, size, vData , &err);
              break;*/

        default:
            printf("Error: Unsupported Mem Flag\n");
            //printf("WebCLContext::createBuffer host_ptr = %d\n", data->data());
            ec = WebCLException::INVALID_CONTEXT;
            break;
    }
    if (err != CL_SUCCESS) {
        printf("Error: clCreateBuffer\n");
        switch(err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_BUFFER_SIZE:
                printf("Error: CL_INVALID_BUFFER_SIZE\n");
                ec = WebCLException::INVALID_BUFFER_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}


PassRefPtr<WebCLMem> WebCLContext::createBuffer(int memFlags, ImageData *ptr , ExceptionCode& ec)
{
    printf(" inside WebCLContext::createBuffer(int memFlags, ImageData *ptr , ExceptionCode& ec) \n ");

    unsigned char* buffer;
    int buffer_size = 0;

    cl_int err = 0;
    cl_mem cl_mem_id = NULL;

    if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()!=NULL )
    {
        buffer = ptr->data()->data();
        printf(" buffer => %s ",buffer);
        buffer_size =  ptr->data()->length();
        printf(" buffer_size => %d ",buffer_size);
    }
    else
    {
        printf("Error: Invalid ImageData\n");
        ec = WebCL::FAILURE;
        return NULL;

    }

    switch (memFlags)
    {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_ONLY, buffer_size, buffer , &err);
            //printf(" WebCL::MEM_READ_ONLY  ");
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_WRITE_ONLY, buffer_size, buffer , &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_WRITE, buffer_size, buffer , &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_USE_HOST_PTR, buffer_size, buffer , &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, buffer_size, buffer , &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, buffer_size, buffer , &err);
            break;
            /*case WebCL::MEM_HOST_WRITE_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_WRITE_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_READ_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_READ_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_NO_ACCESS:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_NO_ACCESS, size, vData , &err);
              break;*/

        default:
            printf("Error: Unsupported Mem Flag\n");
            //printf("WebCLContext::createBuffer host_ptr = %d\n", data->data());
            ec = WebCLException::INVALID_CONTEXT;
            break;
    }

    if (err != CL_SUCCESS) {
        printf("Error: clCreateBuffer\n");
        switch(err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_BUFFER_SIZE:
                printf("Error: CL_INVALID_BUFFER_SIZE\n");
                ec = WebCLException::INVALID_BUFFER_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;


}
PassRefPtr<WebCLMem> WebCLContext::createBuffer(int memFlags, HTMLCanvasElement *srcCanvas,ExceptionCode& ec) 
{

    printf(" inside PassRefPtr<WebCLMem> WebCLContext::createBuffer(int memFlags, HTMLCanvasElement *srcCanvas,ExceptionCode& ec)   \n ");



    //unsigned char* buffer;
    int buffer_size = 0;

    cl_int err = 0;
    cl_mem cl_mem_id = NULL;

    //ImageData *ptr;

    cl_uint width  =0;
    cl_uint height =0;

    ImageBuffer* imageBuffer = NULL;
    RefPtr<Uint8ClampedArray> bytearray = NULL;
    void* image = NULL;

    if(srcCanvas != NULL )
    {
        width = (cl_uint) srcCanvas->width();
        height = (cl_uint) srcCanvas->height();

        printf(" WebCLContext::createBuffer(int memFlags, HTMLCanvasElement *srcCanvas,ExceptionCode& ec): width => %d  height => %d   \n ",width,height);


        imageBuffer = srcCanvas->buffer();
        if ( imageBuffer == NULL)
            printf("image is null\n");
        bytearray = imageBuffer->getUnmultipliedImageData(IntRect(0,0,width,height));

        if ( bytearray == NULL)
            printf("bytearray is null\n");


        /*if(srcCanvas->getImageData() != NULL)//(srcCanvas->getContext("2d") != NULL)
          {
          ptr = srcCanvas->getImageData().get(); 
          }
          else
          {
          printf("Error: HTMLCanvasElement:srcCanvas->getImageData() is NULL \n ");
          ec = WebCL::FAILURE;
          return NULL;
          }

*/

        if(bytearray->data() == NULL)
        {
            printf("bytearray->data() is null\n");
            ec = WebCLException::FAILURE;
            return NULL;
        }
        image = (void*) bytearray->data();
        buffer_size = bytearray->length();
        if(image == NULL)
        {
            printf("image is null\n");
            ec = WebCLException::FAILURE;
            return NULL;
        }




    }
    else
    {
        printf("Error: HTMLCanvasElement:srcCanvas is NULL \n ");
        ec = WebCL::FAILURE;
        return NULL;
    }

    /*
       if(ptr!=NULL && ptr->data()!=NULL && ptr->data()->data()!=NULL && ptr->data()->data()->data()!=NULL )
       {
       buffer = ptr->data()->data()->data();
       printf(" buffer => %s ",buffer);
       buffer_size =  ptr->data()->data()->length();
       printf(" buffer_size => %d ",buffer_size);
       }
       else
       {
       printf("Error: Invalid ImageData\n");
       ec = WebCL::FAILURE;
       return NULL;

       }
       */

    switch (memFlags)
    {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_ONLY, buffer_size,/* buffer */ image , &err);
            //printf(" WebCL::MEM_READ_ONLY  ");
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_WRITE_ONLY, buffer_size,/* buffer */ image , &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_READ_WRITE, buffer_size,/* buffer */ image , &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_USE_HOST_PTR, buffer_size,/* buffer */ image, &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, buffer_size,/* buffer */ image , &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, buffer_size, /* buffer */ image, &err);
            break;
            /*case WebCL::MEM_HOST_WRITE_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_WRITE_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_READ_ONLY:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_READ_ONLY, size, vData , &err);
              break;
              case WebCL::MEM_HOST_NO_ACCESS:
              cl_mem_id = clCreateBuffer(m_cl_context, CL_MEM_HOST_NO_ACCESS, size, vData , &err);
              break;*/

        default:
            printf("Error: Unsupported Mem Flag\n");
            //printf("WebCLContext::createBuffer host_ptr = %d\n", data->data());
            ec = WebCLException::INVALID_CONTEXT;
            break;
    }

    if (err != CL_SUCCESS) {
        printf("Error: clCreateBuffer\n");
        switch(err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_BUFFER_SIZE:
                printf("Error: CL_INVALID_BUFFER_SIZE\n");
                ec = WebCLException::INVALID_BUFFER_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL; 
}




PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, 
        HTMLCanvasElement* canvasElement, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format = {CL_RGBA, CL_UNSIGNED_INT32};
    cl_uint width = 0;
    cl_uint height = 0;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }	
    ImageBuffer* imageBuffer = NULL;
    RefPtr<Uint8ClampedArray> bytearray = NULL;
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
    }

    if(bytearray->data() == NULL)
    {
        printf("bytearray->data() is null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    void* image = (void*) bytearray->data();
    if(image == NULL)
    {
        printf("image is null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR , 
                    &image_format, width, height,0, image, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, ( CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR  /* |CL_MEM_COPY_HOST_PTR */), 
                    &image_format, width, height, 0, image, &err);
            break;
            // TODO (siba samal) Support other mem_flags & testing 
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}	

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLImageElement* image, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = 0;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    Image* imagebuf = NULL;
    CachedImage* cachedImage = NULL; 
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }
    if (image != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        cachedImage = image->cachedImage();
        if (cachedImage == NULL) {
            ec = WebCLException::FAILURE;
            printf("Error: image null\n");
            return NULL;
        } 
        else {
            width = (cl_uint) image->width();
            height = (cl_uint) image->height();
            imagebuf = cachedImage->image();
            if(imagebuf == NULL)
            {
                ec = WebCLException::FAILURE;
                printf("Error: imagebuf null\n");
                return NULL;
            }
        }
    } 
    else {
        printf("Error: imageElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    const char* image1  = (const char*)cachedImage->image()->data()->data() ;
    if(image1 == NULL)
    {
        ec = WebCLException::FAILURE;
        printf("Error: image data is null\n");
        return NULL;
    }
    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void*)image1, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, width, height, 0, (void*)image1, &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break; 
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLVideoElement* video, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    RefPtr<Image> image = NULL;
    SharedBuffer* sharedBuffer = NULL;
    const char* image_data = NULL;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (video != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        width = (cl_uint) video->width();
        height = (cl_uint) video->height();
        image = videoFrameToImage(video);
        sharedBuffer = image->data();
        if (sharedBuffer == NULL) {
            printf("Error: sharedBuffer null\n");
            ec = WebCLException::FAILURE;
            return NULL;
        } else {
            image_data = sharedBuffer->data();

            if (image_data == NULL) {
                printf("Error: image_data null\n");
                ec = WebCLException::FAILURE;
                return NULL;
            }
        }

    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, ImageData* data, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    //CanvasPixelArray* pixelarray = NULL;
    Uint8ClampedArray* bytearray = NULL;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (data != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        //pixelarray = data->data();
        if(data->data() != NULL)
        {
            bytearray = data->data();
            if(bytearray == NULL)
                return NULL;
        }			
        width = (cl_uint) data->width();
        height = (cl_uint) data->height();
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void*)bytearray, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, width, height, 0, (void*)bytearray, &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

/******* start create image ****/





PassRefPtr<WebCLMem> WebCLContext::createImage(int flags, 
        HTMLCanvasElement* canvasElement, ExceptionCode& ec)
{

    printf( " Inside WebCLContext::createImage(int flags, HTMLCanvasElement* canvasElement, ExceptionCode& ec) \n  "  );

    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format; //={CL_RGBA, CL_FLOAT};//CL_UNSIGNED_INT8};
    cl_uint width = 0;
    cl_uint height = 0;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }	
//ImageBuffer* imageBuffer = NULL;
RefPtr<Uint8ClampedArray> bytearray = NULL;
if (canvasElement != NULL) {
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;
    image_format.image_channel_order = CL_RGBA;
    width = (cl_uint) canvasElement->width();
    height = (cl_uint) canvasElement->height();

    //imageBuffer = canvasElement->buffer();
    if ( /* imageBuffer */ canvasElement->buffer() == NULL)
        printf("image is null\n");
    bytearray = canvasElement->buffer()->getUnmultipliedImageData(IntRect(0,0,width,height));





    if ( bytearray == NULL)
    {
        printf("bytearray is null\n");
    }
    else
    {
        for(unsigned int  i =0 ;i<24 ; i++)
        {
            printf("  before clCreateImage2D(canvasElement) call pixelarray => %d => %u \n ",i,bytearray->item(i));
        }
    }

}

if(bytearray->data() == NULL)
{
    printf("bytearray->data() is null\n");
    ec = WebCLException::FAILURE;
    return NULL;
}
void* image = (void*) bytearray->data();
if(image == NULL)
{
    printf("image is null\n");
    ec = WebCLException::FAILURE;
    return NULL;
}

switch (flags) {
    case WebCL::MEM_READ_ONLY:
        cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR , 
                &image_format, width, height,0, image, &err);
        break;
    case WebCL::MEM_WRITE_ONLY:
        cl_mem_image = clCreateImage2D(m_cl_context, ( CL_MEM_WRITE_ONLY  /*| CL_MEM_COPY_HOST_PTR | CL_MEM_USE_HOST_PTR */   ) , 
                &image_format, width, height, 0, NULL/*image*/, &err);
        for(unsigned int  i =0 ;i<24 ; i++)
        {
            printf("  after clCreateImage2D(canvasElement) call pixelarray => %d => %u \n ",i,bytearray->item(i));
        }


        break;
        // TODO (siba samal) Support other mem_flags & testing 
}
if (cl_mem_image == NULL) {
    switch (err) {
        case CL_INVALID_CONTEXT:
            printf("Error: CL_INVALID_CONTEXT\n");
            ec = WebCLException::INVALID_CONTEXT;
            break;
        case CL_INVALID_VALUE:
            printf("Error: CL_INVALID_VALUE\n");
            ec = WebCLException::INVALID_VALUE;
            break;
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
            ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
            break;
        case CL_INVALID_IMAGE_SIZE:
            printf("Error: CL_INVALID_IMAGE_SIZE\n");
            ec = WebCLException::INVALID_IMAGE_SIZE;
            break;
        case CL_INVALID_HOST_PTR:
            printf("Error: CL_INVALID_HOST_PTR\n");
            ec = WebCLException::INVALID_HOST_PTR;
            break;
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
            ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
            break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
            ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
            break;
        case CL_INVALID_OPERATION:
            printf("Error: CL_INVALID_OPERATION\n");
            ec = WebCLException::INVALID_OPERATION;
            break;
        case CL_OUT_OF_RESOURCES:
            printf("Error: CL_OUT_OF_RESOURCES\n");
            ec = WebCLException::OUT_OF_RESOURCES;
            break;
        case CL_OUT_OF_HOST_MEMORY:
            printf("Error: CL_OUT_OF_HOST_MEMORY\n");
            ec = WebCLException::OUT_OF_HOST_MEMORY;
            break;
        default:
            printf("Error: Invaild Error Type\n");
            ec = WebCLException::FAILURE;
            break;
    }
} else {
    RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
    m_mem_list.append(o);
    m_num_mems++;
    return o;
}
return NULL;
}	

PassRefPtr<WebCLMem> WebCLContext::createImage(int flags, HTMLImageElement* image, ExceptionCode& ec)
{

    printf( " Inside WebCLContext::createImage(int flags, HTMLImageElement* image, ExceptionCode& ec) \n  "  );

    cl_int err = 0;
    cl_mem cl_mem_image = 0;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    Image* imagebuf = NULL;
    CachedImage* cachedImage = NULL; 
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }
    if (image != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        cachedImage = image->cachedImage();
        if (cachedImage == NULL) {
            ec = WebCLException::FAILURE;
            printf("Error: image null\n");
            return NULL;
        } 
        else {
            width = (cl_uint) image->width();
            height = (cl_uint) image->height();
            imagebuf = cachedImage->image();
            if(imagebuf == NULL)
            {
                ec = WebCLException::FAILURE;
                printf("Error: imagebuf null\n");
                return NULL;
            }
        }
    } 
    else {
        printf("Error: imageElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    OwnPtr<ImageBuffer> buffer;
    IntRect rect(0, 0, width, height);
    RefPtr<Uint8ClampedArray> byteArray;
    void *data;


    buffer = ImageBuffer::create(IntSize(width, height));
    buffer->context()->drawImage(cachedImage->image(), ColorSpaceDeviceRGB, rect);
    byteArray = buffer->getUnmultipliedImageData(rect);
    data = byteArray->data();

    if(data == NULL)
    {   
        ec = WebCLException::FAILURE;
        printf("Error: image data is null\n");
        return NULL;
    }   



    /*

       const char* image1  = (const char*)cachedImage->image()->data()->data() ;
       if(image1 == NULL)
       {
       ec = WebCLException::FAILURE;
       printf("Error: image data is null\n");
       return NULL;
       }

*/

    //for(i=0; i<20; i++)
    printf(" inside ebCLContext::createImage(int flags, HTMLImageElement* image, ExceptionCode& ec)  \n ");
    // printf(" Data = %s \n ", (char*)data);

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_COPY_HOST_PTR /*CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR*/, 
                    &image_format, width, height, 0, data /*(void*)image1*/, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, width, height, 0, data /*(void*)image1*/, &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break; 
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage(int flags, HTMLVideoElement* video, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    RefPtr<Image> image = NULL;
    SharedBuffer* sharedBuffer = NULL;
    const char* image_data = NULL;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (video != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        width = (cl_uint) video->width();
        height = (cl_uint) video->height();
        image = videoFrameToImage(video);
        sharedBuffer = image->data();
        if (sharedBuffer == NULL) {
            printf("Error: sharedBuffer null\n");
            ec = WebCLException::FAILURE;
            return NULL;
        } else {
            image_data = sharedBuffer->data();

            if (image_data == NULL) {
                printf("Error: image_data null\n");
                ec = WebCLException::FAILURE;
                return NULL;
            }
        }

    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY| CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;

        case WebCL::MEM_READ_WRITE:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_WRITE, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_ALLOC_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_COPY_HOST_PTR, 
                    &image_format, width, height, 0, (void *)image_data, &err);
            break;

            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage(int flags, ImageData* data, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint width = 0;
    cl_uint height = 0;

    //CanvasPixelArray* pixelarray = NULL;
    Uint8ClampedArray* bytearray = NULL;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (data != NULL) {
        //printf("  data = %s =  \n " , data);
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        /*pixelarray = data->data();
          if(pixelarray != NULL)
          {
        //for(unsigned int  i =0 ;pixelarray->length(); i++)
        for(unsigned int  i =0 ;i<60 ; i++)
        {
        printf("  pixelarray => %d => %u \n ",i,pixelarray->get(i));
        }
        bytearray = pixelarray->data();
        if(bytearray == NULL)
        return NULL;
        }
        */
        if(data->data() != NULL )
        {

            for(unsigned int  i =0 ;i<24 ; i++)
            {
                printf("  before clCreateImage2D(ImageData) call pixelarray => %d => %u \n ",i,data->data()->item(i));
            }

            if(data->data()->data() != NULL)
            {
                bytearray = data->data();
            }
            else
            {
                printf("Error: canvasElement data->data()->data() is  null \n");
                ec = WebCLException::FAILURE;
                return NULL;
            }
        }
        else 
        {
            printf("Error: canvasElement data->data() is  null \n");
            ec = WebCLException::FAILURE;
            return NULL;
        }

        width = (cl_uint) data->width();
        height = (cl_uint) data->height();


    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }




    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void*)bytearray, &err);
            for(unsigned int  i =0 ;i<24 ; i++)
            {
                printf("  after clCreateImage2D(ImageData) call pixelarray => %d => %u \n ",i,data->data()->item(i));
            }

            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, width, height, 0, (void*)bytearray, &err);
            break;

            /*case WebCL::MEM_READ_WRITE:
              cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_WRITE, 
              &image_format, width, height, 0, (void *)image_data, &err);
              break;
              case WebCL::MEM_USE_HOST_PTR:
              cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_USE_HOST_PTR, 
              &image_format, width, height, 0, (void *)image_data, &err);
              break;
              case WebCL::MEM_ALLOC_HOST_PTR:
              cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_ALLOC_HOST_PTR, 
              &image_format, width, height, 0, (void *)image_data, &err);
              break;
              case WebCL::MEM_COPY_HOST_PTR:
              cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_COPY_HOST_PTR, 
              &image_format, width, height, 0, (void *)image_data, &err); */


            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}


PassRefPtr<WebCLMem> WebCLContext::createImageWithDescriptor(int flags,WebCLImageDescriptor* descriptor, ArrayBuffer* data,ExceptionCode& ec )
{
    //flags = 0;
    //descriptor = NULL;
    //srcBuffer = NULL;
    //ec = NULL;
    //return(NULL);

    /********************************************************************************************************************************************************/

    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint cl_width = 0;
    cl_uint cl_height = 0;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (data != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        cl_width =  descriptor->width();
        cl_height = descriptor->height();
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, cl_width, cl_height, 0, data->data(), &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, cl_width, cl_height, 0, data->data(), &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;



}

/******* end create image ******/


PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags,unsigned int width, 
        unsigned int height,ArrayBuffer* data, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint cl_width = 0;
    cl_uint cl_height = 0;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (data != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        cl_width = width;
        cl_height = height;
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, cl_width, cl_height, 0, data->data(), &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, cl_width, cl_height, 0, data->data(), &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage3D(int flags,unsigned int width, 
        unsigned int height,
        unsigned int depth,
        ArrayBuffer* data, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_image = NULL;
    cl_image_format image_format;
    cl_uint cl_width = 0;
    cl_uint cl_height = 0;
    cl_uint cl_depth = 0;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (data != NULL) {
        image_format.image_channel_data_type = CL_UNSIGNED_INT8;
        image_format.image_channel_order = CL_RGBA;
        cl_width = width;
        cl_height = height;
        cl_depth = depth;
    } else {
        printf("Error: canvasElement null\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }

    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
                    &image_format, cl_width, cl_height, cl_depth,0, 0, data->data(), &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_WRITE_ONLY, 
                    &image_format, cl_width, cl_height, cl_depth, 0, 0,data->data(), &err);
            break;
            // TODO (siba samal) Support other flags & testing
    }
    if (cl_mem_image == NULL) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_IMAGE_SIZE:
                printf("Error: CL_INVALID_IMAGE_SIZE\n");
                ec = WebCLException::INVALID_IMAGE_SIZE;
                break;
            case CL_INVALID_HOST_PTR:
                printf("Error: CL_INVALID_HOST_PTR\n");
                ec = WebCLException::INVALID_HOST_PTR;
                break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
                ec = WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
                break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
                ec = WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLBuffer(int flags, WebGLBuffer* bufobj, ExceptionCode& ec)
{
    cl_mem cl_mem_id = NULL;
    cl_int err = 0;
    Platform3DObject buf_id = 0;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if (bufobj != NULL) {
        buf_id = bufobj->object();
        if (buf_id == 0) {
            printf("Error: buf_id null\n");
            ec = WebCLException::FAILURE;
            return NULL;
        }
    }
    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_READ_ONLY, buf_id, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_WRITE_ONLY, buf_id, &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_READ_WRITE, buf_id, &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_USE_HOST_PTR, buf_id, &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, buf_id, &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_id = clCreateFromGLBuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, buf_id, &err);
            break;

    }
    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_GL_OBJECT:
                printf("Error: CL_INVALID_GL_OBJECT\n");
                ec = WebCLException::INVALID_GL_OBJECT;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
        m_mem_list.append(o);
        m_num_mems++;
        return o;

    }
    return NULL;
}
PassRefPtr<WebCLImage> WebCLContext::createFromGLRenderBuffer(int flags, WebGLRenderbuffer* renderbufferobj, ExceptionCode& ec)
{
    cl_mem cl_mem_id = NULL;
    cl_int err = 0;
    GLuint rbuf_id = 0;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::INVALID_CONTEXT;
        return NULL;
    }
    if (renderbufferobj != NULL) {
        rbuf_id =  renderbufferobj->getInternalFormat();  

    }
    switch (flags) {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_READ_ONLY, rbuf_id, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_WRITE_ONLY, rbuf_id, &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_READ_WRITE, rbuf_id, &err);
            break;
        case WebCL::MEM_USE_HOST_PTR:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_USE_HOST_PTR, rbuf_id, &err);
            break;
        case WebCL::MEM_ALLOC_HOST_PTR:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, rbuf_id, &err);
            break;
        case WebCL::MEM_COPY_HOST_PTR:
            cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, rbuf_id, &err);
            break;

    }
    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_GL_OBJECT:
                printf("Error: CL_INVALID_GL_OBJECT\n");
                ec = WebCLException::INVALID_GL_OBJECT;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLImage> o = WebCLImage::create(m_context, cl_mem_id, true);
        m_img_list.append(o);
        m_num_images++;
        return o;

    }
    return NULL;

}

PassRefPtr<WebCLSampler> WebCLContext::createSampler(bool norm_cords, 
        int addr_mode, int fltr_mode, ExceptionCode& ec)
{


    printf(" addr_mode = %d , norm_cords = %d , fltr_mode = %d " ,addr_mode,norm_cords,fltr_mode);
    cl_int err = 0;
    cl_bool normalized_coords = CL_FALSE;
    cl_addressing_mode addressing_mode = CL_ADDRESS_NONE;
    cl_filter_mode filter_mode = CL_FILTER_NEAREST;
    cl_sampler cl_sampler_id = NULL;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    if(norm_cords)
        normalized_coords = CL_TRUE;
    switch(addr_mode)
    {
        case WebCL::ADDRESS_NONE:
            addressing_mode = CL_ADDRESS_NONE;
            printf(" came to ADDRESS_NONE  ");
            break;
        case WebCL::ADDRESS_CLAMP_TO_EDGE:
            addressing_mode = CL_ADDRESS_CLAMP_TO_EDGE;
            break;
        case WebCL::ADDRESS_CLAMP:
            addressing_mode = CL_ADDRESS_CLAMP;
            break;
        case WebCL::ADDRESS_REPEAT: 
            addressing_mode = CL_ADDRESS_REPEAT;
            break;
        default:
            printf("Error: Invaild Addressing Mode\n");
            ec = WebCLException::FAILURE;
            return NULL;
    }
    switch(fltr_mode)
    {
        case WebCL::FILTER_LINEAR:
            filter_mode = CL_FILTER_LINEAR;
            break;
        case WebCL::FILTER_NEAREST :
            filter_mode = CL_FILTER_NEAREST ;
            printf(" came to FILTER_NEARES ");
            break;
        default:
            printf("Error: Invaild Filtering Mode\n");
            ec = WebCLException::FAILURE;
            return NULL;
    }
    cl_sampler_id = clCreateSampler(m_cl_context, normalized_coords, addressing_mode, 
            filter_mode, &err);

    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT \n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE \n");
                ec = WebCLException::INVALID_VALUE;
                break;
            case CL_INVALID_OPERATION :
                printf("Error: CL_INVALID_OPERATION   \n");
                ec = WebCLException::INVALID_OPERATION  ;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY \n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;

            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }

    } else {
        RefPtr<WebCLSampler> o = WebCLSampler::create(m_context, cl_sampler_id);
        if (o != NULL) {
            m_sampler_list.append(o);
            m_num_samplers++;
            return o;
        } else {
            ec = WebCLException::FAILURE;
            return NULL;
        }
    }
    return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLTexture2D(int flags, 
        GC3Denum texture_target, GC3Dint miplevel, GC3Duint texture, ExceptionCode& ec)
{
    cl_int err = 0;
    cl_mem cl_mem_id = NULL;

    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return NULL;
    }
    switch (flags)
    {
        case WebCL::MEM_READ_ONLY:
            cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_ONLY, 
                    texture_target, miplevel, texture, &err);
            break;
        case WebCL::MEM_WRITE_ONLY:
            cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_WRITE_ONLY,
                    texture_target, miplevel, texture, &err);
            break;
        case WebCL::MEM_READ_WRITE:
            cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_WRITE,
                    texture_target, miplevel, texture, &err);
            break;
    }
    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_VALUE:
                printf("Error: CL_INVALID_VALUE\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_INVALID_MIP_LEVEL:
                printf("Error: CL_INVALID_MIP_LEVEL\n");
                ec = WebCLException::INVALID_MIP_LEVEL;
                break;
            case CL_INVALID_GL_OBJECT  :
                printf("Error: CL_INVALID_GL_OBJECT\n");
                ec = WebCLException::INVALID_GL_OBJECT;
                break;

            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
                ec = WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
                break;
            case CL_INVALID_OPERATION:
                printf("Error: CL_INVALID_OPERATION\n");
                ec = WebCLException::INVALID_OPERATION;
                break;
            case CL_OUT_OF_RESOURCES:
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY:
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
        m_mem_list.append(o);
        m_num_mems++;
        return o;
    }
    return NULL;
}

PassRefPtr<WebCLEvent> WebCLContext::createUserEvent( ExceptionCode& ec)
{
    /*    
          cl_int err = -1;	
          cl_event event = NULL;
          if (m_cl_context == NULL) {
          printf("Error: Invalid CL Context\n");
          ec = WebCLException::FAILURE;
          return NULL;
          }


    //printf(" inside createUserEvent call before clCreateUserEvent ");
    //(TODO) To be uncommented for OpenCL1.1
    event =  clCreateUserEvent(m_cl_context, &err);
    //printf(" inside createUserEvent call after clCreateUserEvent ");
    if (err != CL_SUCCESS) {
    switch (err) {
    case CL_INVALID_CONTEXT :
    printf("Error: CL_INVALID_CONTEXT \n");
    ec = WebCLException::INVALID_CONTEXT;
    break;
    case CL_OUT_OF_RESOURCES :
    printf("Error: CCL_OUT_OF_RESOURCES \n");
    ec = WebCLException::OUT_OF_RESOURCES;
    break;
    case CL_OUT_OF_HOST_MEMORY :
    printf("Error: CCL_OUT_OF_HOST_MEMORY \n");
    ec = WebCLException::OUT_OF_HOST_MEMORY;
    break;
    default:
    printf("Error: Invaild Error Type\n");
    ec = WebCLException::FAILURE;
    break;
    }

    } else {
    RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, event);
    m_event_list.append(o);
    m_num_events++;
    return o;
    }
    */
    UNUSED_PARAM(ec);
    return NULL;
}

void WebCLContext::releaseCL( ExceptionCode& ec)
{
    cl_int err = 0;
    if (m_cl_context == NULL) {
        printf("Error: Invalid CL Context\n");
        ec = WebCLException::FAILURE;
        return;
    }
    err = clReleaseContext(m_cl_context);
    if (err != CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT :
                printf("Error: CL_INVALID_CONTEXT\n");
                ec = WebCLException::INVALID_CONTEXT;
                break;
            case CL_OUT_OF_RESOURCES  :
                printf("Error: CL_OUT_OF_RESOURCES\n");
                ec = WebCLException::OUT_OF_RESOURCES;
                break;
            case CL_OUT_OF_HOST_MEMORY  :
                printf("Error: CL_OUT_OF_HOST_MEMORY\n");
                ec = WebCLException::OUT_OF_HOST_MEMORY;
                break;
            default:
                printf("Error: Invaild Error Type\n");
                ec = WebCLException::FAILURE;
                break;
        }
    } else {
        m_context = NULL;
        return;
    }
    return;
}

void WebCLContext::setDevice(RefPtr<WebCLDevice> device_id)
{
    m_device_id = device_id;
}
cl_context WebCLContext::getCLContext()
{
    return m_cl_context;
}
PassRefPtr<Image> WebCLContext::videoFrameToImage(HTMLVideoElement* video)
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

    WebCLContext::LRUImageBufferCache::LRUImageBufferCache(int capacity)
    : m_buffers(adoptArrayPtr(new OwnPtr<ImageBuffer>[capacity]))
      , m_capacity(capacity)
{
}

ImageBuffer* WebCLContext::LRUImageBufferCache::imageBuffer(const IntSize& size)
{
    int i;
    for (i = 0; i < m_capacity; ++i) {
        ImageBuffer* buf = m_buffers[i].get();
        if (!buf)
            break;
        // TODO siba check this call
        //if (buf->size() != size)
        //	continue;
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

void WebCLContext::LRUImageBufferCache::bubbleToFront(int idx)
{
    for (int i = idx; i > 0; --i)
        m_buffers[i].swap(m_buffers[i-1]);
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
