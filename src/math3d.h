#pragma once
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ═══════════════════════════════════════════════
// VEC2
// ═══════════════════════════════════════════════
struct Vec2 {
    float x, y;
    Vec2()                   : x(0), y(0) {}
    Vec2(float x, float y)   : x(x), y(y) {}
    Vec2(float v)            : x(v), y(v) {}
    Vec2 operator+(const Vec2& b) const { return {x+b.x, y+b.y}; }
    Vec2 operator-(const Vec2& b) const { return {x-b.x, y-b.y}; }
    Vec2 operator*(float s)       const { return {x*s,   y*s};   }
    float length() const { return sqrtf(x*x + y*y); }
};

// ═══════════════════════════════════════════════
// VEC3
// ═══════════════════════════════════════════════
struct Vec3 {
    float x, y, z;
    Vec3()                        : x(0), y(0), z(0) {}
    Vec3(float v)                 : x(v), y(v), z(v) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& b) const { return {x+b.x, y+b.y, z+b.z}; }
    Vec3 operator-(const Vec3& b) const { return {x-b.x, y-b.y, z-b.z}; }
    Vec3 operator*(float s)       const { return {x*s,   y*s,   z*s};   }
    Vec3 operator/(float s)       const { return {x/s,   y/s,   z/s};   }
    Vec3 operator-()              const { return {-x,    -y,    -z};     }
    Vec3& operator+=(const Vec3& b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
    Vec3& operator-=(const Vec3& b) { x-=b.x; y-=b.y; z-=b.z; return *this; }

    float dot(const Vec3& b) const {
        return x*b.x + y*b.y + z*b.z;
    }

    Vec3 cross(const Vec3& b) const {
        return {
            y*b.z - z*b.y,
            z*b.x - x*b.z,
            x*b.y - y*b.x
        };
    }

    float length() const {
        return sqrtf(x*x + y*y + z*z);
    }

    Vec3 normalize() const {
        float len = length();
        if (len < 1e-8f) return {0, 0, 0};
        return {x/len, y/len, z/len};
    }

    float& operator[](int i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    const float& operator[](int i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
};

inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

// ═══════════════════════════════════════════════
// VEC4
// ═══════════════════════════════════════════════
struct Vec4 {
    float x, y, z, w;
    Vec4()                                   : x(0), y(0), z(0), w(0) {}
    Vec4(float v)                            : x(v), y(v), z(v), w(v) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w)             : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4 operator+(const Vec4& b) const { return {x+b.x, y+b.y, z+b.z, w+b.w}; }
    Vec4 operator-(const Vec4& b) const { return {x-b.x, y-b.y, z-b.z, w-b.w}; }
    Vec4 operator*(float s)       const { return {x*s,   y*s,   z*s,   w*s};   }
    Vec4 operator/(float s)       const { return {x/s,   y/s,   z/s,   w/s};   }

    Vec3 xyz() const { return {x, y, z}; }
};

// ═══════════════════════════════════════════════
// MAT4 — column-major 4x4 (matches OpenGL)
// m[col][row]
// ═══════════════════════════════════════════════
struct Mat4 {
    float m[4][4];

    // identity by default
    Mat4() {
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                m[c][r] = (c == r) ? 1.f : 0.f;
    }

    // matrix multiply
    Mat4 operator*(const Mat4& b) const {
        Mat4 out;
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++) {
                out.m[c][r] = 0;
                for (int k = 0; k < 4; k++)
                    out.m[c][r] += m[k][r] * b.m[c][k];
            }
        return out;
    }

    // Mat4 * Vec4
    Vec4 operator*(const Vec4& v) const {
        return {
            m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0]*v.w,
            m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1]*v.w,
            m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2]*v.w,
            m[0][3]*v.x + m[1][3]*v.y + m[2][3]*v.z + m[3][3]*v.w
        };
    }

    // raw pointer for glUniformMatrix4fv
    const float* ptr() const { return &m[0][0]; }
};

// ═══════════════════════════════════════════════
// MAT3 — 3x3 matrix
// ═══════════════════════════════════════════════
struct Mat3 {
    float m[3][3];
    Mat3() {
        for (int c = 0; c < 3; c++)
            for (int r = 0; r < 3; r++)
                m[c][r] = (c == r) ? 1.f : 0.f;
    }
    const float* ptr() const { return &m[0][0]; }
};

// ═══════════════════════════════════════════════
// UTILITY FUNCTIONS
// ═══════════════════════════════════════════════

inline float toRad(float deg) {
    return deg * (float)M_PI / 180.f;
}

inline float radians(float deg) {
    return toRad(deg);
}

