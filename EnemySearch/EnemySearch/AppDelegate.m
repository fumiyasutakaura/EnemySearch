//
//  AppDelegate.m
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 12/23/13.
//  Copyright (c) 2013 Fumiyasu Takaura. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)dealloc
{
    [self.window setContentView:nil];
    [tVC release];
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    tVC = [[TestViewController alloc] initWithNibName:@"TestViewController" bundle:nil];
    [self.window setContentView:tVC.view];
}

@end
