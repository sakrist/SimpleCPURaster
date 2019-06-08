

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

template<typename T> 
class Vec2 
{ 
public: 
    Vec2() : x(0), y(0) {} 
    Vec2(T xx) : x(xx), y(xx) {} 
    Vec2(T xx, T yy) : x(xx), y(yy) {} 
    Vec2(T* x) : x(x[0]), y(x[1]) {} 
    Vec2 operator + (const Vec2 &v) const 
    { return Vec2(x + v.x, y + v.y); } 
    Vec2 operator / (const T &r) const 
    { return Vec2(x / r, y / r); } 
    Vec2 operator * (const T &r) const 
    { return Vec2(x * r, y * r); } 
    Vec2& operator /= (const T &r) 
    { x /= r, y /= r; return *this; } 
    Vec2& operator *= (const T &r) 
    { x *= r, y *= r; return *this; } 
    friend std::ostream& operator << (std::ostream &s, const Vec2<T> &v) 
    { 
        return s << '[' << v.x << ' ' << v.y << ']'; 
    } 
    friend Vec2 operator * (const T &r, const Vec2<T> &v) 
    { return Vec2(v.x * r, v.y * r); } 
    T x, y; 
}; 
 
typedef Vec2<float> Vec2f; 
typedef Vec2<int> Vec2i; 
 
// Implementation of a generic vector class - it will be used to deal with 3D points, vectors and normals. The class is implemented as a template. While it may complicate the code a bit, it gives us the flexibility later, to specialize the type of the coordinates into anything we want. For example: Vec3f if we want the coordinates to be floats or Vec3i if we want the coordinates to be integers.
// Vec3 is a standard/common way of naming vectors, points, etc. The OpenEXR and Autodesk libraries use this convention for instance.
template<typename T> 
class Vec3 
{ 
public: 
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {} 
    Vec3(T xx) : x(xx), y(xx), z(xx) {} 
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3(T* x) : x(x[0]), y(x[1]), z(x[2]) {}
    Vec3(const T* x) : x(x[0]), y(x[1]), z(x[2]) {}
    Vec3 operator + (const Vec3 &v) const 
    { return Vec3(x + v.x, y + v.y, z + v.z); } 
    Vec3 operator - (const Vec3 &v) const 
    { return Vec3(x - v.x, y - v.y, z - v.z); } 
    Vec3 operator - () const 
    { return Vec3(-x, -y, -z); } 
    Vec3 operator * (const T &r) const 
    { return Vec3(x * r, y * r, z * r); } 
    Vec3 operator * (const Vec3 &v) const 
    { return Vec3(x * v.x, y * v.y, z * v.z); } 
    T dotProduct(const Vec3<T> &v) const 
    { return x * v.x + y * v.y + z * v.z; } 
    Vec3& operator /= (const T &r) 
    { x /= r, y /= r, z /= r; return *this; } 
    Vec3& operator *= (const T &r) 
    { x *= r, y *= r, z *= r; return *this; } 
    Vec3 crossProduct(const Vec3<T> &v) const 
    { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); } 
    T norm() const 
    { return x * x + y * y + z * z; } 
    T length() const 
    { return sqrt(norm()); } 
// The next two operators are sometimes called access operators or accessors. The Vec coordinates can be accessed that way v[0], v[1], v[2], rather than using the more traditional form v.x, v.y, v.z. This useful when vectors are used in loops: the coordinates can be accessed with the loop index (e.g. v[i]).
    const T& operator [] (uint8_t i) const { return (&x)[i]; } 
    T& operator [] (uint8_t i) { return (&x)[i]; } 
    Vec3& normalize() 
    { 
        T n = norm(); 
        if (n > 0) { 
            T factor = 1 / sqrt(n); 
            x *= factor, y *= factor, z *= factor; 
        } 
 
        return *this; 
    } 
 
    friend Vec3 operator * (const T &r, const Vec3 &v) 
    { return Vec3<T>(v.x * r, v.y * r, v.z * r); } 
    friend Vec3 operator / (const T &r, const Vec3 &v) 
    { return Vec3<T>(r / v.x, r / v.y, r / v.z); } 
 
    friend std::ostream& operator << (std::ostream &s, const Vec3<T> &v) 
    { 
        return s << '[' << v.x << ' ' << v.y << ' ' << v.z << ']'; 
    } 
 
    T x, y, z; 
}; 
 