inline float clamp(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

inline Vec3 clamp(const Vec3& v, float lo, float hi) {
    return { clamp(v.x,lo,hi), clamp(v.y,lo,hi), clamp(v.z,lo,hi) };
}

inline float length(const Vec2& v) { return v.length(); }
inline float length(const Vec3& v) { return v.length(); }

inline Vec3 normalize(const Vec3& v) { return v.normalize(); }

inline float dot(const Vec3& a, const Vec3& b) { return a.dot(b); }

inline Vec3 cross(const Vec3& a, const Vec3& b) { return a.cross(b); }

inline float absf(float v) { return v < 0 ? -v : v; }

inline float sqrtf_safe(float v) { return sqrtf(v < 0 ? 0 : v); }

// ═══════════════════════════════════════════════
// MATRIX OPERATIONS
// ═══════════════════════════════════════════════

// Extract Mat3 from Mat4 (strips translation — used for skybox)
inline Mat3 mat3(const Mat4& M) {
    Mat3 out;
    for (int c = 0; c < 3; c++)
        for (int r = 0; r < 3; r++)
            out.m[c][r] = M.m[c][r];
    return out;
}

// Build Mat4 from Mat3 (used for skybox view matrix)
inline Mat4 mat4(const Mat3& M) {
    Mat4 out;
    for (int c = 0; c < 3; c++)
        for (int r = 0; r < 3; r++)
            out.m[c][r] = M.m[c][r];
    return out;
}

// ── Translation ─────────────────────────────────
// Adds tx,ty,tz into the last column of the matrix
//
// [ 1  0  0  tx ]
// [ 0  1  0  ty ]
// [ 0  0  1  tz ]
// [ 0  0  0   1 ]
inline Mat4 translate(const Mat4& M, const Vec3& t) {
    Mat4 T;
    T.m[3][0] = t.x;
    T.m[3][1] = t.y;
    T.m[3][2] = t.z;
    return M * T;
}

// ── Scale ────────────────────────────────────────
// Puts sx,sy,sz on the diagonal
//
// [ sx  0   0   0 ]
// [  0  sy  0   0 ]
// [  0  0   sz  0 ]
// [  0  0   0   1 ]
inline Mat4 scale(const Mat4& M, const Vec3& s) {
    Mat4 S;
    S.m[0][0] = s.x;
    S.m[1][1] = s.y;
    S.m[2][2] = s.z;
    return M * S;
}

// ── Perspective Projection ───────────────────────
// Built from the frustum equations:
// f = cot(fovy/2) = 1/tan(fovy/2)
//
// [ f/aspect    0        0              0       ]
// [    0        f        0              0       ]
// [    0        0   (zF+zN)/(zN-zF)  2*zF*zN/(zN-zF) ]
// [    0        0       -1              0       ]
inline Mat4 perspective(float fovyDeg, float aspect, float zNear, float zFar) {
    Mat4 P;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            P.m[c][r] = 0.f;

    float f = 1.f / tanf(toRad(fovyDeg) * 0.5f);

    P.m[0][0] = f / aspect;
    P.m[1][1] = f;
    P.m[2][2] = (zFar + zNear) / (zNear - zFar);
    P.m[2][3] = -1.f;
    P.m[3][2] = (2.f * zFar * zNear) / (zNear - zFar);

    return P;
}

// ── View Matrix (lookAt) ─────────────────────────
// Builds orthonormal camera basis:
// forward = normalize(eye - target)
// right   = normalize(up x forward)
// up      = forward x right
//
// [ rx  ry  rz  -dot(r,eye) ]
// [ ux  uy  uz  -dot(u,eye) ]
// [ fx  fy  fz  -dot(f,eye) ]
// [  0   0   0       1      ]
inline Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 f = (eye - target).normalize();   // forward
    Vec3 r = up.cross(f).normalize();      // right
    Vec3 u = f.cross(r);                   // true up

    Mat4 V;
    V.m[0][0]=r.x;  V.m[1][0]=r.y;  V.m[2][0]=r.z;  V.m[3][0]=-r.dot(eye);
    V.m[0][1]=u.x;  V.m[1][1]=u.y;  V.m[2][1]=u.z;  V.m[3][1]=-u.dot(eye);
    V.m[0][2]=f.x;  V.m[1][2]=f.y;  V.m[2][2]=f.z;  V.m[3][2]=-f.dot(eye);
    V.m[0][3]=0;    V.m[1][3]=0;     V.m[2][3]=0;    V.m[3][3]=1;
    return V;
}

