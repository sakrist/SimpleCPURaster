//
//  Resource.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 07/06/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef Resource_hpp
#define Resource_hpp

#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef enum : int {
    PrimitiveTypeTriangle = 0
} PrimitiveType;

struct Triangle {
    int a,b,c;
    Triangle() {};
};



typedef enum : int {
    Attribute0 = 0,
    PositionsAttribute = Attribute0,
    Attribute1 = 1,
    NormalsAttribute = Attribute1,
    Attribute2 = 2,
    TextureCoordinatesAttribute = Attribute2,
    Attribute3 = 3,
    Attribute4 = 4,
    Attribute5 = 5,
    Attribute6 = 6,
    Attribute8 = 7,
    Attribute7 = 8
} AttributeType;


struct Attribute {
    uint32_t offset = 0; // offset in number of floats
    uint32_t stride = 0; // in case if data is  
};

class Resource {
public:
    PrimitiveType type = PrimitiveTypeTriangle;
    
    uint32_t primitivesCount() {
        return _indicesCount / 3; 
    }
    
    void setIndices(uint32_t indicesCount, uint32_t *indicesBuffer) {
        
        size_t sizeBytes = sizeof(uint32_t) * indicesCount;
        
        assert(sizeBytes != 0);
        assert(sizeBytes != UINT32_MAX);
        assert(indicesBuffer != NULL);
        
        _indicesCount = indicesCount;
        if (_indices != NULL) {
            free(_indices);
            _indices = NULL; 
        }
        
        _indices = (uint32_t *)malloc(sizeBytes);
        memcpy(_indices, indicesBuffer, sizeBytes);
    }
    
    void setAttributesBuffer(float *buffer, uint32_t size) {
        size_t sizeBytes = sizeof(float) * size;
        assert(sizeBytes != 0);
        assert(sizeBytes != UINT32_MAX);
        assert(buffer != NULL);
        
        if (_attributesBuffer != NULL) {
            free(_attributesBuffer);
            _attributesBuffer = NULL;
        }
        
        _attributesBuffer = (float *)malloc(sizeBytes);
        memcpy(_attributesBuffer, buffer, sizeBytes);
        _attributesBufferSize = size;
    }
    
    void setAttribute(AttributeType type, uint32_t stride, uint32_t offset = 0) {
        assert(stride != 0);
        assert(type < 10);
        
        _attributes[type].offset = offset;
        _attributes[type].stride = stride;
    }
    
    const float* getAttribute(AttributeType type, uint32_t index) const {
        uint32_t atIndex = _attributes[type].stride * index;
        return _attributesBuffer + atIndex + _attributes[type].offset;
    }
    
    Triangle getTriangle(uint32_t index) {
        assert(index < _indicesCount);
        
        Triangle triangle;
        if (_indices != NULL) {
            int index_ = index * 3;
            triangle.a = _indices[index_];
            triangle.b = _indices[index_+1];
            triangle.c = _indices[index_+2];
        }    
        return triangle;
    }
        
private:
    Attribute _attributes[10];
    uint32_t _indicesCount = 0;
    uint32_t* _indices = NULL;
    
    uint32_t _attributesBufferSize = 0;
    float* _attributesBuffer = NULL;
};

#endif /* Resource_hpp */
