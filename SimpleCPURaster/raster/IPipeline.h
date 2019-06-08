//
//  IPipeline.h
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
    virtual Vec3f position(const Resource *resource, uint32_t index) = 0;
    virtual void pixel(const Resource *resource, Vec3f& pixel, const Vec3f& barycentric, const Triangle& triangle) = 0;
};


#endif /* IPipeline_h */
