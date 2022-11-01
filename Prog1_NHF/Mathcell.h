#pragma once
#include "Core.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/*
* Vector and matrix math.
*/

//Generalizable vector operations.
#define vecOps(prefix, vec_t, scal_t, compNum) \
    static vec_t new_ ## prefix ## vec ## compNum ## _v(scal_t v) { vec_t ret; for(int i = 0; i < compNum; i++) ret.comp[i] = v; return ret; } \
    static vec_t prefix ## vec ## compNum ## _Add(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] += b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Add_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] += b; return a; } \
    static vec_t prefix ## vec ## compNum ## _s_Add(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] += b; return a; } \
    static vec_t prefix ## vec ## compNum ## _Sub(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] -= b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Sub_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] -= b; return a; } \
    static vec_t prefix ## vec ## compNum ## _s_Sub(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] = b - a.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Mul(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] *= b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Mul_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] *= b; return a; } \
    static vec_t prefix ## vec ## compNum ## _s_Mul(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] *= b; return a; } \
    static vec_t prefix ## vec ## compNum ## _Div(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] /= b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Div_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] /= b; return a; } \
    static vec_t prefix ## vec ## compNum ## _s_Div(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] = b / a.comp[i]; return a; } \
    static scal_t prefix ## vec ## compNum ## _Max(vec_t a) { scal_t max = 0; for(int i = 0; i < compNum; i++) max = (max < a.comp[i])? a.comp[i] : max; return max; } \
    static scal_t prefix ## vec ## compNum ## _Min(vec_t a) { scal_t min = prefix ## vec ## compNum ## _Max(a); for(int i = 0; i < compNum; i++) min = (min > a.comp[i])? a.comp[i] : min; return min; } \
    static vec_t prefix ## vec ## compNum ## _Max_v(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] = (a.comp[i] > b.comp[i]) ? a.comp[i] : b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Min_v(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] = (a.comp[i] < b.comp[i]) ? a.comp[i] : b.comp[i]; return a; } \
    static vec_t prefix ## vec ## compNum ## _Abs(vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] = (scal_t)fabs((double)a.comp[i]); return a; } \
    static scal_t prefix ## vec ## compNum ## _Len(vec_t a) { scal_t lensqr = 0; for(int i = 0; i < compNum; i++) lensqr += a.comp[i] * a.comp[i]; return (scal_t)sqrt(lensqr); } \
    static scal_t prefix ## vec ## compNum ## _Sum(vec_t a) { scal_t sum = 0; for(int i = 0; i < compNum; i++) sum += a.comp[i]; return sum; } \
    static scal_t prefix ## vec ## compNum ## _Dot(vec_t a, vec_t b) { return prefix ## vec ## compNum ## _Sum(prefix ## vec ## compNum ## _Mul(a, b)); } \
    static vec_t prefix ## vec ## compNum ## _Normalize(vec_t a) { scal_t len = prefix ## vec ## compNum ## _Len(a); return len != 0.f ? prefix ## vec ## compNum ## _Div_s(a, len) : a; }

