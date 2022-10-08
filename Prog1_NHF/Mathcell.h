#pragma once

#include <stdint.h>
#include <math.h>

//Generalizable vector operations.
#define vecOps(prefix, vec_t, scal_t, compNum) \
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
    static scal_t prefix ## vec ## compNum ## _Len(vec_t a) { scal_t lensqr = 0; for(int i = 0; i < compNum; i++) lensqr += a.comp[i] * a.comp[i]; return (scal_t)sqrt(lensqr); } \
    static scal_t prefix ## vec ## compNum ## _Sum(vec_t a) { scal_t sum = 0; for(int i = 0; i < compNum; i++) sum += a.comp[i]; return sum; } \
    static scal_t prefix ## vec ## compNum ## _Dot(vec_t a, vec_t b) { return prefix ## vec ## compNum ## _Sum(prefix ## vec ## compNum ## _Mul(a, b)); }
//Vector definitions for different types.
#define vecTypes(prefix, scal_t)        \
typedef union prefix ## vec2            \
{                                       \
    struct { scal_t x, y; };            \
    struct { scal_t r, g; };            \
    scal_t comp[2];               \
} prefix ## vec2;                       \
                                        \
typedef union prefix ## vec3            \
{                                       \
    struct { scal_t x, y, z; };         \
    struct { scal_t r, g, b; };         \
    scal_t comp[3];               \
} prefix ## vec3;                       \
                                        \
typedef union prefix ## vec4            \
{                                       \
    struct { scal_t x, y, z, w; };      \
    struct { scal_t r, g, b, a; };      \
    scal_t comp[4];               \
} prefix ## vec4;                       \
                                        \
static prefix ## vec2 new_ ## prefix ## vec2(scal_t x, scal_t y) { prefix ## vec2 vec; vec.x = x; vec.y = y; return vec; } \
static prefix ## vec3 new_ ## prefix ## vec3(scal_t x, scal_t y, scal_t z) { prefix ## vec3 vec; vec.x = x; vec.y = y; vec.z = z; return vec; } \
static prefix ## vec4 new_ ## prefix ## vec4(scal_t x, scal_t y, scal_t z, scal_t w) { prefix ## vec4 vec; vec.x = x; vec.y = y; vec.z = z; vec.w = w; return vec; } \
                                        \
vecOps(prefix, prefix ## vec2, scal_t, 2)      \
vecOps(prefix, prefix ## vec3, scal_t, 3)      \
vecOps(prefix, prefix ## vec4, scal_t, 4)      \
                                        \
static prefix ## vec2 prefix ## vec2_Rot(prefix ## vec2 a, float angle) { return new_ ## prefix ## vec2((scal_t)(a.x * cosf(angle) - a.y * sinf(angle)), (scal_t)(a.x * sinf(angle) + a.y * cosf(angle))); } \
static float prefix ## vec2_Angle(prefix ## vec2 a) { return (float)atan2((double)a.x, (double)a.y); } \
static prefix ## vec3 prefix ## vec3_Cross(prefix ## vec3 a, prefix ## vec3 b) { return new_ ## prefix ## vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

vecTypes(, float)
vecTypes(d, double)
vecTypes(i, int32_t)
vecTypes(u, uint32_t)

//Matrix operations.
#define matOps(prefix, cols, rows, mat_t, scal_t)   \
static mat_t prefix ## mat ## cols ## x ## rows ## _Add(mat_t a, mat_t b) { for(int x = 0; x < cols; x++) for(int y = 0; y < rows; y++) a.col[x].comp[y] += b.col[x].comp[y]; return a; }  \
static mat_t prefix ## mat ## cols ## x ## rows ## _Mul_s(mat_t a, scal_t b) { for(int x = 0; x < cols; x++) for(int y = 0; y < rows; y++) a.col[x].comp[y] *= b; return a; }  \
static mat_t prefix ## mat ## cols ## x ## rows ## _s_Mul(scal_t b, mat_t a) { for(int x = 0; x < cols; x++) for(int y = 0; y < rows; y++) a.col[x].comp[y] *= b; return a; }  \
static prefix ## vec ## cols prefix ## mat ## cols ## x ## rows ## _Mul_v(mat_t a, prefix ## vec ## rows b) { prefix ## vec ## cols ret; for(int x = 0; x < cols; x++) { ret.comp[x] = 0; for(int y = 0; y < rows; y++) ret.comp[x] += a.col[x].comp[y] * b.comp[y]; } return ret; }  \
static prefix ## mat ## rows ## x ## cols prefix ## mat ## cols ## x ## rows ## _Transpose(mat_t a) { prefix ## mat ## rows ## x ## cols ret; for(int x = 0; x < cols; x++) for(int y = 0; y < rows; y++) ret.col[y].comp[x] = a.col[x].comp[y]; return ret; }

//Matrix definition. Column major.
#define matDef(prefix, cols, rows, scal_t)   \
typedef struct prefix ## mat ## cols ## x ## rows  \
{   \
    prefix ## vec ## rows col[cols];    \
} prefix ## mat ## cols ## x ## rows;  \

//Matrix multiplication for each valid combination.
#define matMul(prefix, m, n, p) \
static prefix ## mat ## m ## x ## p prefix ## mat ## m ## x ## n ## x ## p ## _Mul(prefix ## mat ## m ## x ## n a, prefix ## mat ## n ## x ## p b) \
{ \
    prefix ## mat ## m ## x ## p ret; \
    for(int i = 0; i < m; i++) \
        for(int j = 0; j < p; j++) \
        { \
            ret.col[i].comp[j] = 0; \
            for(int r = 0; r < n; r++) ret.col[i].comp[j] += a.col[i].comp[r] * b.col[r].comp[j]; \
        }; \
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

matTypes(, float);

#define PI 