// Now you can specialize the class. We are just showing 3 examples here. In your code you can declare a vector either that way: Vec3<float> a, or that way: Vec3f a
typedef Vec3<float> Vec3f; 
typedef Vec3<int> Vec3i;
typedef Vec3<unsigned char> Vec3c;
 
template<typename T> 
class Matrix44 
{ 
public: 
 
    T x[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; 
 
    Matrix44() {} 
 
    Matrix44 (T a, T b, T c, T d, T e, T f, T g, T h, 
              T i, T j, T k, T l, T m, T n, T o, T p) 
    { 
        x[0] = a; 
        x[1] = b; 
        x[2] = c; 
        x[3] = d; 
        x[4] = e; 
        x[5] = f; 
        x[6] = g; 
        x[7] = h; 
        x[8] = i; 
        x[9] = j; 
        x[10] = k; 
        x[11] = l; 
        x[12] = m; 
        x[13] = n; 
        x[14] = o; 
        x[15] = p; 
    } 
 
    const T* operator [] (uint8_t i) const { return x[i]; } 
    T* operator [] (uint8_t i) { return x[i]; } 
 
    // Multiply the current matrix with another matrix (rhs)
    Matrix44 operator * (const Matrix44& v) const 
    { 
        Matrix44 tmp; 
        multiply (*this, v, tmp); 
 
        return tmp; 
    } 
    
    static Matrix44 perspective(float fovyRadians, float aspect, float nearZ, float farZ)
    {
        float ys = 1 / tanf(fovyRadians * 0.5);
        float xs = ys / aspect;
        float zs = farZ / (nearZ - farZ);
        return Matrix44(xs, 0, 0, 0,
                        0, ys, 0,  0,
                        0, 0, zs, -1,
                        0, 0, nearZ * zs, 0);
    }
    
    static Matrix44 translation(float tx, float ty, float tz)
    {
        return Matrix44(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        tx, ty, tz, 1);
    }
    
    static Matrix44 xRotation(float radians)
    {
        float cos = cosf(radians);
        float sin = sinf(radians);
        return Matrix44(1.0, 0.0, 0.0, 0.0,
                        0.0, cos, sin, 0.0,
                        0.0, -sin, cos, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    }
    
    static Matrix44 yRotation(float radians)
    {
        float cos = cosf(radians);
        float sin = sinf(radians);
        return Matrix44(cos, 0.0, -sin, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        sin, 0.0, cos, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    }
    
 
    static void multiply(const Matrix44<T> &a, const Matrix44<T>& b, Matrix44<T> &c) 
    { 

        const T * __restrict ap = &a.x[0]; 
        const T * __restrict bp = &b.x[0]; 
              T * __restrict cp = &c.x[0]; 
 
        cp[0]  = ap[0] * bp[0]  + ap[4] * bp[1]  + ap[8] * bp[2]   + ap[12] * bp[3];
        cp[4]  = ap[0] * bp[4]  + ap[4] * bp[5]  + ap[8] * bp[6]   + ap[12] * bp[7];
        cp[8]  = ap[0] * bp[8]  + ap[4] * bp[9]  + ap[8] * bp[10]  + ap[12] * bp[11];
        cp[12] = ap[0] * bp[12] + ap[4] * bp[13] + ap[8] * bp[14]  + ap[12] * bp[15];
        
        cp[1]  = ap[1] * bp[0]  + ap[5] * bp[1]  + ap[9] * bp[2]   + ap[13] * bp[3];
        cp[5]  = ap[1] * bp[4]  + ap[5] * bp[5]  + ap[9] * bp[6]   + ap[13] * bp[7];
        cp[9]  = ap[1] * bp[8]  + ap[5] * bp[9]  + ap[9] * bp[10]  + ap[13] * bp[11];
        cp[13] = ap[1] * bp[12] + ap[5] * bp[13] + ap[9] * bp[14]  + ap[13] * bp[15];
        
        cp[2]  = ap[2] * bp[0]  + ap[6] * bp[1]  + ap[10] * bp[2]  + ap[14] * bp[3];
        cp[6]  = ap[2] * bp[4]  + ap[6] * bp[5]  + ap[10] * bp[6]  + ap[14] * bp[7];
        cp[10] = ap[2] * bp[8]  + ap[6] * bp[9]  + ap[10] * bp[10] + ap[14] * bp[11];
        cp[14] = ap[2] * bp[12] + ap[6] * bp[13] + ap[10] * bp[14] + ap[14] * bp[15];
        
        cp[3]  = ap[3] * bp[0]  + ap[7] * bp[1]  + ap[11] * bp[2]  + ap[15] * bp[3];
        cp[7]  = ap[3] * bp[4]  + ap[7] * bp[5]  + ap[11] * bp[6]  + ap[15] * bp[7];
        cp[11] = ap[3] * bp[8]  + ap[7] * bp[9]  + ap[11] * bp[10] + ap[15] * bp[11];
        cp[15] = ap[3] * bp[12] + ap[7] * bp[13] + ap[11] * bp[14] + ap[15] * bp[15];
       
    } 
    
    
    template<typename S>
    Vec3<S> operator * (const Vec3<S> &src) 
    { 
#if   defined(__SSE3__)
                
        float val[4] = {src[0], src[1], src[2], 1.0f};
        const __m128 v = _mm_load_ps(&val[0]);
        
        const __m128 r = _mm_load_ps(&x[0])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0))
        + _mm_load_ps(&x[4])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1))
        + _mm_load_ps(&x[8])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2))
        + _mm_load_ps(&x[12]) * _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
        
        *(__m128*)&val = r;
        return Vec3<S>(val[0], val[1], val[2]);
