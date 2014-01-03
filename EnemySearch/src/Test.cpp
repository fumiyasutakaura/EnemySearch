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
#include <deque>
#include <OpenCL/OpenCL.h>

static const int screenWidth = 640;
static const int screenHeight = 480;
static const unsigned char colorPerPixel = 4;

static const int maxDequeSize = 10;


class TestImpl : public Test {
public:
    TestImpl();
    ~TestImpl();
    
    void init();
    
    // > virtual method ------------------
    void run();
    void pushData( const unsigned char* pixel, const int num_of_pixels );
    void getResult( unsigned char* retPixel );
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
    cl_mem memScreenWidth;
    cl_mem memColorPerPixel;
    const int bufferSize;
    
    const int max_deque_size;
    std::deque<boost::shared_array<unsigned char> > dataDeq;
};

TestImpl::TestImpl() : bufferSize(screenWidth*screenHeight*colorPerPixel), max_deque_size(maxDequeSize) {
    
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
    succeeded = clReleaseMemObject(memScreenWidth);
    succeeded = clReleaseMemObject(memColorPerPixel);
    
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
        kernel = clCreateKernel(program, "blendPic", &ret);
    }
    
    {
        memObj0 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*bufferSize, NULL, &ret);
        memObj1 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned char)*bufferSize, NULL, &ret);
        memObjResult = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(unsigned char)*bufferSize, NULL, &ret);
        memScreenWidth = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
        memColorPerPixel = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
    }
    
    { 
        
        clEnqueueWriteBuffer(commandQueue, memScreenWidth, CL_TRUE, 0, sizeof(int), &screenWidth, 0, NULL, NULL);
        clEnqueueWriteBuffer(commandQueue, memColorPerPixel, CL_TRUE, 0, sizeof(unsigned char), &colorPerPixel, 0, NULL, NULL);
    }
    
}

void TestImpl::pushData( const unsigned char* pixel, const int num_of_pixels ) {
    static boost::shared_array<unsigned char> temp( new unsigned char[num_of_pixels]() );
    memcpy(temp.get(), pixel, sizeof(unsigned char)*num_of_pixels);
    dataDeq.push_back(temp);
    while ( dataDeq.size() < max_deque_size ) {
        boost::shared_array<unsigned char> temp2( new unsigned char[num_of_pixels]() );
        memcpy(temp2.get(), pixel, sizeof(unsigned char)*num_of_pixels);
        dataDeq.push_back(temp2);
    }
    while( dataDeq.size() > max_deque_size ) {
        dataDeq.pop_front();
    }
}

void TestImpl::run() {
    cl_int ret;
    
    
    size_t global_work_size[3] = {(size_t)bufferSize, 1, 1};
    size_t local_work_size[3]  = {1, 1, 1};
    static boost::shared_array<unsigned char> retPixel( new unsigned char[bufferSize]() );
    cl_event events[10];
    {
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memObjResult);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memScreenWidth);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memColorPerPixel);
        
        clEnqueueWriteBuffer(commandQueue, memObj0, CL_TRUE, 0, sizeof(unsigned char)*bufferSize, dataDeq[0].get(), 0, NULL, NULL);
        clEnqueueWriteBuffer(commandQueue, memObj1, CL_TRUE, 0, sizeof(unsigned char)*bufferSize, dataDeq[1].get(), 0, NULL, NULL);
        ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&memObj0);
        ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&memObj1);
        
        ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
        
//        ret = clEnqueueReadBuffer(commandQueue, memObjResult, CL_TRUE, 0, bufferSize * sizeof(unsigned char), retPixel.get(), 0, NULL, NULL);
        clEnqueueCopyBuffer(commandQueue, memObjResult, memObj1, 0, 0, bufferSize * sizeof(unsigned char), 0, NULL, &events[0]);
    }
//    for( int i=2; i<3; ++i ) {
//        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memObjResult);
//        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memScreenWidth);
//        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memColorPerPixel);
//        
//        clEnqueueWriteBuffer(commandQueue, memObj0, CL_TRUE, 0, sizeof(unsigned char)*bufferSize, dataDeq[i].get(), 0, NULL, NULL);
//        ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&memObj0);
//        ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&memObj1);
//        
//        ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
//        
//        //        ret = clEnqueueReadBuffer(commandQueue, memObjResult, CL_TRUE, 0, bufferSize * sizeof(unsigned char), retPixel.get(), 0, NULL, NULL);
//        clEnqueueCopyBuffer(commandQueue, memObjResult, memObj1, 0, 0, bufferSize * sizeof(unsigned char), 0, &events[0], NULL);
//    }
    
}

void TestImpl::getResult( unsigned char* retPixel ) {
    cl_int ret;
    ret = clEnqueueReadBuffer(commandQueue, memObjResult, CL_TRUE, 0, bufferSize * sizeof(unsigned char), retPixel, 0, NULL, NULL);
    
}
