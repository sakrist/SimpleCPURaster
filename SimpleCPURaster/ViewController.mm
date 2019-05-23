//
//  ViewController.m
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 13/04/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>

#include "raster.hpp"
#include "framebuffer.hpp"
#include "cow.h"

@interface NSImage (Framebuffer)
- (instancetype) initWithFramebuffer:(Framebuffer *)framebuffer;
@end

void raster(Framebuffer *framebuffer, Matrix44f proj);

@implementation ViewController {
    Framebuffer *_frame;
    
    Raster *_raster;
    
    GLKMatrix4 _projection;
    GLKMatrix4 _viewMatrix;
    GLKMatrix4 _mvp;
    
    Matrix44f _proj;
    
    float _rotation;
}

- (Matrix44f)projectionMatrix {
    float nearZ = 0.1; 
    float farZ = 1000;
    float fovAngle = 65.0f * (M_PI / 180.0f);
    float aspect = fabs(self.view.frame.size.width / self.view.frame.size.height);
     
    Matrix44f matrix = Matrix44f().perspective(fovAngle, aspect, nearZ, farZ);
    return matrix;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _raster = new Raster( [](Vec3f vertex, Matrix44f projection) -> Vec3f {
        Vec3f vertexOut = projection * vertex;
        return vertexOut;
    } , [](Vec3f vertex) -> Vec3f {
        Vec3f n = vertex.normalize();        
        return n;
    });

}

- (void)viewDidLayout{
    [super viewDidLayout];
    [self resize];
    Matrix44f projection = [self projectionMatrix];
    Matrix44f mvp = projection * Matrix44f().translation(0, 0, -4);
    _raster->projection = mvp;

    [self draw];
    
}

- (void)resize {
    if (_frame && !CGSizeEqualToSize(self.view.frame.size, CGSizeMake(_frame->getSize().x, _frame->getSize().y))) {
        delete _frame;
        _frame = NULL;
    }
    
    if (!_frame) {
        _frame = new Framebuffer(Vec2i(self.view.frame.size.width, self.view.frame.size.height));
    }
    
    if (_raster) {
        _raster->setFramebuffer(_frame);
    }
}

- (void)draw {
//    _raster->draw(vertices, nvertices, 3156);
    
    _raster->draw(cubeVertices, cubeIndices, 12);
    
    [self present];
}

- (void)present {
    NSImage *image = [[NSImage alloc] initWithFramebuffer:_frame];
    [self.imageView setImage:image];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end


@implementation NSImage (Framebuffer)

- (instancetype)initWithFramebuffer:(Framebuffer *)framebuffer {
    int width = framebuffer->getSize().x;
    int height = framebuffer->getSize().y;
    int channel = 3;
    
    size_t bufferLength = width * height * channel;
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, (void *)framebuffer->getColorbuffer(), bufferLength, NULL);
    size_t bitsPerComponent = 8;
    size_t bitsPerPixel = bitsPerComponent * channel;
    size_t bytesPerRow = channel * width;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
    if(channel < 4) {
        bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaNone;
    }
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    CGImageRef iref = CGImageCreate(width, 
                                    height, 
                                    bitsPerComponent, 
                                    bitsPerPixel, 
                                    bytesPerRow, 
                                    colorSpaceRef, 
                                    bitmapInfo, 
                                    provider,   // data provider
                                    NULL,       // decode
                                    YES,        // should interpolate
                                    renderingIntent);
        
    self = [self initWithCGImage:iref size:NSMakeSize(width, height)];
    CGImageRelease(iref);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpaceRef);
    return self;
}


@end
