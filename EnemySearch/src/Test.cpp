//
//  Test.cpp
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#include "Test.hpp"
#include <OpenCL/OpenCL.h>

class TestImpl : public Test {
public:
    static boost::shared_ptr<Test> Create();
    
    // > virtual method ------------------
    void run();
    // < virtual method ------------------
    
private:
    void init();
};


boost::shared_ptr<Test> TestImpl::Create() {
    boost::shared_ptr<TestImpl> ret( new TestImpl() );
    ret->init();
    return ret;
}

void TestImpl::init() {
    
}

void TestImpl::run() {
    
}