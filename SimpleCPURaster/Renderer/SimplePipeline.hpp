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
#include "IPipeline.h"
#include "Resource.hpp"


class SimplePipeline : public PipelineInterface {
    
public:
    
    SimplePipeline() {}
    
    Matrix44f projection;
    
    Vec3f _project(const Vec3f& v, Matrix44f _p) {
        return _p * v;
    }
    
    Vec3f position(const Resource *resource, uint32_t index) {
        const Vec3f v = ((const Vec3f *)(resource->getAttribute(PositionsAttribute, index)))[0];
        return _project(v, projection); 
    }
    void pixel(const Resource *resource, Vec3f& pixel, const Vec3f& barycentric, const Triangle& triangle) {
        
        
        const Vec3f &n0 = ((const Vec3f *)(resource->getAttribute(NormalsAttribute, triangle.a)))[0];
        const Vec3f &n1 = ((const Vec3f *)(resource->getAttribute(NormalsAttribute, triangle.b)))[0];
        const Vec3f &n2 = ((const Vec3f *)(resource->getAttribute(NormalsAttribute, triangle.c)))[0];

        Vec3f normal = cBarycentric(n0, n1, n2, barycentric);
        
        Vec3f lightDirection(1.0f);
        Vec3f color = fmax(normal.dotProduct(lightDirection), 0.0) * 0.56f;
        pixel = clamp(color, 0.0, 1.0);
        
    }
    
    
};


#endif /* SimplePipeline_hpp */
