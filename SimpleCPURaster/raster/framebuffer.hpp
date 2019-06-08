//
//  Framebuffer.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 13/04/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef Framebuffer_hpp
#define Framebuffer_hpp

#include <stdio.h>
#include "math.h"

class Framebuffer {
public:
    Framebuffer(Vec2i size) : _size(size), _length(size.x * size.y) {
        assert(_size.x > 0);
        assert(_size.y > 0);
        
        _colorBuffer = new Vec3c[_length];
        _depthBuffer = new float[_length];
        clear();
    }
    
    ~Framebuffer() {
        delete _colorBuffer;
        delete _depthBuffer;
    }
    
    void clear() {
        int r = ((int *)&_clearColor)[0];
        memset(_colorBuffer, r, _length * sizeof(Vec3c));
        float v = MAXFLOAT;
        void *rr = &v;
        memset_pattern4(_depthBuffer, rr, _length * sizeof(float));
    }
    
    Vec2i getSize() const {
        return _size;
    }
    
    Vec3c * getColorbuffer() const {
        return _colorBuffer;
    }
    
    void * getColorData() const {
        return (void *)_colorBuffer;
    } 
    
    float * getDepthbuffer() const {
        return _depthBuffer;
    }
    
private:
    Vec2i _size;
    int _length;
    
    Vec3c _clearColor = Vec3c(255);
    
    Vec3c *_colorBuffer;
    float *_depthBuffer;
};

#endif /* Framebuffer_hpp */
