//
//  TestViewController.m
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 1/1/14.
//  Copyright (c) 2014 Fumiyasu Takaura. All rights reserved.
//

#import "TestViewController.h"
#include "Test.hpp"

@interface TestViewController ()

@end

@implementation TestViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)test {
    boost::shared_ptr<Test> test = Test::Create();
    test->run();
    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();
    //    test->run();
    //    test->showResult();

}

@end
