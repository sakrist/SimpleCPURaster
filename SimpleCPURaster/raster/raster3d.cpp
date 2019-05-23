/*
A practical implementation of the rasterization algorithm.
Instructions to compile this program:
Download the raster3d.cpp, cow.h and geometry.h files to the same folder. Open a shell/terminal, and run the following command where the files are saved:
c++ -o raster3d raster3d.cpp  -std=c++11 -O3
Run with: ./raster3d. Open the file ./output.png in Photoshop or any program reading PPM files.
*/


#include "math.h" 
#include <fstream> 
#include <chrono> 
 
#include "cow.h" 
#include "framebuffer.hpp" 
 
static const float inchToMm = 25.4; 
enum FitResolutionGate { kFill = 0, kOverscan }; 
 
// Compute screen coordinates based on a physically-based camera model http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
void computeScreenCoordinates( 
    const float &filmApertureWidth, 
    const float &filmApertureHeight, 
    const uint32_t &imageWidth, 
    const uint32_t &imageHeight, 
    const int &fitFilm, 
    const float &nearClippingPLane, 
    const float &focalLength, 
    float &top, float &bottom, float &left, float &right 
) 
{ 
    float filmAspectRatio = filmApertureWidth / filmApertureHeight; 
    float deviceAspectRatio = imageWidth / (float)imageHeight; 
 
    top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPLane; 
    right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPLane; 
 
    // field of view (horizontal)
    float fov = 2 * 180 / M_PI * atan((filmApertureWidth * inchToMm / 2) / focalLength); 
    std::cerr << "Field of view " << fov << std::endl; 
 
    float xscale = 1; 
    float yscale = 1; 
 
    switch (fitFilm) { 
        default: 
        case kFill: 
            if (filmAspectRatio > deviceAspectRatio) { 
                xscale = deviceAspectRatio / filmAspectRatio; 
            } 
            else { 
                yscale = filmAspectRatio / deviceAspectRatio; 
            } 
            break; 
        case kOverscan: 
            if (filmAspectRatio > deviceAspectRatio) { 
                yscale = filmAspectRatio / deviceAspectRatio; 
            } 
            else { 
                xscale = deviceAspectRatio / filmAspectRatio; 
            } 
            break; 
    } 
 
    right *= xscale; 
    top *= yscale; 
 
    bottom = -top; 
    left = -right; 
} 
 
// Compute vertex raster screen coordinates. Vertices are defined in world space. They are then converted to camera space, then to NDC space (in the range [-1,1]) and then to raster space. The z-coordinates of the vertex in raster space is set with the z-coordinate of the vertex in camera space.
void convertToRaster( 
    const Vec3f &vertexWorld, 
    const Matrix44f &worldToCamera, 
    const float &l, 
    const float &r, 
    const float &t, 
    const float &b, 
    const float &near, 
    const uint32_t &imageWidth, 
    const uint32_t &imageHeight, 
    Vec3f &vertexRaster 
) 
{ 
    Vec3f vertexCamera = (Matrix44f)worldToCamera * (Vec3f)vertexWorld; 
 
    // convert to screen space
    Vec2f vertexScreen; 
    vertexScreen.x = near * vertexCamera.x / -vertexCamera.z; 
    vertexScreen.y = near * vertexCamera.y / -vertexCamera.z; 
 
    // now convert point from screen space to NDC space (in range [-1,1])
    Vec2f vertexNDC; 
    vertexNDC.x = 2 * vertexScreen.x / (r - l) - (r + l) / (r - l); 
    vertexNDC.y = 2 * vertexScreen.y / (t - b) - (t + b) / (t - b); 
 
    // convert to raster space
    vertexRaster.x = (vertexNDC.x + 1) / 2 * imageWidth; 
    // in raster space y is down so invert direction
    vertexRaster.y = (1 - vertexNDC.y) / 2 * imageHeight; 
    vertexRaster.z = -vertexCamera.z; 
} 
 
 
uint32_t imageWidth = 640; 
uint32_t imageHeight = 480; 
Matrix44f worldToCamera = {0.707107, -0.331295, 0.624695, 0, 
    0, 0.883452, 0.468521, 0, 
    -0.707107, -0.331295, 0.624695, 0, 
    -1.63871, -5.747777, -40.400412, 1}; 
 
const uint32_t ntris = 3156; 
const float nearClippingPLane = 1; 
float focalLength = 20; // in mm 
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980; 
float filmApertureHeight = 0.735; 
 