#endif
        
        Vec3<S> dst;
        S a, b, c, w; 
        a = src[0] * x[0] + src[1] * x[4] + src[2] * x[8] + x[12]; 
        b = src[0] * x[1] + src[1] * x[5] + src[2] * x[9] + x[13]; 
        c = src[0] * x[2] + src[1] * x[6] + src[2] * x[10] + x[14]; 
        w = src[0] * x[3] + src[1] * x[7] + src[2] * x[11] + x[15]; 
        
        dst.x = a / w; 
        dst.y = b / w; 
        dst.z = c / w;
        
        return dst;
    }
 

}; 
 
typedef Matrix44<float> Matrix44f; 

static inline Vec3f cBarycentric(const Vec3f &a, const Vec3f &b, const Vec3f &c, const Vec3f &w) {
    Vec3f result;
    
#if defined(__SSE3__)
    
    float ret[4];
    _mm_storer_ps(&ret[0], _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(w.x), 
                                                            _mm_set_ps(a.x, a.y, a.z, 0.0f)),
                                                 _mm_mul_ps(_mm_set1_ps(w.y), 
                                                            _mm_set_ps(b.x, b.y, b.z, 0.0f))),
                                      _mm_mul_ps(_mm_set1_ps(w.z), 
                                                 _mm_set_ps(c.x, c.y, c.z, 0.0f))));    
    return Vec3f(ret[0], ret[1], ret[2]);
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


static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c) { 
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


static inline Vec3f clamp(  Vec3f& val, float minval, float maxval ) {
    return Vec3f(clamp(val.x, minval, maxval), clamp(val.y, minval, maxval), clamp(val.z, minval, maxval));
}

#endif
