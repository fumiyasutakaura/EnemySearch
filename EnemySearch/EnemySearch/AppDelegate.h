//
//  AppDelegate.h
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TestViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    TestViewController *tVC;
}
@property (assign) IBOutlet NSWindow *window;

@end
