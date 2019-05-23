/*This program illustrates how the concept of vector and matrix can be implemented in C++. This is a light version of the implementation. It contains the most essential methods to manipulate vectors and matrices. It should be enough for most projects. Vectors and matrices are really the alphabet as we said in the lesson of any graphics application. It's really important you feel confortable with these techniques especially with the concepts of normalizing vectors, computing their length, computing the dot and cross products of two vectors, and the point- and vector-matrix multiplication (and knowing the difference between the two).
Instructions to compile this program:
c++ geometry.cpp  -o geometry -std=c++11
*/

#ifndef _MATH_H_
#define _MATH_H_
#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <iomanip> 
#include <cmath> 
 
template<typename T> 
class Vec2 
{ 
public: 
    Vec2() : x(0), y(0) {} 
    Vec2(T xx) : x(xx), y(xx) {} 
    Vec2(T xx, T yy) : x(xx), y(yy) {} 
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
 
// Now you can specialize the class. We are just showing two examples here. In your code you can declare a vector either that way: Vec3<float> a, or that way: Vec3f a
typedef Vec3<float> Vec3f; 
typedef Vec3<int> Vec3i;
typedef Vec3<unsigned char> Vec3c;
 
// Implementation of a generic 4x4 Matrix class - Same thing here than with the Vec3 class. It uses a template which is maybe less useful than with vectors but it can be used to define the coefficients of the matrix to be either floats (the most case) or doubles depending on our needs.
// To use you can either write: Matrix44<float> m; or: Matrix44f m;
template<typename T> 
class Matrix44 
{ 
public: 
 
    T x[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}; 
 
    Matrix44() {} 
 
    Matrix44 (T a, T b, T c, T d, T e, T f, T g, T h, 
              T i, T j, T k, T l, T m, T n, T o, T p) 
    { 
        x[0][0] = a; 
        x[0][1] = b; 
        x[0][2] = c; 
        x[0][3] = d; 
        x[1][0] = e; 
        x[1][1] = f; 
        x[1][2] = g; 
        x[1][3] = h; 
        x[2][0] = i; 
        x[2][1] = j; 
        x[2][2] = k; 
        x[2][3] = l; 
        x[3][0] = m; 
        x[3][1] = n; 
        x[3][2] = o; 
        x[3][3] = p; 
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
    
 
// To make it easier to understand how a matrix multiplication works, the fragment of code included within the #if-#else statement, show how this works if you were to iterate over the coefficients of the resulting matrix (a). However you will often see this multiplication being done using the code contained within the #else-#end statement. It is exactly the same as the first fragment only we have litteraly written down as a series of operations what would actually result from executing the two for() loops contained in the first fragment. It is supposed to be faster, however considering matrix multiplicatin is not necessarily that common, this is probably not super useful nor really necessary (but nice to have -- and it gives you an example of how it can be done, as this how you will this operation implemented in most libraries).
    static void multiply(const Matrix44<T> &a, const Matrix44<T>& b, Matrix44<T> &c) 
    { 
#if 0 
        for (uint8_t i = 0; i < 4; ++i) { 
            for (uint8_t j = 0; j < 4; ++j) { 
                c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + 
                    a[i][2] * b[2][j] + a[i][3] * b[3][j]; 
            } 
        } 
#else 
        // A restric qualified pointer (or reference) is basically a promise
        // to the compiler that for the scope of the pointer, the target of the
        // pointer will only be accessed through that pointer (and pointers
        // copied from it.
        const T * __restrict ap = &a.x[0][0]; 
        const T * __restrict bp = &b.x[0][0]; 
              T * __restrict cp = &c.x[0][0]; 
 
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
        
#endif 
    } 
    
 
    // \brief return a transposed copy of the current matrix as a new matrix
    Matrix44 transposed() const 
    { 
#if 0 
        Matrix44 t; 
        for (uint8_t i = 0; i < 4; ++i) { 
            for (uint8_t j = 0; j < 4; ++j) { 
                t[i][j] = x[j][i]; 
            } 
        } 
 
        return t; 
#else 
        return Matrix44 (x[0][0], 
                         x[1][0], 
                         x[2][0], 
                         x[3][0], 
                         x[0][1], 
                         x[1][1], 
                         x[2][1], 
                         x[3][1], 
                         x[0][2], 
                         x[1][2], 
                         x[2][2], 
                         x[3][2], 
                         x[0][3], 
                         x[1][3], 
                         x[2][3], 
                         x[3][3]); 
#endif 
    } 
 
    // \brief transpose itself
    Matrix44& transpose () 
    { 
        Matrix44 tmp (x[0][0], 
                      x[1][0], 
                      x[2][0], 
                      x[3][0], 
                      x[0][1], 
                      x[1][1], 
                      x[2][1], 
                      x[3][1], 
                      x[0][2], 
                      x[1][2], 
                      x[2][2], 
                      x[3][2], 
                      x[0][3], 
                      x[1][3], 
                      x[2][3], 
                      x[3][3]); 
        *this = tmp; 
 
        return *this; 
    } 
 
// This method needs to be used for point-matrix multiplication. Keep in mind we don't make the distinction between points and vectors at least from a programming point of view, as both (as well as normals) are declared as Vec3. However, mathematically they need to be treated differently. Points can be translated when translation for vectors is meaningless. Furthermore, points are implicitly be considered as having homogeneous coordinates. Thus the w coordinates needs to be computed and to convert the coordinates from homogeneous back to Cartesian coordinates, we need to divided x, y z by w.
// The coordinate w is more often than not equals to 1, but it can be different than 1 especially when the matrix is projective matrix (perspective projection matrix).
//    template<typename S> 
//    void multVecMatrix(const Vec3<S> &src, Vec3<S> &dst) const 
//    { 
//        S a, b, c, w; 
// 
//        a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0] + x[3][0]; 
//        b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1] + x[3][1]; 
//        c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2] + x[3][2]; 
//        w = src[0] * x[0][3] + src[1] * x[1][3] + src[2] * x[2][3] + x[3][3]; 
// 
//        dst.x = a / w; 
//        dst.y = b / w; 
//        dst.z = c / w; 
//    } 
    
    template<typename S>
    Vec3<S> operator * (const Vec3<S> &src) 
    { 
        Vec3<S> dst;
        
        S a, b, c, w; 
        
        a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0] + x[3][0]; 
        b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1] + x[3][1]; 
        c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2] + x[3][2]; 
        w = src[0] * x[0][3] + src[1] * x[1][3] + src[2] * x[2][3] + x[3][3]; 
        
