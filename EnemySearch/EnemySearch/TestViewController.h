//
//  TestViewController.h
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 1/1/14.
//  Copyright (c) 2014 Fumiyasu Takaura. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>

@interface TestViewController : NSViewController<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    NSImageView* imageView;
}
@property (nonatomic, strong) AVCaptureSession* session;
@property (nonatomic, strong) AVCaptureVideoDataOutput* dataOutput;

@end
