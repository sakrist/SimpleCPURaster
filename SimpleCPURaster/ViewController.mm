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

- (GLKMatrix4)projectionMatrix {
    float nearZ = 0.1; 
    float farZ = 1000;
    float fovAngle = GLKMathDegreesToRadians(25);
    float aspect = fabs(self.view.frame.size.width / self.view.frame.size.height);
    float nearHeight = 2.0 * nearZ * tanf(fovAngle/2.0);
    float nearWidth = nearHeight * aspect;    
    float left = -nearWidth * 0.5;
    float right = nearWidth * 0.5;
    float top = nearHeight * 0.5;
    float bottom = -nearHeight * 0.5;
    
    return GLKMatrix4MakeFrustum(left, right, bottom, top, nearZ, farZ);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _raster = new Raster( [](Vec3f vertex, Matrix44f projection) -> Vec3f {
        Vec3f vertexOut = projection * vertex;
        return vertexOut;
    } , [](Vec3f vertex) -> Vec3<unsigned char> {
//        Vec3f n = vertex.normalize();
        return Vec3c(0);
    });

    
    _rotation = 0;
    
    _proj = {0.707107, -0.331295, 0.624695, 0, 
        0, 0.883452, 0.468521, 0, 
        -0.707107, -0.331295, 0.624695, 0, 
        -1.63871, -5.747777, -40.400412, 1}; 
    
    _projection = [self projectionMatrix];
    _viewMatrix = GLKMatrix4MakeTranslation(0, 0, 50);
    _mvp = GLKMatrix4Multiply(_projection, _viewMatrix);
    memcpy(&_raster->projection, _mvp.m, sizeof(GLKMatrix4));
    
    
    [self resize];
    [self draw];
//    GLKMatrix4 i = GLKMatrix4Identity;
//    memcpy(&_proj, i.m, sizeof(GLKMatrix4));
    
    raster(_frame, _proj);
    
    
    
    NSImage *image = [[NSImage alloc] initWithFramebuffer:_frame];
    [self.imageView setImage:image];
}

- (void)viewDidLayout{
    [super viewDidLayout];
    [self resize];
    [self draw];
    
    
    _rotation += 0.05;
//    
    GLKMatrix4 viewMatrix = GLKMatrix4Rotate(_viewMatrix, _rotation, 0, 1, 0);
    _mvp = GLKMatrix4Multiply(_projection, viewMatrix);
    memcpy(&_raster->projection, _mvp.m, sizeof(GLKMatrix4));
    
//    
//    raster(_frame, _proj);
//    NSImage *image = [[NSImage alloc] initWithFramebuffer:_frame];
//    [self.imageView setImage:image];
    
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
