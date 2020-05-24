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
    Raster() = default;
    ~Raster() = default;
    
    void setFramebuffer(std::weak_ptr<Framebuffer> framebuffer) {
        _framebuffer = framebuffer;
        if (_framebuffer.lock().get() != nullptr) {
            _fimageSize = _framebuffer.lock()->getSize();
        }
    }

    void clear() {
        Framebuffer *framebuffer = _framebuffer.lock().get();
        assert(framebuffer != nullptr);
        framebuffer->clear();
    }

    void setPipeline(std::weak_ptr<PipelineInterface> pipeline) {
        _pipeline = pipeline;
    }

    void draw(const Resource& item) {
        Framebuffer *framebuffer = _framebuffer.lock().get();
        PipelineInterface *pipeline = _pipeline.lock().get();
        assert(framebuffer != nullptr);
        assert(pipeline != nullptr);
        
        auto& frameBuffer = framebuffer->_colorBuffer;
        auto& depthBuffer  = framebuffer->_depthBuffer;
        uint32_t imageWidth = _fimageSize.x;
        uint32_t imageHeight = _fimageSize.y;
        
        // TODO: iterator primitives from  Resource
        for (uint32_t i = 0; i < item.primitivesCount(); i++ ) {

            const Triangle& triangle = item.getTriangle(i);
            
            vec3 v0Raster = pipeline->position(item, triangle.a);
            vec3 v1Raster = pipeline->position(item, triangle.b);
            vec3 v2Raster = pipeline->position(item, triangle.c);
            
            _toRasterSpace(v0Raster);
            _toRasterSpace(v1Raster);
            _toRasterSpace(v2Raster);
            
            if (isnan(v0Raster.z) || isnan(v1Raster.z) || isnan(v2Raster.z)) {
                continue;
            }
            
            if (v0Raster.z < 0.0f && v1Raster.z < 0.0f && v2Raster.z < 0.0f) {
                continue;
            }
            
            // bound box
            const float& xmin = min3(v0Raster.x, v1Raster.x, v2Raster.x);
            const float& ymin = min3(v0Raster.y, v1Raster.y, v2Raster.y);
            const float& xmax = max3(v0Raster.x, v1Raster.x, v2Raster.x);
            const float& ymax = max3(v0Raster.y, v1Raster.y, v2Raster.y);
            
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
                    if (w[0] >= 0.0f && w[1] >= 0.0f && w[2] >= 0.0f) {
                        w /= area;

                        float z = 1.0f / (v0Raster.z * w[0] + v1Raster.z * w[1] + v2Raster.z * w[2]);
                        
                        // Depth-buffer test
                        float& depthValue = depthBuffer[y * imageWidth + x];
                        if (z < depthValue) {
                            depthValue = z;
                            pixelSample.z = z;
                            
                            pipeline->pixel(item, pixelSample, w, triangle);
                            
                            frameBuffer[y * imageWidth + x] = cvec3(pixelSample.x * 255.0f, pixelSample.y * 255.0f, pixelSample.z * 255.0f);
                        }
                    }
                }
            }
        }
    }

        
private:
    vec2 _fimageSize{0, 0};
    
    std::weak_ptr<PipelineInterface> _pipeline;
    std::weak_ptr<Framebuffer> _framebuffer;
    
    void _toRasterSpace(vec3& vertex) {
        
        // project onto the screen
        vertex.x /= vertex.z;
        vertex.y /= vertex.z; 
        
        // convert from screen space to NDC then raster (in one go)
        vertex.x = (1.0f + vertex.x) * 0.5f * _fimageSize.x;
        vertex.y = (1.0f - (1.0f + vertex.y) * 0.5f) * _fimageSize.y;
        
        vertex.z = 1.0f / vertex.z;        
    }
};


#endif /* raster_hpp */
