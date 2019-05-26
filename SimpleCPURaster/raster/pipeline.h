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
    size_t size = 0;
    size_t offset = 0;
    size_t stride = 0;
};

typedef enum : int {
  PrimitiveTypeTriangle = 0
} PrimitiveType;

struct Resource {
    
    Attribute<Vec3f> position;
    Attribute<Vec3f> normal;
    Attribute<Vec2f> textureCoord;

    size_t indicesCount = 0;
    int *indices = NULL;
    
    PrimitiveType type = PrimitiveTypeTriangle;
};


class PipelineInterface {
public:
    virtual Vec3f vertex(Resource *resource, int index) = 0;
    virtual void pixel(Vec3f& pixel, const Vec3f& barycentric) = 0;
};


#endif /* IPipeline_h */
