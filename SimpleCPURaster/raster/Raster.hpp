//
//  Raster.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 16/04/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef raster_hpp
#define raster_hpp

#include <stdio.h>
#include "Framebuffer.hpp"
#include "PipelineInterface.hpp"



class Raster {
    
public:    
    
    Raster();
    ~Raster();
    
    void setFramebuffer(std::weak_ptr<Framebuffer> framebuffer);
    
    void setPipeline(std::weak_ptr<PipelineInterface> pipeline);
    
    void draw(const Resource& item);
    
    void clear();
        
private:
    vec2 _fimageSize{0, 0};
    
    std::weak_ptr<PipelineInterface> _pipeline;
    std::weak_ptr<Framebuffer> _framebuffer;
    
    void _toRasterSpace(vec3& vertex);
};


#endif /* raster_hpp */