        dst.x = a / w; 
        dst.y = b / w; 
        dst.z = c / w; 
        return dst;
    }
 
// This method needs to be used for vector-matrix multiplication. Look at the differences with the previous method (to compute a point-matrix multiplication). We don't use the coefficients in the matrix that account for translation (x[3][0], x[3][1], x[3][2]) and we don't compute w.
    template<typename S> 
    void multDirMatrix(const Vec3<S> &src, Vec3<S> &dst) const 
    { 
        S a, b, c; 
 
        a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0]; 
        b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1]; 
        c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2]; 
 
        dst.x = a; 
        dst.y = b; 
        dst.z = c; 
    } 
 
// Compute the inverse of the matrix using the Gauss-Jordan (or reduced row) elimination method. We didn't explain in the lesson on Geometry how the inverse of matrix can be found. Don't worry at this point if you don't understand how this works. But we will need to be able to compute the inverse of matrices in the first lessons of the "Foundation of 3D Rendering" section, which is why we've added this code. For now, you can just use it and rely on it for doing what it's supposed to do. If you want to learn how this works though, check the lesson on called Matrix Inverse in the "Mathematics and Physics of Computer Graphics" section.
    Matrix44 inverse() const 
    { 
        int i, j, k; 
        Matrix44 s; 
        Matrix44 t (*this); 
 
        // Forward elimination
        for (i = 0; i < 3 ; i++) { 
            int pivot = i; 
 
            T pivotsize = t[i][i]; 
 
            if (pivotsize < 0) 
                pivotsize = -pivotsize; 
 
                for (j = i + 1; j < 4; j++) { 
                    T tmp = t[j][i]; 
 
                    if (tmp < 0) 
                        tmp = -tmp; 
 
                        if (tmp > pivotsize) { 
                            pivot = j; 
                            pivotsize = tmp; 
                        } 
                } 
 
            if (pivotsize == 0) { 
                // Cannot invert singular matrix
                return Matrix44(); 
            } 
 
            if (pivot != i) { 
                for (j = 0; j < 4; j++) { 
                    T tmp; 
 
                    tmp = t[i][j]; 
                    t[i][j] = t[pivot][j]; 
                    t[pivot][j] = tmp; 
 
                    tmp = s[i][j]; 
                    s[i][j] = s[pivot][j]; 
                    s[pivot][j] = tmp; 
                } 
            } 
 
            for (j = i + 1; j < 4; j++) { 
                T f = t[j][i] / t[i][i]; 
 
                for (k = 0; k < 4; k++) { 
                    t[j][k] -= f * t[i][k]; 
                    s[j][k] -= f * s[i][k]; 
                } 
            } 
        } 
 
        // Backward substitution
        for (i = 3; i >= 0; --i) { 
            T f; 
 
            if ((f = t[i][i]) == 0) { 
                // Cannot invert singular matrix
                return Matrix44(); 
            } 
 
            for (j = 0; j < 4; j++) { 
                t[i][j] /= f; 
                s[i][j] /= f; 
            } 
 
            for (j = 0; j < i; j++) { 
                f = t[j][i]; 
 
                for (k = 0; k < 4; k++) { 
                    t[j][k] -= f * t[i][k]; 
                    s[j][k] -= f * s[i][k]; 
                } 
            } 
        } 
 
        return s; 
    } 
 
    // \brief set current matrix to its inverse
    const Matrix44<T>& invert() 
    { 
        *this = inverse(); 
        return *this; 
    } 
 
    friend std::ostream& operator << (std::ostream &s, const Matrix44 &m) 
    { 
        std::ios_base::fmtflags oldFlags = s.flags(); 
        int width = 12; // total with of the displayed number 
        s.precision(5); // control the number of displayed decimals 
        s.setf (std::ios_base::fixed); 
 
        s << "[" << std::setw (width) << m[0][0] << 
             " " << std::setw (width) << m[0][1] << 
             " " << std::setw (width) << m[0][2] << 
             " " << std::setw (width) << m[0][3] << "\n" << 
 
             " " << std::setw (width) << m[1][0] << 
             " " << std::setw (width) << m[1][1] << 
             " " << std::setw (width) << m[1][2] << 
             " " << std::setw (width) << m[1][3] << "\n" << 
 
             " " << std::setw (width) << m[2][0] << 
             " " << std::setw (width) << m[2][1] << 
             " " << std::setw (width) << m[2][2] << 
             " " << std::setw (width) << m[2][3] << "\n" << 
 
             " " << std::setw (width) << m[3][0] << 
             " " << std::setw (width) << m[3][1] << 
             " " << std::setw (width) << m[3][2] << 
             " " << std::setw (width) << m[3][3] << "]"; 
 
        s.flags (oldFlags); 
        return s; 
    } 
}; 
 
