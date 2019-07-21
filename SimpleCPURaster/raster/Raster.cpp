//
//  Raster.cpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 25/05/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#include <stdio.h>
#include "Raster.hpp"
    
Raster::Raster() {}

Raster::~Raster() {}

void Raster::setFramebuffer(Framebuffer *framebuffer) {
    p_framebuffer = framebuffer;
    if (p_framebuffer) {
        p_fimageWidth = p_framebuffer->getSize().x;
        p_fimageHeight = p_framebuffer->getSize().y;
    }
}

void Raster::clear() {
    p_framebuffer->clear();
}

void Raster::setPipeline(PipelineInterface *pipeline) {
    p_pipeline = pipeline;
}

void Raster::_toRasterSpace(vec3& vertex) {
    
    // project onto the screen
    vertex.x /= vertex.z;
    vertex.y /= vertex.z; 
    
    // convert from screen space to NDC then raster (in one go)
    vertex.x = (1.0f + vertex.x) * 0.5f * p_fimageWidth;
    vertex.y = (1.0f - (1.0f + vertex.y) * 0.5f) * p_fimageHeight; 
    
    vertex.z = 1.0f / vertex.z;        
}

void Raster::draw(Resource *item) {
    assert(p_framebuffer != NULL);
    assert(p_pipeline != NULL);
    
    cvec3 *frameBuffer = p_framebuffer->getColorbuffer();
    float *depthBuffer  = p_framebuffer->getDepthbuffer();
    uint32_t imageWidth = p_framebuffer->getSize().x;
    uint32_t imageHeight = p_framebuffer->getSize().y;
    
    for (uint32_t i = 0; i < item->primitivesCount(); i++ ) { // TODO: iterator of primitives from  Resource

        Triangle triangle = item->getTriangle(i);
        
        vec3 v0Raster = p_pipeline->position(item, triangle.a);
        vec3 v1Raster = p_pipeline->position(item, triangle.b);
        vec3 v2Raster = p_pipeline->position(item, triangle.c);
        
        _toRasterSpace(v0Raster);
        _toRasterSpace(v1Raster);
        _toRasterSpace(v2Raster);
        
        if (isnan(v0Raster.z) || isnan(v1Raster.z) || isnan(v2Raster.z)) {
            continue;
        }
        
        if (v0Raster.z < 0.0 && v1Raster.z < 0.0 && v2Raster.z < 0.0) {
            continue;
        }
        
//        if (v0Raster.z > 1.0 && v1Raster.z > 1.0 && v2Raster.z > 1.0) {
//            continue;
//        }
        
        // bound box
        float xmin = min3(v0Raster.x, v1Raster.x, v2Raster.x); 
        float ymin = min3(v0Raster.y, v1Raster.y, v2Raster.y); 
        float xmax = max3(v0Raster.x, v1Raster.x, v2Raster.x); 
        float ymax = max3(v0Raster.y, v1Raster.y, v2Raster.y); 
        
        // the triangle is out of screen
        if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0) continue; 
        
        // be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
        uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin))); 
        uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(xmax))); 
        uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin))); 
        uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(ymax))); 
        
        float area = edgeFunction(v0Raster, v1Raster, v2Raster); 
        
        // Inner loop 
        for (uint32_t y = y0; y <= y1; ++y) { 
            for (uint32_t x = x0; x <= x1; ++x) { 
                vec3 pixelSample(x + 0.5f, y + 0.5f, 0.0f);
                vec3 w;
                w[0] = edgeFunction(v1Raster, v2Raster, pixelSample); 
                w[1] = edgeFunction(v2Raster, v0Raster, pixelSample); 
                w[2] = edgeFunction(v0Raster, v1Raster, pixelSample); 
                if (w[0] >= 0.0 && w[1] >= 0.0 && w[2] >= 0.0) {
                    w /= area;

//#if defined(__SSE3__)
//                    __m128 v = _mm_mul_ps(_mm_set_ps(v0Raster.z, v1Raster.z, v2Raster.z, 0.0f),
//                                          _mm_set_ps(w[0], w[1], w[2], 0.0f));
//                    v = _mm_hadd_ps(v,v);
//                    v = _mm_hadd_ps(v,v);
//                    float z;
//                    _mm_store_ss(&z, _mm_div_ss(_mm_set_ss(1.0f), v));
//#else
                    float z = 1.0f / (v0Raster.z * w[0] + v1Raster.z * w[1] + v2Raster.z * w[2]); 
//#endif               
                    // Depth-buffer test 
                    if (z < depthBuffer[y * imageWidth + x]) { 
                        depthBuffer[y * imageWidth + x] = z;
                        
                        pixelSample.z = z;
                        
                        vec3 pixelSample(x + 0.5f, y + 0.5f, 0.0f);
                        p_pipeline->pixel(item, pixelSample, w, triangle);
                        
                        frameBuffer[y * imageWidth + x] = cvec3(pixelSample.x * 255.0, pixelSample.y * 255.0, pixelSample.z * 255.0);
                    }
                }
            }
        }
    }
}


