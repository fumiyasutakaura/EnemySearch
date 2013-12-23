//
//  Test.h
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#ifndef __EnemySearch__Test__
#define __EnemySearch__Test__

#include <boost/shared_ptr.hpp>

class Test {
public:
    static boost::shared_ptr<Test> Create();
    virtual void run() = 0;
    virtual void showResult() = 0;
    
protected:
    explicit Test() {}
};

#endif /* defined(__EnemySearch__Test__) */