void raster(Framebuffer *framebuffer, Matrix44f proj) {
    
    framebuffer->clear();
    worldToCamera = proj;
 
    // compute screen coordinates
    float t, b, l, r; 
 
    imageWidth = framebuffer->getSize().x;
    imageHeight = framebuffer->getSize().y;
    
    computeScreenCoordinates( 
        filmApertureWidth, filmApertureHeight, 
        imageWidth, imageHeight, 
        kFill, 
        nearClippingPLane, 
        focalLength, 
        t, b, l, r); 
 
    // define the frame-buffer and the depth-buffer. Initialize depth buffer
    // to far clipping plane.
    Vec3<unsigned char> *frameBuffer = framebuffer->getColorbuffer();
    float *depthBuffer  = framebuffer->getDepthbuffer(); 
 
 
    auto t_start = std::chrono::high_resolution_clock::now(); 
 
// Outer loop 
    for (uint32_t i = 0; i < ntris; ++i) { 
        const Vec3f &v0 = vertices[nvertices[i * 3]]; 
        const Vec3f &v1 = vertices[nvertices[i * 3 + 1]]; 
        const Vec3f &v2 = vertices[nvertices[i * 3 + 2]]; 

//        Vec3f v0 = Vec3f(0.0, 0.0, 1.0); 
//        Vec3f v1 = Vec3f(1.0, 0.0, 1.0);
//        Vec3f v2 = Vec3f(0.0, 1.0, 1.0);
        
// Convert the vertices of the triangle to raster space 
        Vec3f v0Raster, v1Raster, v2Raster; 
        convertToRaster(v0, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v0Raster); 
        convertToRaster(v1, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v1Raster); 
        convertToRaster(v2, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v2Raster); 
 
// Precompute reciprocal of vertex z-coordinate 
        v0Raster.z = 1.0f / v0Raster.z;
        v1Raster.z = 1.0f / v1Raster.z; 
        v2Raster.z = 1.0f / v2Raster.z; 
 
 
// Prepare vertex attributes. Divde them by their vertex z-coordinate (though we use a multiplication here because v.z = 1 / v.z) 
        Vec2f st0 = st[stindices[i * 3]]; 
        Vec2f st1 = st[stindices[i * 3 + 1]]; 
        Vec2f st2 = st[stindices[i * 3 + 2]]; 
 
        st0 *= v0Raster.z;
        st1 *= v1Raster.z;
        st2 *= v2Raster.z; 
 
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
                Vec3f pixelSample(x + 0.5, y + 0.5, 0); 
                float w0 = edgeFunction(v1Raster, v2Raster, pixelSample); 
                float w1 = edgeFunction(v2Raster, v0Raster, pixelSample); 
                float w2 = edgeFunction(v0Raster, v1Raster, pixelSample); 
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) { 
                    w0 /= area; 
                    w1 /= area; 
                    w2 /= area; 
                    float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2; 
                    float z = 1 / oneOverZ; 
// Depth-buffer test 
                    if (z < depthBuffer[y * imageWidth + x]) { 
                        depthBuffer[y * imageWidth + x] = z; 
 
                        Vec2f st = st0 * w0 + st1 * w1 + st2 * w2; 
 
                        st *= z; 
 
// If you need to compute the actual position of the shaded point in camera space. Proceed like with the other vertex attribute. Divide the point coordinates by the vertex z-coordinate then interpolate using barycentric coordinates and finally multiply by sample depth.  
                        Vec3f v0Cam = worldToCamera * v0;
                        Vec3f v1Cam = worldToCamera * v1;
                        Vec3f v2Cam = worldToCamera * v2; 
 
                        float px = (v0Cam.x/-v0Cam.z) * w0 + (v1Cam.x/-v1Cam.z) * w1 + (v2Cam.x/-v2Cam.z) * w2; 
                        float py = (v0Cam.y/-v0Cam.z) * w0 + (v1Cam.y/-v1Cam.z) * w1 + (v2Cam.y/-v2Cam.z) * w2; 
 
                        Vec3f pt(px * z, py * z, -z); // pt is in camera space 
 
// Compute the face normal which is used for a simple facing ratio. Keep in mind that we are doing all calculation in camera space. Thus the view direction can be computed as the point on the object in camera space minus Vec3f(0), the position of the camera in camera space. 
                        Vec3f n = (v1Cam - v0Cam).crossProduct(v2Cam - v0Cam); 
                        n.normalize(); 
                        Vec3f viewDirection = -pt; 
                        viewDirection.normalize(); 
 
                        float nDotView =  std::max(0.f, n.dotProduct(viewDirection)); 
 
// The final color is the reuslt of the faction ration multiplied by the checkerboard pattern. 
                        const int M = 10; 
                        float checker = (fmod(st.x * M, 1.0) > 0.5) ^ (fmod(st.y * M, 1.0) < 0.5); 
                        float c = 0.3 * (1 - checker) + 0.7 * checker; 
                        nDotView *= c; 
                        frameBuffer[y * imageWidth + x].x = nDotView * 255; 
                        frameBuffer[y * imageWidth + x].y = nDotView * 255; 
                        frameBuffer[y * imageWidth + x].z = nDotView * 255; 
                    } 
                } 
            } 
        } 
    } 
 
    auto t_end = std::chrono::high_resolution_clock::now(); 
    auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count(); 
    std::cerr << "Wall passed time:  " << passedTime << " ms" << std::endl; 
 
// Store the result of the framebuffer to a PPM file (Photoshop reads PPM files). 
//    std::ofstream ofs; 
//    ofs.open("./output.ppm"); 
//    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n"; 
//    ofs.write((char*)frameBuffer, imageWidth * imageWidth * 3); 
//    ofs.close(); 
 
 
//    return 0; 
}
