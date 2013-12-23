//
//  AppDelegate.m
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#import "AppDelegate.h"
#include "Test.hpp"

@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    boost::shared_ptr<Test> test = Test::Create();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
    test->run();
    test->showResult();
}

@end