#define vecConversion(prefix, scal_t, prefix1, scal_t1, compNum) \
    static prefix ## vec ## compNum prefix1 ## vec ## compNum ## _to_ ## prefix ## vec ## compNum ## (prefix1 ## vec ## compNum a) \
        { prefix ## vec ## compNum ret; for(int i = 0; i < compNum; i++) ret.comp[i] = (scal_t)a.comp[i]; return ret; } \
    static prefix1 ## vec ## compNum prefix ## vec ## compNum ## _to_ ## prefix1 ## vec ## compNum ## (prefix ## vec ## compNum a) \
        { prefix1 ## vec ## compNum ret; for(int i = 0; i < compNum; i++) ret.comp[i] = (scal_t1)a.comp[i]; return ret; } \

#define vecConversions(prefix, scal_t, prefix1, scal_t1) \
    vecConversion(prefix, scal_t, prefix1, scal_t1, 2) \
    vecConversion(prefix, scal_t, prefix1, scal_t1, 3) \
    vecConversion(prefix, scal_t, prefix1, scal_t1, 4) \

//Vector definitions for different types.
#define vecTypes(prefix, scal_t)        \
typedef union prefix ## vec2            \
{                                       \
    struct { scal_t x, y; };            \
    struct { scal_t r, g; };            \
    scal_t comp[2];                     \
} prefix ## vec2;                       \
                                        \
typedef union prefix ## vec3            \
{                                       \
    struct { scal_t x, y, z; };         \
    struct { scal_t r, g, b; };         \
    scal_t comp[3];                     \
} prefix ## vec3;                       \
                                        \
typedef union prefix ## vec4            \
{                                       \
    struct { scal_t x, y, z, w; };      \
    struct { scal_t r, g, b, a; };      \
    scal_t comp[4];                     \
} prefix ## vec4;                       \
                                        \
static prefix ## vec2 new_ ## prefix ## vec2(scal_t x, scal_t y) \
{ \
    prefix ## vec2 vec; \
    vec.x = x; \
    vec.y = y; \
    return vec; \
} \
static prefix ## vec2 new_ ## prefix ## vec2_v3(prefix ## vec3 a) \
{ \
    prefix ## vec2 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    return vec; \
} \
static prefix ## vec2 new_ ## prefix ## vec2_v4(prefix ## vec4 a) \
{ \
    prefix ## vec2 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    return vec; \
} \
static prefix ## vec3 new_ ## prefix ## vec3_v2(prefix ## vec2 a, scal_t z) \
{ \
    prefix ## vec3 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    vec.z = z; \
    return vec; \
} \
static prefix ## vec3 new_ ## prefix ## vec3(scal_t x, scal_t y, scal_t z) \
{ \
    prefix ## vec3 vec; \
    vec.x = x; \
    vec.y = y; \
    vec.z = z; \
    return vec; \
} \
static prefix ## vec3 new_ ## prefix ## vec3_v4(prefix ## vec4 a) \
{ \
    prefix ## vec3 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    vec.z = a.z; \
    return vec; \
} \
static prefix ## vec4 new_ ## prefix ## vec4_v2(prefix ## vec2 a, scal_t z, scal_t w) \
{ \
    prefix ## vec4 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    vec.z = z; \
    vec.w = w; \
    return vec; \
} \
static prefix ## vec4 new_ ## prefix ## vec4_v3(prefix ## vec3 a, scal_t w) \
{ \
    prefix ## vec4 vec; \
    vec.x = a.x; \
    vec.y = a.y; \
    vec.z = a.z; \
    vec.w = w; \
    return vec; \
} \
static prefix ## vec4 new_ ## prefix ## vec4(scal_t x, scal_t y, scal_t z, scal_t w) \
{ \
    prefix ## vec4 vec; \
    vec.x = x; \
    vec.y = y; \
    vec.z = z; \
    vec.w = w; \
    return vec; \
} \
vecOps(prefix, prefix ## vec2, scal_t, 2)      \
vecOps(prefix, prefix ## vec3, scal_t, 3)      \
vecOps(prefix, prefix ## vec4, scal_t, 4)      \
                                        \
static prefix ## vec2 prefix ## vec2_Rot(prefix ## vec2 a, float angle) { return new_ ## prefix ## vec2((scal_t)(a.x * cosf(angle) - a.y * sinf(angle)), (scal_t)(a.x * sinf(angle) + a.y * cosf(angle))); } \
static float prefix ## vec2_Angle(prefix ## vec2 a) { return (float)atan2((double)a.y, (double)a.x); } \
static prefix ## vec3 prefix ## vec3_Cross(prefix ## vec3 a, prefix ## vec3 b) { return new_ ## prefix ## vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

//Vector "template" instantiations.
vecTypes(, float)
vecTypes(d, double)
vecTypes(i, int32_t)
vecTypes(u, uint32_t)

vecConversions(, float, d, double)
vecConversions(, float, i, int32_t)
vecConversions(, float, u, uint32_t)
vecConversions(d, double, i, int32_t)
vecConversions(d, double, u, uint32_t)
vecConversions(i, int32_t, u, uint32_t)


//Matrix operations.
#define matOps(prefix, rows, cols, mat_t, scal_t) \
static mat_t prefix ## mat ## rows ## x ## cols ## _Add(mat_t a, mat_t b) \
{ \
    for(int x = 0; x < cols; x++) \
        for(int y = 0; y < rows; y++) \
        a.col[x].comp[y] += b.col[x].comp[y]; \
    return a; \
}  \
static mat_t prefix ## mat ## rows ## x ## cols ## _Mul_s(mat_t a, scal_t b) \
{ \
    for(int x = 0; x < cols; x++) \
        for(int y = 0; y < rows; y++) \
            a.col[x].comp[y] *= b; \
    return a; \
} \
static mat_t prefix ## mat ## rows ## x ## cols ## _s_Mul(scal_t b, mat_t a) \
{ \
    for(int x = 0; x < cols; x++) \
        for(int y = 0; y < rows; y++) \
            a.col[x].comp[y] *= b; \
    return a; \
} \
static prefix ## vec ## rows prefix ## mat ## rows ## x ## cols ## _Mul_v(mat_t a, prefix ## vec ## cols b) \
{ \
    prefix ## vec ## rows ret; \
    for(int x = 0; x < rows; x++) ret.comp[x] = 0.f; \
    for(int x = 0; x < rows; x++) \
    { \
        for(int y = 0; y < cols; y++) \
            ret.comp[x] += a.col[y].comp[x] * b.comp[y]; \
    } \
    return ret; \
}  \
static prefix ## mat ## cols ## x ## rows prefix ## mat ## rows ## x ## cols ## _Transpose(prefix ## mat ## rows ## x ## cols a) \
{ \
    prefix ## mat ## cols ## x ## rows ret; \
    for(int x = 0; x < cols; x++) \
        for(int y = 0; y < rows; y++) \
            ret.col[y].comp[x] = a.col[x].comp[y]; \
    return ret; \
} \
static mat_t prefix ## mat ## rows ## x ## cols ## _Identity() \
{ \
    mat_t ret; \
    for(int x = 0; x < cols; x++) \
        for(int y = 0; y < rows; y++) \
            ret.col[x].comp[y] = !!(x == y); \
    return ret; \
}

//Matrix definition. Column major.
#define matDef(prefix, rows, cols, scal_t)   \
typedef struct prefix ## mat ## rows ## x ## cols  \
{   \
    prefix ## vec ## rows col[cols];    \
} prefix ## mat ## rows ## x ## cols;  

//Matrix multiplication for each valid combination.
#define matMul(prefix, m, n, p) \
static prefix ## mat ## m ## x ## p prefix ## mat ## m ## x ## n ## x ## p ## _Mul(prefix ## mat ## m ## x ## n a, prefix ## mat ## n ## x ## p b) \
{ \
    prefix ## mat ## m ## x ## p ret; \
    for(int i = 0; i < p; i++) \
        ret.col[i] = prefix ## mat ## m ## x ## n ## _Mul_v(a, b.col[i]); \
    return ret; \
}

//Matrix definitions from 2x2 up to 4x4.
#define matTypes(prefix, scal_t)    \
matDef(prefix, 2, 2, scal_t)    \
matDef(prefix, 2, 3, scal_t)    \
matDef(prefix, 2, 4, scal_t)    \
matDef(prefix, 3, 2, scal_t)    \
matDef(prefix, 3, 3, scal_t)    \
matDef(prefix, 3, 4, scal_t)    \
matDef(prefix, 4, 2, scal_t)    \
matDef(prefix, 4, 3, scal_t)    \
matDef(prefix, 4, 4, scal_t)    \
matOps(prefix, 2, 2, prefix ## mat2x2, scal_t)  \
matOps(prefix, 2, 3, prefix ## mat2x3, scal_t)  \
matOps(prefix, 2, 4, prefix ## mat2x4, scal_t)  \
matOps(prefix, 3, 2, prefix ## mat3x2, scal_t)  \
matOps(prefix, 3, 3, prefix ## mat3x3, scal_t)  \
matOps(prefix, 3, 4, prefix ## mat3x4, scal_t)  \
matOps(prefix, 4, 2, prefix ## mat4x2, scal_t)  \
matOps(prefix, 4, 3, prefix ## mat4x3, scal_t)  \
matOps(prefix, 4, 4, prefix ## mat4x4, scal_t)  \
matMul(prefix, 2, 2, 2) \
matMul(prefix, 2, 3, 2) \
matMul(prefix, 2, 3, 3) \
matMul(prefix, 2, 3, 4) \
matMul(prefix, 2, 4, 2) \
matMul(prefix, 2, 4, 3) \
matMul(prefix, 2, 4, 4) \
matMul(prefix, 3, 2, 2) \
matMul(prefix, 3, 2, 3) \
matMul(prefix, 3, 2, 4) \
matMul(prefix, 3, 3, 2) \
matMul(prefix, 3, 3, 3) \
matMul(prefix, 3, 3, 4) \
matMul(prefix, 3, 4, 2) \
matMul(prefix, 3, 4, 3) \
matMul(prefix, 3, 4, 4) \
matMul(prefix, 4, 2, 2) \
matMul(prefix, 4, 2, 3) \
matMul(prefix, 4, 2, 4) \
matMul(prefix, 4, 3, 2) \
matMul(prefix, 4, 3, 3) \
matMul(prefix, 4, 3, 4) \
matMul(prefix, 4, 4, 2) \
matMul(prefix, 4, 4, 3) \
matMul(prefix, 4, 4, 4) \
typedef prefix ## mat4x4 prefix ## mat4; \
typedef prefix ## mat3x3 prefix ## mat3; \
typedef prefix ## mat2x2 prefix ## mat2; \

//Matrix "template" instantiations.
matTypes(, float);

//Matrix transform functions
static mat4 mat4_Scale(mat4 a, vec3 scale) 
{
    for (int i = 0; i < 3; i++)
        a.col[i] = vec4_Mul_s(a.col[i], scale.comp[i]);
    return a;
}

static mat4 mat4_Translate(mat4 a, vec3 translate)
{
    a.col[3] = 
        vec4_Add(vec4_Mul_s(a.col[0], translate.comp[0]),
            vec4_Add(vec4_Mul_s(a.col[1], translate.comp[1]),
                vec4_Add(vec4_Mul_s(a.col[2], translate.comp[2]),
                    a.col[3])));
    return a;
}

static mat4 mat4_Rotate(mat4 a, float angle, vec3 axis_)
{
    const float c = cosf(angle);
    const float s = sinf(angle);
    const vec3 axis = vec3_Normalize(axis_);
    const vec3 temp = vec3_Mul(new_vec3(1.f - c, 1.f - c, 1.f - c), axis);

    //Algorithm from https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
    mat4 Rotate;
    Rotate.col[0].comp[0] = c + temp.comp[0] * axis.comp[0];
    Rotate.col[0].comp[1] = temp.comp[0] * axis.comp[1] + s * axis.comp[2];
    Rotate.col[0].comp[2] = temp.comp[0] * axis.comp[2] - s * axis.comp[1];

    Rotate.col[1].comp[0] = temp.comp[1] * axis.comp[0] - s * axis.comp[2];
    Rotate.col[1].comp[1] = c + temp.comp[1] * axis.comp[1];
    Rotate.col[1].comp[2] = temp.comp[1] * axis.comp[2] + s * axis.comp[0];

    Rotate.col[2].comp[0] = temp.comp[2] * axis.comp[0] + s * axis.comp[1];
    Rotate.col[2].comp[1] = temp.comp[2] * axis.comp[1] - s * axis.comp[0];
    Rotate.col[2].comp[2] = c + temp.comp[2] * axis.comp[2];

    mat4 Result;
    Result.col[0] = vec4_Add(vec4_Mul_s(a.col[0], Rotate.col[0].comp[0]), vec4_Add(vec4_Mul_s(a.col[1], Rotate.col[0].comp[1]), vec4_Mul_s(a.col[2], Rotate.col[0].comp[2])));
    Result.col[1] = vec4_Add(vec4_Mul_s(a.col[0], Rotate.col[1].comp[0]), vec4_Add(vec4_Mul_s(a.col[1], Rotate.col[1].comp[1]), vec4_Mul_s(a.col[2], Rotate.col[1].comp[2])));
    Result.col[2] = vec4_Add(vec4_Mul_s(a.col[0], Rotate.col[2].comp[0]), vec4_Add(vec4_Mul_s(a.col[1], Rotate.col[2].comp[1]), vec4_Mul_s(a.col[2], Rotate.col[2].comp[2])));
    Result.col[3] = a.col[3];
    return Result;
}

static mat4 mat4_LookAt(mat4 a, vec3 eye, vec3 center, vec3 up) 
{
    //Algorithm from https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
#   ifndef MATH_LHS //Turn on for vulkan and the like.
    const vec3 f = vec3_Normalize(vec3_Sub(eye, center));
    const vec3 s = vec3_Normalize(vec3_Cross(up, f));
    const vec3 u = vec3_Cross(f, s);

    mat4 Result = mat4x4_Identity();
    Result.col[0].comp[0] = s.x;
    Result.col[1].comp[0] = s.y;
    Result.col[2].comp[0] = s.z;
    Result.col[3].comp[0] = -vec3_Dot(s, eye);

    Result.col[0].comp[1] = u.x;
    Result.col[1].comp[1] = u.y;
    Result.col[2].comp[1] = u.z;
    Result.col[3].comp[1] = -vec3_Dot(u, eye);

    Result.col[0].comp[2] = -f.x;
    Result.col[1].comp[2] = -f.y;
    Result.col[2].comp[2] = -f.z;
    Result.col[3].comp[2] = vec3_Dot(f, eye);

    return Result;
#   else
    mat4 Result = mat4x4_Identity();
    Result.col[0].comp[0] = s.x;
    Result.col[1].comp[0] = s.y;
    Result.col[2].comp[0] = s.z;
    Result.col[3].comp[0] = -vec3_Dot(s, eye);

    Result.col[0].comp[1] = u.x;
    Result.col[1].comp[1] = u.y;
    Result.col[2].comp[1] = u.z;
    Result.col[3].comp[1] = -vec3_Dot(u, eye);

    Result.col[0].comp[2] = f.x;
    Result.col[1].comp[2] = f.y;
    Result.col[2].comp[2] = f.z;
    Result.col[3].comp[2] = -vec3_Dot(f, eye);

    return Result;
#   endif
}

static mat4 mat4_Ortho(float left, float right, float top, float bottom, float zNear, float zFar) 
{
#   ifndef MATH_LHS //Turn on for vulkan and the like.
    mat4 Result = mat4x4_Identity();
    Result.col[0].comp[0] =  2.f / (right - left);
    Result.col[1].comp[1] =  2.f / (top - bottom);
    Result.col[2].comp[2] = -2.f / (zFar - zNear);
    Result.col[3].comp[0] = -(right + left) / (right - left);
    Result.col[3].comp[1] = -(top + bottom) / (top - bottom);
    Result.col[3].comp[2] = -(zFar + zNear) / (zFar - zNear);
    return Result;
#   else
    mat4 Result = mat4x4_Identity();
    Result.col[0].comp[0] = 2.f / (right - left);
    Result.col[1].comp[1] = 2.f / (top - bottom);
    Result.col[2].comp[2] = 2.f / (zFar - zNear);
    Result.col[3].comp[0] = -(right + left) / (right - left);
    Result.col[3].comp[1] = -(top + bottom) / (top - bottom);
    Result.col[3].comp[2] = -(zFar + zNear) / (zFar - zNear);
    return Result;
#   endif
}

static mat4 mat4_Inverse(mat4 a) 
{
    //Algorithm from https://github.com/g-truc/glm/blob/master/glm/detail/func_matrix.inl
    float Coef00 = a.col[2].comp[2] * a.col[3].comp[3] - a.col[3].comp[2] * a.col[2].comp[3];
    float Coef02 = a.col[1].comp[2] * a.col[3].comp[3] - a.col[3].comp[2] * a.col[1].comp[3];
    float Coef03 = a.col[1].comp[2] * a.col[2].comp[3] - a.col[2].comp[2] * a.col[1].comp[3];

    float Coef04 = a.col[2].comp[1] * a.col[3].comp[3] - a.col[3].comp[1] * a.col[2].comp[3];
    float Coef06 = a.col[1].comp[1] * a.col[3].comp[3] - a.col[3].comp[1] * a.col[1].comp[3];
    float Coef07 = a.col[1].comp[1] * a.col[2].comp[3] - a.col[2].comp[1] * a.col[1].comp[3];

    float Coef08 = a.col[2].comp[1] * a.col[3].comp[2] - a.col[3].comp[1] * a.col[2].comp[2];
    float Coef10 = a.col[1].comp[1] * a.col[3].comp[2] - a.col[3].comp[1] * a.col[1].comp[2];
    float Coef11 = a.col[1].comp[1] * a.col[2].comp[2] - a.col[2].comp[1] * a.col[1].comp[2];

    float Coef12 = a.col[2].comp[0] * a.col[3].comp[3] - a.col[3].comp[0] * a.col[2].comp[3];
    float Coef14 = a.col[1].comp[0] * a.col[3].comp[3] - a.col[3].comp[0] * a.col[1].comp[3];
    float Coef15 = a.col[1].comp[0] * a.col[2].comp[3] - a.col[2].comp[0] * a.col[1].comp[3];

    float Coef16 = a.col[2].comp[0] * a.col[3].comp[2] - a.col[3].comp[0] * a.col[2].comp[2];
    float Coef18 = a.col[1].comp[0] * a.col[3].comp[2] - a.col[3].comp[0] * a.col[1].comp[2];
    float Coef19 = a.col[1].comp[0] * a.col[2].comp[2] - a.col[2].comp[0] * a.col[1].comp[2];

    float Coef20 = a.col[2].comp[0] * a.col[3].comp[1] - a.col[3].comp[0] * a.col[2].comp[1];
    float Coef22 = a.col[1].comp[0] * a.col[3].comp[1] - a.col[3].comp[0] * a.col[1].comp[1];
    float Coef23 = a.col[1].comp[0] * a.col[2].comp[1] - a.col[2].comp[0] * a.col[1].comp[1];

    vec4 Fac0 = new_vec4(Coef00, Coef00, Coef02, Coef03);
    vec4 Fac1 = new_vec4(Coef04, Coef04, Coef06, Coef07);
    vec4 Fac2 = new_vec4(Coef08, Coef08, Coef10, Coef11);
    vec4 Fac3 = new_vec4(Coef12, Coef12, Coef14, Coef15);
    vec4 Fac4 = new_vec4(Coef16, Coef16, Coef18, Coef19);
    vec4 Fac5 = new_vec4(Coef20, Coef20, Coef22, Coef23);

    vec4 Vec0 = new_vec4(a.col[1].comp[0], a.col[0].comp[0], a.col[0].comp[0], a.col[0].comp[0]);
    vec4 Vec1 = new_vec4(a.col[1].comp[1], a.col[0].comp[1], a.col[0].comp[1], a.col[0].comp[1]);
    vec4 Vec2 = new_vec4(a.col[1].comp[2], a.col[0].comp[2], a.col[0].comp[2], a.col[0].comp[2]);
    vec4 Vec3 = new_vec4(a.col[1].comp[3], a.col[0].comp[3], a.col[0].comp[3], a.col[0].comp[3]);

    vec4 Inv0 = vec4_Add(vec4_Sub(vec4_Mul(Vec1, Fac0), vec4_Mul(Vec2, Fac1)), vec4_Mul(Vec3, Fac2));
    vec4 Inv1 = vec4_Add(vec4_Sub(vec4_Mul(Vec0, Fac0), vec4_Mul(Vec2, Fac3)), vec4_Mul(Vec3, Fac4));
    vec4 Inv2 = vec4_Add(vec4_Sub(vec4_Mul(Vec0, Fac1), vec4_Mul(Vec1, Fac3)), vec4_Mul(Vec3, Fac5));
    vec4 Inv3 = vec4_Add(vec4_Sub(vec4_Mul(Vec0, Fac2), vec4_Mul(Vec1, Fac4)), vec4_Mul(Vec2, Fac5));

    vec4 SignA = new_vec4(+1, -1, +1, -1);
    vec4 SignB = new_vec4(-1, +1, -1, +1);
    mat4 Inverse;
    Inverse.col[0] = vec4_Mul(Inv0, SignA);
    Inverse.col[1] = vec4_Mul(Inv1, SignB);
    Inverse.col[2] = vec4_Mul(Inv2, SignA);
    Inverse.col[3] = vec4_Mul(Inv3, SignB);

    vec4 Row0 = new_vec4(Inverse.col[0].comp[0], Inverse.col[1].comp[0], Inverse.col[2].comp[0], Inverse.col[3].comp[0]);

    vec4 Dot0 = vec4_Mul(a.col[0], Row0);
    float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

    float OneOverDeterminant = 1.f / Dot1;

    return mat4x4_Mul_s(Inverse, OneOverDeterminant);
}

//TODO projection, frustrum, decompose and other matrix funcions.

/*
* Extra helpful things.
*/

typedef union Rect 
{
    struct { float x, y, w, h; };
    struct { vec2 Pos, Size; };
    vec4 rect;
} Rect;

static Rect new_Rect(float x, float y, float w, float h)
{
    Rect ret;
    ret.rect = new_vec4(x, y, w, h);
    return ret;
}

static Rect new_Rect_ps(vec2 Pos, vec2 Size) 
{
    Rect ret;
    ret.Pos = Pos;
    ret.Size = Size;
    return ret;
}

static bool Rect_Contains(Rect r, vec2 p) 
{
    return (p.x >= r.x && p.y >= r.y) && (p.x <= (r.x + r.w) && p.y <= (r.y + r.h));
}

static bool Rect_Intersects(Rect a, Rect b, vec2* n, float* p) 
{
    vec2 diff = vec2_Sub(vec2_Add(b.Pos, vec2_Div_s(b.Size, 2.f)), vec2_Add(a.Pos, vec2_Div_s(a.Size, 2.f)));
    vec2 overlap = vec2_Sub(vec2_Mul_s(vec2_Add(a.Size, b.Size), 0.5f), vec2_Abs(diff));

    //The penetration depth. negative values represent the distance between non-intersecting rectangles.
    if (p) *p = vec2_Min(overlap);
    //The collision normal. Along the axis of least penetration.
    if (n) 
    {
        bool normalalongx = overlap.x < overlap.y;
        (*n).x = ((float)!!normalalongx) * (diff.x / fabsf(diff.x));
        (*n).y = ((float)!!(!normalalongx)) * (diff.y / fabsf(diff.y));
    }

    return (overlap.x > 0 && overlap.y > 0); //Whether the 2 intersect.
}

static Rect Rect_Transformed(mat4 transform, Rect rect) 
{
    vec4 verts[4] =
    {
        mat4x4_Mul_v(transform, new_vec4_v2(rect.Pos, 0.f, 1.f)),
        mat4x4_Mul_v(transform, new_vec4(rect.x + rect.w, rect.y, 0.f, 1.f)),
        mat4x4_Mul_v(transform, new_vec4_v2(vec2_Add(rect.Pos, rect.Size), 0.f, 1.f)),
        mat4x4_Mul_v(transform, new_vec4(rect.x, rect.y + rect.h, 0.f, 1.f)),
    };
    vec2 minPos = new_vec2_v4(verts[0]);
    vec2 maxPos = new_vec2_v4(verts[0]);
    for (int i = 0; i < 4; i++) 
    {
        minPos.x = min(minPos.x, verts[i].x);
        minPos.y = min(minPos.y, verts[i].y);
        maxPos.x = max(maxPos.x, verts[i].x);
        maxPos.y = max(maxPos.y, verts[i].y);
    }
    vec2 Size = vec2_Sub(maxPos, minPos);
    return new_Rect_ps(minPos, Size);
}

static float clamp(float low, float high, float val) 
{
    return max(low, min(high, val));
}

#define PI 3.1415f

