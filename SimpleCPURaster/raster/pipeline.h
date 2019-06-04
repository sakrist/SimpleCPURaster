//
//  IPipeline.h
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 25/05/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef IPipeline_h
#define IPipeline_h


template<typename T> 
struct Attribute {
    T * vector = NULL;
    uint32_t size = 0; // number of elemets in vector
    uint32_t offset = 0; // sum of elements 
    uint32_t stride = 0; // in case if data is 
};

typedef enum : int {
  PrimitiveTypeTriangle = 0
} PrimitiveType;

struct Triangle {
    int a,b,c;
    Triangle(int a, int b, int c) : a(a), b(b), c(c) {};
};

struct Resource {
    
    Attribute<Vec3f> position;
    Attribute<Vec3f> normal;
    Attribute<Vec2f> textureCoord;

    uint32_t indicesCount = 0;
    uint32_t *indices = NULL;
    
    PrimitiveType type = PrimitiveTypeTriangle;
    
    uint32_t primitivesCount() {
        return indicesCount / 3; 
    }
    
    const void updateIndices(Triangle& triangle) const {
        if (indices != NULL) { 
            triangle.a = indices[triangle.a];
            triangle.b = indices[triangle.b];
            triangle.c = indices[triangle.c];
        }    
    }
    
    
    const Vec3f& getPosition(int atIndex) const {
        atIndex = (position.stride == 0)? atIndex : atIndex * position.stride;
        return position.vector[atIndex + position.offset];    
    }
    
    const Vec3f& getNormal(int atIndex) const {
        atIndex = (normal.stride == 0)? atIndex : atIndex * normal.stride;
        return normal.vector[atIndex + normal.offset];    
    }    
    
};


class PipelineInterface {
public:
    virtual Vec3f vertex(const Resource *resource, uint32_t index) = 0;
    virtual void pixel(const Resource *resource, Vec3f& pixel, const Vec3f& barycentric, const Triangle& triangle) = 0;
};


#endif /* IPipeline_h */
