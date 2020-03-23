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
#include "Math.h"
#include "PipelineInterface.hpp"
#include "Resource.hpp"


class SimplePipeline : public PipelineInterface {
    
public:
    
    SimplePipeline() {}
    
    mat4 projection;
    
    vec3 _project(const vec3& v, const mat4& _p) {
        return _p * vec4(v, 1.0);
    }
    
    vec3 position(const Resource *resource, const uint32_t& index) override {
        const vec3& v = ((const vec3 *)(resource->getAttribute(PositionsAttribute, index)))[0];
        return _project(v, projection); 
    }

    void pixel(const Resource *resource, vec3& pixel, const vec3& barycentric, const Triangle& triangle) override {

        const vec3& n0 = ((const vec3*)(resource->getAttribute(NormalsAttribute, triangle.a)))[0];
        const vec3& n1 = ((const vec3*)(resource->getAttribute(NormalsAttribute, triangle.b)))[0];
        const vec3& n2 = ((const vec3*)(resource->getAttribute(NormalsAttribute, triangle.c)))[0];

        vec3 normal = cBarycentric(n0, n1, n2, barycentric);
        
        vec3 lightDirection(1.0f);
        vec3 color = vec3(fmax(dot(normal, lightDirection), 0.0) * 0.56f );
        pixel = clamp(color, 0.0, 1.0);
    }
    
    
};


#endif /* SimplePipeline_hpp */
