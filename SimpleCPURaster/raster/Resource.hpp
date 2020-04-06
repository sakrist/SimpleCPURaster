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
#include <vector>

typedef enum : int {
    PrimitiveTypeTriangle = 0
} PrimitiveType;

struct Triangle {
    uint32_t a,b,c;
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
    
    size_t primitivesCount() const {
        return _indices.size();
    }
    
    void setIndices(uint32_t indicesCount, uint32_t *indicesBuffer) {
        
        size_t sizeBytes = sizeof(uint32_t) * indicesCount;
        
        assert(sizeBytes != 0);
        assert(sizeBytes != UINT32_MAX);
        assert(indicesBuffer != NULL);
        
        if (_indices.size() != 0) {
            _indices.clear();
        }

        Triangle *tris = (Triangle *)indicesBuffer;
        std::copy(tris, tris + indicesCount/3, std::back_inserter(_indices));
    }

    /// Set mesh attributes data to resource where vertices are in float format.
    /// @param buffer pointer on data with float values
    /// @param sizeElements number of float elements
    void setAttributesBuffer(float *buffer, uint32_t sizeElements) {
        assert(sizeElements != 0);
        assert(sizeElements != UINT32_MAX);
        assert(buffer != NULL);
        
        if (_attributesBuffer.size() != 0) {
            _attributesBuffer.clear();
        }

        std::copy(buffer, buffer + sizeElements, std::back_inserter(_attributesBuffer));
    }
    
    void setAttribute(AttributeType type, uint32_t stride, uint32_t offset = 0) {
        assert(stride != 0);
        assert(type < 10);
        
        _attributes[type].offset = offset;
        _attributes[type].stride = stride;
    }

    template <typename T>
    const T& getAttribute(const AttributeType& type, const uint32_t& index) const {
        uint32_t atIndex = _attributes[type].stride * index + _attributes[type].offset;
        return ((const T* )&_attributesBuffer[atIndex])[0];
    }
    
    const Triangle& getTriangle(uint32_t index) const {
        assert(index < _indices.size());
        return _indices[index];
    }
        
private:
    Attribute _attributes[10];
    std::vector<Triangle> _indices;
    std::vector<float> _attributesBuffer;
};

#endif /* Resource_hpp */
