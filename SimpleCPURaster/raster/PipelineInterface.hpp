//
//  PipelineInterface.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 25/05/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef IPipeline_h
#define IPipeline_h

#include "Resource.hpp"

class PipelineInterface {
public:
    
    /**
     Position of vertex

     */
    virtual vec3 position(const Resource *resource, const uint32_t& index) = 0;
    virtual void pixel(const Resource *resource, vec3& pixel, const vec3& barycentric, const Triangle& triangle) = 0;
};


#endif /* IPipeline_h */
