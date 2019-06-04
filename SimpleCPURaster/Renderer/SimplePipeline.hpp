//
//  SimplePipeline.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 26/05/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef SimplePipeline_hpp
#define SimplePipeline_hpp

#include <stdio.h>
#include "math.h"
#include "pipeline.h"


class SimplePipeline : public PipelineInterface {
    
public:
    
    typedef std::function<Vec3f (const Vec3f& vertex, const Matrix44f& projection)> VertexFunction;
    typedef std::function<Vec3f (Vec3f position, Vec3f normal )> FragmentFunction;
    
    Matrix44f projection;
    
    
    SimplePipeline(VertexFunction vFunction, FragmentFunction fFunction) : p_vertexFunction(vFunction), p_fragmentFunction(fFunction) {}
    
    Vec3f vertex(const Resource *resource, uint32_t index) {
        const Vec3f &v = resource->getPosition(index);
        return p_vertexFunction(v, projection);
    }
    void pixel(const Resource *resource, Vec3f& pixel, const Vec3f& barycentric, const Triangle& triangle) {
        Vec3f normal;
        
        const Vec3f &n0 = resource->getNormal(triangle.a);
        const Vec3f &n1 = resource->getNormal(triangle.b);
        const Vec3f &n2 = resource->getNormal(triangle.c);
        
        normal.x = barycentric.x * n0[0] + barycentric.y * n1[0] + barycentric.z * n2[0]; 
        normal.y = barycentric.x * n0[1] + barycentric.y * n1[1] + barycentric.z * n2[1]; 
        normal.z = barycentric.x * n0[2] + barycentric.y * n1[2] + barycentric.z * n2[2];
        
        pixel = p_fragmentFunction(pixel, normal);
    }
    
private:
    VertexFunction p_vertexFunction;
    FragmentFunction p_fragmentFunction;
    
};


#endif /* SimplePipeline_hpp */
