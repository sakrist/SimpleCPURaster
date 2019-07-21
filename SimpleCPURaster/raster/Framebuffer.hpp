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
#include "Math.h"

class Framebuffer {
public:
    Framebuffer(ivec2 size) : _size(size), _length(size.x * size.y) {
        assert(_size.x > 0);
        assert(_size.y > 0);
        
        _colorBuffer = new cvec3[_length];
        _depthBuffer = new float[_length];
        clear();
    }
    
    ~Framebuffer() {
        delete _colorBuffer;
        delete _depthBuffer;
    }
    
    void clear() {
        int r = ((int *)&_clearColor)[0];
        memset(_colorBuffer, r, _length * sizeof(cvec3));
        float v = MAXFLOAT;
        void *rr = &v;
        memset_pattern4(_depthBuffer, rr, _length * sizeof(float));
    }
    
    ivec2 getSize() const {
        return _size;
    }
    
    cvec3 * getColorbuffer() const {
        return _colorBuffer;
    }
    
    void * getColorData() const {
        return (void *)_colorBuffer;
    } 
    
    float * getDepthbuffer() const {
        return _depthBuffer;
    }
    
private:
    ivec2 _size;
    int _length;
    
    cvec3 _clearColor = cvec3(255);
    
    cvec3 *_colorBuffer;
    float *_depthBuffer;
};

#endif /* Framebuffer_hpp */
