

#ifndef _MATH_H_
#define _MATH_H_
#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <iomanip> 
#include <cmath> 
 

#ifdef __SSE3__
#include <immintrin.h>
#include <stdint.h>
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace glm;

namespace glm
{
    typedef vec<3, unsigned char, defaultp>        cvec3;
}


static inline vec3 cBarycentric(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &w) {
    vec3 result;
    
#if defined(__SSE3__)
    
    float ret[4];
    _mm_storer_ps(&ret[0], _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(w.x), 
                                                            _mm_set_ps(a.x, a.y, a.z, 0.0f)),
                                                 _mm_mul_ps(_mm_set1_ps(w.y), 
                                                            _mm_set_ps(b.x, b.y, b.z, 0.0f))),
                                      _mm_mul_ps(_mm_set1_ps(w.z), 
                                                 _mm_set_ps(c.x, c.y, c.z, 0.0f))));    
    return vec3(ret[0], ret[1], ret[2]);
#else
    result.x = (fmaf(w.x, a.x, fmaf(w.y, b.x, w.z * c.x)));
    result.y = (fmaf(w.x, a.y, fmaf(w.y, b.y, w.z * c.y)));
    result.z = (fmaf(w.x, a.z, fmaf(w.y, b.z, w.z * c.z)));
#endif
    return result;
}

static inline float degreesToRadians(float degrees) {
    return degrees * (M_PI / 180.0);
}


static inline float edgeFunction(const vec3 &a, const vec3 &b, const vec3 &c) { 
#if defined(__SSE3__)
    
    float r2[4];
    _mm_storer_ps(&r2[0], _mm_sub_ps(_mm_set_ps(c[0], b[1], c[1], b[0]), _mm_set_ps(a[0], a[1], a[1], a[0])));
    return r2[0] * r2[1] - r2[2] * r2[3]; 
    
#else
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
#endif
}


static inline float clamp( float& val, float& minval, float& maxval ) {
    return std::min( std::max(val, minval), maxval);
}    

static inline float min3(float& a, float& b, float& c) {
    return std::min(a, std::min(b, c));
} 

static inline float max3(float &a, float &b, float &c) {
    return std::max(a, std::max(b, c));
} 


static inline vec3 clamp(  vec3& val, float minval, float maxval ) {
    return vec3(clamp(val.x, minval, maxval), clamp(val.y, minval, maxval), clamp(val.z, minval, maxval));
}

#endif
