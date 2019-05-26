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
#include "pipeline.h"



class Raster {

    typedef std::function<Vec3f (Vec3f vertex, Matrix44f projection)> VertexFunction;
    typedef std::function<Vec3f (Vec3f vertex)> FragmentFunction;
    
public:    
    
    Raster(VertexFunction vFunction, FragmentFunction fFunction);
    
    ~Raster();
    
    void setFramebuffer(Framebuffer *framebuffer);
    void clear();
    
    void setPipeline(PipelineInterface *pipeline);
    
    void draw(Vec3f *vertices, uint32_t *indices, uint32_t primitivesCount);
    
    Matrix44f projection;
    
private:
    float p_fimageWidth = 0.0;
    float p_fimageHeight = 0.0;
    
    PipelineInterface *p_pipeline = NULL;
    
    Framebuffer *_framebuffer = NULL;
    VertexFunction _vertexFunction;
    FragmentFunction _fragmentFunction;
    
    void _toRasterSpace(Vec3f& vertex);
};


#endif /* raster_hpp */