typedef Matrix44<float> Matrix44f; 
 

static inline float min3(const float &a, const float &b, const float &c) { 
    return std::min(a, std::min(b, c)); 
} 

static inline float max3(const float &a, const float &b, const float &c) { 
    return std::max(a, std::max(b, c)); 
} 

static inline float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c) { 
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
}



// Testing our code. To test the matrix inversion code, we used Maya to output the values of a matrix and its inverse (check the video at the top of this page). Of course this implies that Maya actually does the right thing, but we can probably agree, that is actually does;). These are the values for the input matrix:
// 0.707107 0 -0.707107 0 -0.331295 0.883452 -0.331295 0 0.624695 0.468521 0.624695 0 4.000574 3.00043 4.000574 1
// Given the input matrix, the inverse matrix computed by our code should match the following values:
// 0.707107 -0.331295 0.624695 0 0 0.883452 0.468521 0 -0.707107 -0.331295 0.624695 0 0 0 -6.404043 1
#if 0 
int main(int argc, char **argv) 
{ 
    Vec3f v(0, 1, 2); 
    std::cerr << v << std::endl; 
    Matrix44f a, b, c; 
    c = a * b; 
 
    Matrix44f d(0.707107, 0, -0.707107, 0, -0.331295, 0.883452, -0.331295, 0, 0.624695, 0.468521, 0.624695, 0, 4.000574, 3.00043, 4.000574, 1); 
    std::cerr << d << std::endl; 
    d.invert(); 
    std::cerr << d << std::endl; 
 
    return 0; 
} 
#endif 

#endif
