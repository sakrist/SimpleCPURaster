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
#include <vector>
#include <string.h>
#include "Math.h"

class Framebuffer {
public:
    Framebuffer(ivec2 size) : _size(size), _length(size.x * size.y) {
        assert(_size.x > 0);
        assert(_size.y > 0);
        
        _colorBuffer.reserve(_length);
        _depthBuffer.reserve(_length);
        clear();
    }
    
    ~Framebuffer() {

    }
    
    void clear() {
        int r = ((int *)&_clearColor)[0];
        memset(_colorBuffer.data(), r, _length * sizeof(cvec3));
        float v = MAXFLOAT;
        void *rr = &v;
        memset_pattern4(_depthBuffer.data(), rr, _length * sizeof(float));
    }
    
    const ivec2& getSize() const {
        return _size;
    }
    
    const void* getColorbuffer() const {
        return _colorBuffer.data();
    }

    const size_t getColorbufferSize() {
        return _length * sizeof(cvec3);
    }
    
    const void* getDepthbuffer() const {
        return _depthBuffer.data();
    }

    const size_t getDepthbufferSize() {
        return _length * sizeof(float);
    }
    
private:
    friend class Raster;

    const ivec2 _size;
    const int _length;
    
    cvec3 _clearColor = cvec3(255);
    
    std::vector<cvec3> _colorBuffer{};
    std::vector<float> _depthBuffer{};
};

#endif /* Framebuffer_hpp */
