//
//  Test.cpp
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#include "Test.hpp"
#include "File.hpp"
#include <boost/shared_array.hpp>
#include <fstream>
#include <OpenCL/OpenCL.h>

class TestImpl : public Test {
public:
    TestImpl();
    ~TestImpl();
    
    void init();
    
    // > virtual method ------------------
    void run();
    void showResult();
    // < virtual method ------------------
    
private:
    cl_device_id deviceID;
    cl_context context;
    cl_command_queue commandQueue;
    cl_program program;
    cl_kernel kernel;
    cl_mem memObj0;
    cl_mem memObj1;
    cl_mem memObjResult;
    const int bufferSize;
};

TestImpl::TestImpl() : bufferSize(128) {
    
}
TestImpl::~TestImpl() {
    cl_int succeeded;
    succeeded = clFlush(commandQueue);
    succeeded = clFinish(commandQueue);
    succeeded = clReleaseKernel(kernel);
    succeeded = clReleaseProgram(program);
    succeeded = clReleaseMemObject(memObj0);
    succeeded = clReleaseMemObject(memObj1);
    succeeded = clReleaseMemObject(memObjResult);
    succeeded = clReleaseCommandQueue(commandQueue);
    succeeded = clReleaseContext(context);
}

boost::shared_ptr<Test> Test::Create() {
    boost::shared_ptr<TestImpl> ret( new TestImpl() );
    ret->init();
    return ret;
}

static cl_device_id getDeviceID() {
    cl_device_id ret;
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &ret, &ret_num_devices);
    if( ret ) {
        return ret;
    }
    return NULL;
}

void TestImpl::init() {
    
    cl_int ret;
    
    {
        deviceID = getDeviceID();
    }
    
    {
        context = clCreateContext( NULL, 1, &deviceID, NULL, NULL, &ret);
    }
    
    {
        commandQueue = clCreateCommandQueue(context, deviceID, 0, &ret);
    }
    
    {
        char *source_str;
        size_t source_size;
        {
            FILE *fp = fopen(GetFilePath("blend.kernel").c_str(), "r");
            if (!fp) {
                fprintf(stderr, "Failed to load kernel.\n");
                exit(1);
            }
            const int max_source_size = 0x100000;
            source_str = (char*)malloc(max_source_size);
            source_size = fread( source_str, 1, max_source_size, fp);
            fclose( fp );
        }
        program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                            (const size_t *)&source_size, &ret);
        ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);
    }
    
    {
        kernel = clCreateKernel(program, "vec_add", &ret);
    }
    
    {
        memObj0 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*bufferSize, NULL, &ret);
        memObj1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*bufferSize, NULL, &ret);
        memObjResult = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*bufferSize, NULL, &ret);
    }
    
    {   
        float val0[bufferSize];
        float val1[bufferSize];
        for( int i=0; i<bufferSize; ++i ) {
            val0[i] = i;
            val1[i] = i;
        }
        clEnqueueWriteBuffer(commandQueue, memObj0, CL_TRUE, 0, bufferSize * sizeof(float), val0, 0, NULL, NULL);
        clEnqueueWriteBuffer(commandQueue, memObj1, CL_TRUE, 0, bufferSize * sizeof(float), val1, 0, NULL, NULL);
    }
    
}

void TestImpl::run() {
    cl_int ret;
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memObj0);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memObj1);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memObjResult);
    ret = clEnqueueTask(commandQueue, kernel, 0, NULL,NULL);
}

void TestImpl::showResult() {
    cl_int ret;
    float result[bufferSize];
    ret = clEnqueueReadBuffer(commandQueue, memObjResult, CL_TRUE, 0, bufferSize * sizeof(float), result, 0, NULL, NULL);
    for( unsigned int i=0; i<bufferSize; ++i ) {
        std::cout << result[i] << std::endl;
    }
}
