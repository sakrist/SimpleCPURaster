//
//  raster.cpp
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 25/05/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#include <stdio.h>
#include "raster.hpp"
    
Raster::Raster(VertexFunction vFunction, FragmentFunction fFunction) :
_vertexFunction(std::forward<VertexFunction>(vFunction)), 
_fragmentFunction(std::forward<FragmentFunction>(fFunction)) {}

Raster::~Raster() {}

void Raster::setFramebuffer(Framebuffer *framebuffer) {
    _framebuffer = framebuffer;
    if (_framebuffer) {
        p_fimageWidth = _framebuffer->getSize().x;
        p_fimageHeight = _framebuffer->getSize().y;
    }
}

void Raster::clear() {
    _framebuffer->clear();
}

void Raster::setPipeline(PipelineInterface *pipeline) {
    p_pipeline = pipeline;
}

void Raster::_toRasterSpace(Vec3f& vertex) {
    
    // project onto the screen
    vertex.x /= vertex.z;
    vertex.y /= vertex.z; 
    
    // convert from screen space to NDC then raster (in one go)
    vertex.x = (1.0f + vertex.x) * 0.5f * p_fimageWidth;
    vertex.y = (1.0f + vertex.y) * 0.5f * p_fimageHeight; 
    vertex.z = 1.0f / vertex.z;        
}

void Raster::draw(Vec3f *vertices, uint32_t *indices, uint32_t primitivesCount) {
    if (!_framebuffer)
        return;
    
    Vec3<unsigned char> *frameBuffer = _framebuffer->getColorbuffer();
    float *depthBuffer  = _framebuffer->getDepthbuffer();
    uint32_t imageWidth = _framebuffer->getSize().x;
    uint32_t imageHeight = _framebuffer->getSize().y;
    
    for (uint32_t i = 0; i < primitivesCount; ++i) { 
        const Vec3f &v0 = vertices[indices[i * 3]]; 
        const Vec3f &v1 = vertices[indices[i * 3 + 1]]; 
        const Vec3f &v2 = vertices[indices[i * 3 + 2]];
        
        Vec3f v0Raster = _vertexFunction(v0, projection);
        Vec3f v1Raster = _vertexFunction(v1, projection);
        Vec3f v2Raster = _vertexFunction(v2, projection);
        
        _toRasterSpace(v0Raster);
        _toRasterSpace(v1Raster);
        _toRasterSpace(v2Raster);
        
        if (isnan(v0Raster.z) || isnan(v1Raster.z) || isnan(v2Raster.z)) {
            continue;
        }
        
        if (v0Raster.z < 0.0 && v1Raster.z < 0.0 && v2Raster.z < 0.0) {
            continue;
        }
        
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
                Vec3f pixelSample(x + 0.5f, y + 0.5f, 0.0f);
                Vec3f w;
                w[0] = edgeFunction(v1Raster, v2Raster, pixelSample); 
                w[1] = edgeFunction(v2Raster, v0Raster, pixelSample); 
                w[2] = edgeFunction(v0Raster, v1Raster, pixelSample); 
                if (w[0] >= 0.0 && w[1] >= 0.0 && w[2] >= 0.0) {
                    w /= area;
                    
                    float z = 1.0f / (v0Raster.z * w[0] + v1Raster.z * w[1] + v2Raster.z * w[2]); 
                    
                    // Depth-buffer test 
                    if (z < depthBuffer[y * imageWidth + x]) { 
                        depthBuffer[y * imageWidth + x] = z;
                        
                        pixelSample.z = z;
                        
                        Vec3f pixelSample(x + 0.5f, y + 0.5f, 0.0f);
                        pixelSample = _fragmentFunction(pixelSample);
                        frameBuffer[y * imageWidth + x] = Vec3c(pixelSample.x * 255.0, pixelSample.y * 255.0, pixelSample.z * 255.0);
                        
                    }
                }
            }
        }
    }
}


