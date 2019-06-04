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
#include "SimplePipeline.hpp"

@interface NSImage (Framebuffer)
- (instancetype) initWithFramebuffer:(Framebuffer *)framebuffer;
@end

void raster(Framebuffer *framebuffer, Matrix44f proj);

@implementation ViewController {
    Framebuffer *_frame;
    Raster *_raster;
    SimplePipeline *_pipeline;
    Resource _cube;
    Resource _cow;
    
    Resource _bunny;
    
    NSMutableData *indicesData;
    NSMutableData *verticesData;
    
    float _rotation;
    
    CGPoint _lastPoint;
    float pitch;
    float yaw;
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
    
    ((View *)self.view).pointerDelegate = self;
    [((View *)self.view) activeTrackingArea];
    [self loadBunny];
    
    _cube.position.vector = cubeVertices;
    _cube.indices = cubeIndices;
    _cube.indicesCount = 12*3;
    
    _cow.position.vector = vertices;
    _cow.indices = nvertices;
    _cow.indicesCount = 3156*3;
    
    _raster = new Raster(); 
    _pipeline = new SimplePipeline( [](Vec3f vertex, Matrix44f projection) -> Vec3f {
        Vec3f vertexOut = projection * vertex;
        return vertexOut;
    } , [](Vec3f vertex, Vec3f normal) -> Vec3f {
        
        Vec3f diffuse = Vec3f(0.56f);
        Vec3f lightDirection(1, 1, 1);
        Vec3f color = fmax(normal.dotProduct(lightDirection), 0.0) * diffuse;
        color = clamp(color, 0.0, 1.0);
//        Vec3f n = vertex.normalize();        
        return color ;
    });

    _raster->setPipeline(_pipeline);
}

- (void)viewDidLayout{
    [super viewDidLayout];
    [self resize];
    Matrix44f projection = [self projectionMatrix];
    Matrix44f mvp = projection * (Matrix44f().translation(0, 0, -30) * (Matrix44f().xRotation(pitch) * Matrix44f().yRotation(yaw)));
    _pipeline->projection = mvp;

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
    
    NSDate *date = [NSDate date];
    
//    _raster->draw(&_cow);
//    _raster->draw(&_cube);
    _raster->draw(&_bunny);
    
    NSLog(@"draw: %f ms", -1000 * [date timeIntervalSinceNow]);
    
    [self present];
}

- (void)present {
    NSImage *image = [[NSImage alloc] initWithFramebuffer:_frame];
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
    
    Matrix44f projection = [self projectionMatrix];
    Matrix44f mvp = projection * (Matrix44f().translation(0, 0, -30) * (Matrix44f().xRotation(pitch) * Matrix44f().yRotation(yaw)));
    _pipeline->projection = mvp;
    
    _lastPoint = point;
    
    _raster->clear();
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
    
    int bytesOffset = 0;
    
    GLKVector3 min_vec;
    [data getBytes:&min_vec range:NSMakeRange(bytesOffset, 4*3)];
    bytesOffset += sizeof(min_vec);
    GLKVector3 max_vec;
    [data getBytes:&max_vec range:NSMakeRange(bytesOffset, 4*3)];
    bytesOffset += sizeof(max_vec);
    
    // getting center
    GLKVector3 center = GLKVector3MultiplyScalar(GLKVector3Add(max_vec, min_vec), 0.5);
    
    int icount = 0;
    [data getBytes:&icount range:NSMakeRange(bytesOffset, 4)];
    bytesOffset += 4;
    
//    _indexCount = icount;
    
    NSLog(@"indices count %d", icount);
    indicesData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(bytesOffset,  sizeof(GLuint) * icount)]];
    
    bytesOffset += sizeof(GLuint) * icount;
    
    int vcount = 0;
    [data getBytes:&vcount range:NSMakeRange(bytesOffset, 4)];
    bytesOffset += 4;
    
    verticesData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(bytesOffset, vcount*(3+3)*4)]];
    
    bytesOffset += vcount*(3+3)*4; // end
    
    NSLog(@"vertices count %d", vcount);
    
    GLKVector3 *v = (GLKVector3*)[verticesData bytes];
    int all_vn = vcount*2;
    for (int i = 0; i < all_vn; i += 2) {
        v[i] = GLKVector3MultiplyScalar(GLKVector3Subtract(v[i], center), 0.05);
    }
    
    _bunny.indicesCount = icount;
    _bunny.indices = (uint32_t *)indicesData.bytes;
    
    _bunny.position.vector = (Vec3f *)verticesData.bytes; 
    _bunny.position.stride = 2;
    
    _bunny.normal.vector = (Vec3f *)verticesData.bytes;
    _bunny.normal.offset = 1;
    _bunny.normal.stride = 2;
    
    
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
