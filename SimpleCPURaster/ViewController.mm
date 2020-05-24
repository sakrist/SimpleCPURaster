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
#include "Framebuffer.hpp"
#include "SimplePipeline.hpp"

@interface NSImage (Framebuffer)
- (instancetype) initWithFramebuffer:(Framebuffer *)framebuffer;
@end

void raster(Framebuffer *framebuffer, mat4 proj);

@implementation ViewController {
    std::shared_ptr<Framebuffer> _frame;
    std::shared_ptr<Raster> _raster;
    std::shared_ptr<SimplePipeline> _pipeline;
    
    Resource _bunny;
        
    float _rotation;
    
    CGPoint _lastPoint;
    float pitch;
    float yaw;
}

- (mat4)projectionMatrix {
    float nearZ = 0.1; 
    float farZ = 1000;
    float fovAngle = 65.0f * (M_PI / 180.0f);
    float aspect = fabs(self.view.frame.size.width / self.view.frame.size.height);
     
    mat4 matrix = glm::perspective(fovAngle, aspect, nearZ, farZ);
    return matrix;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    ((View *)self.view).pointerDelegate = self;
    [((View *)self.view) activeTrackingArea];
    [self loadBunny];
    
    _raster = std::make_shared<Raster>();
    _pipeline = std::make_shared<SimplePipeline>();

    _raster->setPipeline(_pipeline);
}

- (void)viewDidLayout{
    [super viewDidLayout];
    [self resize];
    mat4 projection = [self projectionMatrix];
    mat4 mvp = translate(projection, vec3(0, 0, -30));
    mvp = glm::rotate(mvp, pitch, vec3(1, 0, 0));
    mvp = glm::rotate(mvp, yaw, vec3(0, 1, 0));
    _pipeline->projection = mvp;
    [self draw];
    
}

- (void)resize {
    if (_frame && !CGSizeEqualToSize(self.view.frame.size, CGSizeMake(_frame->getSize().x, _frame->getSize().y))) {
        _frame = nullptr;
    }
    
    if (!_frame) {
        _frame = std::make_shared<Framebuffer>(ivec2(self.view.frame.size.width, self.view.frame.size.height));
    }
    
    if (_raster) {
        _raster->setFramebuffer(_frame);
    }
}

- (void)draw {
    
    NSDate *date = [NSDate date];

    _raster->draw(_bunny);
    
    NSLog(@"draw: %f ms", -1000 * [date timeIntervalSinceNow]);
    
    [self present];
}

- (void)present {
    NSImage *image = [[NSImage alloc] initWithFramebuffer:_frame.get()];
    [self.imageView setImage:image];
}
    
-(void)mouseDown:(CGPoint)point {
    _lastPoint = point;
}
    
-(void)mouseDragged:(CGPoint)point {
    CGSize size = self.view.frame.size;
    float x = (point.x - _lastPoint.x) * size.height/size.width*0.5;
    float y = (point.y - _lastPoint.y) * (size.width/size.height*0.5);

    pitch +=  -degreesToRadians(y);
    yaw += degreesToRadians(x);
    
    mat4 projection = [self projectionMatrix];
    mat4 mvp = translate(projection, vec3(0, 0, -30));
    mvp = glm::rotate(mvp, pitch, vec3(1, 0, 0));
    mvp = glm::rotate(mvp, yaw, vec3(0, 1, 0));
    _pipeline->projection = mvp;
    
    _lastPoint = point;
    
    NSDate *date = [NSDate date];
    _raster->clear();
    NSLog(@"clear: %f ms", -1000 * [date timeIntervalSinceNow]);
    [self draw];
}
    
-(void)mouseUp:(CGPoint)point {
    
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)loadBunny {
    NSData *data = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"bunny" ofType:@"bin"]];
    
    const char *bytes = (const char *)data.bytes;
    size_t length = data.length;
    
    int bytesOffset = 0;
    
    glm::vec3 min_vec;
    std::memcpy(&min_vec, bytes + bytesOffset, sizeof(min_vec));
    bytesOffset += sizeof(min_vec);
    
    glm::vec3 max_vec;
    std::memcpy(&max_vec, bytes + bytesOffset, sizeof(max_vec));
    bytesOffset += sizeof(max_vec);
    
    // getting center
    glm::vec3 center = (max_vec + min_vec) * glm::vec3(0.5);
    
    int icount = 0;
    std::memcpy(&icount, bytes + bytesOffset, sizeof(icount));
    bytesOffset += sizeof(icount);
        
    NSLog(@"indices count %d", icount);
    const void *indicesData = bytes + bytesOffset;
    
    bytesOffset += sizeof(uint32_t) * icount;
    
    int vcount = 0;
    std::memcpy(&vcount, bytes + bytesOffset, sizeof(vcount));
    bytesOffset += sizeof(vcount);
    
    const void *vertices = bytes + bytesOffset;
    
    // 3 position + 3 normal 
    size_t verticesSize = vcount*(3+3);
    
    
    NSLog(@"vertices count %d", vcount);
    
    glm::vec3 *v = (glm::vec3 *)vertices;
    int all_vn = vcount*2;
    for (int i = 0; i < all_vn; i += 2) {
        v[i] = (v[i] - center) * glm::vec3(0.05);
    }
    
    _bunny.setIndices(icount, (uint32_t *)indicesData);
    _bunny.setAttributesBuffer((float *)vertices, (uint32_t)verticesSize);
    _bunny.setAttribute(PositionsAttribute, 6);
    _bunny.setAttribute(NormalsAttribute, 6, 3);
    
}

@end


@implementation NSImage (Framebuffer)

- (instancetype)initWithFramebuffer:(Framebuffer *)framebuffer {
    if (framebuffer == nullptr) {
        return nil;
    }
    int width = framebuffer->getSize().x;
    int height = framebuffer->getSize().y;
    int channel = 3;
    
    size_t bufferLength = width * height * channel;
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, framebuffer->getColorbuffer(), bufferLength, NULL);
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
