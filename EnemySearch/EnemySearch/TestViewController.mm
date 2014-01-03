//
//  TestViewController.m
//  EnemySearch
//
//  Created by Fumiyasu Takaura on 1/1/14.
//  Copyright (c) 2014 Fumiyasu Takaura. All rights reserved.
//

#import "TestViewController.h"

#include "Test.hpp"
static boost::shared_ptr<Test> test;

@interface TestViewController ()

@end

@implementation TestViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
        test = Test::Create();
    }
    
    return self;
}

-(void)dealloc {
    [self.session removeOutput:self.dataOutput];
    [imageView removeFromSuperview];
    [imageView release];
    [super dealloc];
}

- (void)loadView
{
    [super loadView];
    
    imageView = [[NSImageView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];
    [self.view addSubview:imageView];
    
    AVCaptureDevice* device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    AVCaptureDeviceInput* deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:device error:NULL];
    
    NSDictionary* settings = @{(id)kCVPixelBufferPixelFormatTypeKey:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA]};
    self.dataOutput = [[[AVCaptureVideoDataOutput alloc] init] autorelease];
    self.dataOutput.videoSettings = settings;
    [self.dataOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
    
    self.session = [[[AVCaptureSession alloc] init] autorelease];
    [self.session addInput:deviceInput];
    [self.session addOutput:self.dataOutput];
    self.session.sessionPreset = AVCaptureSessionPresetHigh;
    
    AVCaptureConnection *videoConnection = NULL;
    
    [self.session beginConfiguration];
    
    for ( AVCaptureConnection *connection in [self.dataOutput connections] )
    {
        for ( AVCaptureInputPort *port in [connection inputPorts] )
        {
            if ( [[port mediaType] isEqual:AVMediaTypeVideo] )
            {
                videoConnection = connection;
                
            }
        }
    }
    if([videoConnection isVideoOrientationSupported]) // Here it is, its always false
    {
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
    }
    
    [self.session commitConfiguration];

    [self.session startRunning];
    
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    CVImageBufferRef buffer;
    buffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferLockBaseAddress(buffer, 0); {
        
        unsigned char* base;
        size_t width, height, bytesPerRow;
        base = (unsigned char*)CVPixelBufferGetBaseAddress(buffer);
        width = CVPixelBufferGetWidth(buffer);
        height = CVPixelBufferGetHeight(buffer);
        bytesPerRow = CVPixelBufferGetBytesPerRow(buffer);
        
        CGColorSpaceRef colorSpace;
        CGContextRef cgContext;
        colorSpace = CGColorSpaceCreateDeviceRGB();
        cgContext = CGBitmapContextCreate(base, width, height, 8, bytesPerRow, colorSpace,
                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
        { // base -> kernel
            test->pushData(base, (int)(width*height*4));
        }
        { // run kernel
            test->run();
        }
        { // kernel -> base
            test->getResult(base);
        }
        CGColorSpaceRelease(colorSpace);
        
        CGImageRef  cgImage;
        cgImage = CGBitmapContextCreateImage(cgContext);
        NSImage* img = [[NSImage alloc] initWithCGImage:cgImage size:CGSizeMake(width, height)];
        [imageView setImage:img];
        [img release];
        CGImageRelease(cgImage);
        CGContextRelease(cgContext);
        
    } CVPixelBufferUnlockBaseAddress(buffer, 0);
    
}


@end