// ── Matrix Inverse ───────────────────────────────
// Full 4x4 inverse using cofactor expansion
// Used for ray picking (rayToWater in main.cpp)
inline Mat4 inverse(const Mat4& M) {
    const float* s = &M.m[0][0];
    float inv[16];

    inv[0]  =  s[5]*s[10]*s[15]-s[5]*s[11]*s[14]-s[9]*s[6]*s[15]+s[9]*s[7]*s[14]+s[13]*s[6]*s[11]-s[13]*s[7]*s[10];
    inv[4]  = -s[4]*s[10]*s[15]+s[4]*s[11]*s[14]+s[8]*s[6]*s[15]-s[8]*s[7]*s[14]-s[12]*s[6]*s[11]+s[12]*s[7]*s[10];
    inv[8]  =  s[4]*s[9]*s[15] -s[4]*s[11]*s[13]-s[8]*s[5]*s[15]+s[8]*s[7]*s[13]+s[12]*s[5]*s[11]-s[12]*s[7]*s[9];
    inv[12] = -s[4]*s[9]*s[14] +s[4]*s[10]*s[13]+s[8]*s[5]*s[14]-s[8]*s[6]*s[13]-s[12]*s[5]*s[10]+s[12]*s[6]*s[9];
    inv[1]  = -s[1]*s[10]*s[15]+s[1]*s[11]*s[14]+s[9]*s[2]*s[15]-s[9]*s[3]*s[14]-s[13]*s[2]*s[11]+s[13]*s[3]*s[10];
    inv[5]  =  s[0]*s[10]*s[15]-s[0]*s[11]*s[14]-s[8]*s[2]*s[15]+s[8]*s[3]*s[14]+s[12]*s[2]*s[11]-s[12]*s[3]*s[10];
    inv[9]  = -s[0]*s[9]*s[15] +s[0]*s[11]*s[13]+s[8]*s[1]*s[15]-s[8]*s[3]*s[13]-s[12]*s[1]*s[11]+s[12]*s[3]*s[9];
    inv[13] =  s[0]*s[9]*s[14] -s[0]*s[10]*s[13]-s[8]*s[1]*s[14]+s[8]*s[2]*s[13]+s[12]*s[1]*s[10]-s[12]*s[2]*s[9];
    inv[2]  =  s[1]*s[6]*s[15] -s[1]*s[7]*s[14] -s[5]*s[2]*s[15]+s[5]*s[3]*s[14]+s[13]*s[2]*s[7] -s[13]*s[3]*s[6];
    inv[6]  = -s[0]*s[6]*s[15] +s[0]*s[7]*s[14] +s[4]*s[2]*s[15]-s[4]*s[3]*s[14]-s[12]*s[2]*s[7] +s[12]*s[3]*s[6];
    inv[10] =  s[0]*s[5]*s[15] -s[0]*s[7]*s[13] -s[4]*s[1]*s[15]+s[4]*s[3]*s[13]+s[12]*s[1]*s[7] -s[12]*s[3]*s[5];
    inv[14] = -s[0]*s[5]*s[14] +s[0]*s[6]*s[13] +s[4]*s[1]*s[14]-s[4]*s[2]*s[13]-s[12]*s[1]*s[6] +s[12]*s[2]*s[5];
    inv[3]  = -s[1]*s[6]*s[11] +s[1]*s[7]*s[10] +s[5]*s[2]*s[11]-s[5]*s[3]*s[10]-s[9]*s[2]*s[7]  +s[9]*s[3]*s[6];
    inv[7]  =  s[0]*s[6]*s[11] -s[0]*s[7]*s[10] -s[4]*s[2]*s[11]+s[4]*s[3]*s[10]+s[8]*s[2]*s[7]  -s[8]*s[3]*s[6];
    inv[11] = -s[0]*s[5]*s[11] +s[0]*s[7]*s[9]  +s[4]*s[1]*s[11]-s[4]*s[3]*s[9] -s[8]*s[1]*s[7]  +s[8]*s[3]*s[5];
    inv[15] =  s[0]*s[5]*s[10] -s[0]*s[6]*s[9]  -s[4]*s[1]*s[10]+s[4]*s[2]*s[9] +s[8]*s[1]*s[6]  -s[8]*s[2]*s[5];

    float det = s[0]*inv[0] + s[1]*inv[4] + s[2]*inv[8] + s[3]*inv[12];
    if (det == 0.f) return Mat4();
    det = 1.f / det;

    Mat4 result;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            result.m[i][j] = inv[i*4+j] * det;
    return result;
}

// ── value_ptr equivalent ─────────────────────────
// Already handled by Mat4::ptr() and Mat3::ptr()
// This is just for compatibility if needed
inline const float* value_ptr(const Mat4& m) { return m.ptr(); }
inline const float* value_ptr(const Mat3& m) { return m.ptr(); }