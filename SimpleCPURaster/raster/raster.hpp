//
//  raster.hpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 16/04/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#ifndef raster_hpp
#define raster_hpp

#include <stdio.h>
#include "framebuffer.hpp"
#include "IPipeline.h"



class Raster {
    
public:    
    
    Raster();
    ~Raster();
    
    void setFramebuffer(Framebuffer *framebuffer);
    void clear();
    
    void setPipeline(PipelineInterface *pipeline);
    
    void draw(Resource *item);
        
private:
    float p_fimageWidth = 0.0;
    float p_fimageHeight = 0.0;
    
    PipelineInterface *p_pipeline = NULL;
    
    Framebuffer *p_framebuffer = NULL;
    
    void _toRasterSpace(Vec3f& vertex);
};


#endif /* raster_hpp */
