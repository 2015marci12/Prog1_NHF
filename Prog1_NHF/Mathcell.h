#pragma once

#include <stdint.h>
#include <math.h>

//Generalizable vector operations.
#define vecOps(prefix, vec_t, scal_t, compNum) \
    vec_t prefix ## vec ## compNum ## _Add(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] += b.comp[i]; return a; } \
    vec_t prefix ## vec ## compNum ## _Add_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] += b; return a; } \
    vec_t prefix ## vec ## compNum ## _s_Add(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] += b; return a; } \
    vec_t prefix ## vec ## compNum ## _Sub(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] -= b.comp[i]; return a; } \
    vec_t prefix ## vec ## compNum ## _Sub_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] -= b; return a; } \
    vec_t prefix ## vec ## compNum ## _s_Sub(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] = b - a.comp[i]; return a; } \
    vec_t prefix ## vec ## compNum ## _Mul(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] *= b.comp[i]; return a; } \
    vec_t prefix ## vec ## compNum ## _Mul_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] *= b; return a; } \
    vec_t prefix ## vec ## compNum ## _s_Mul(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] *= b; return a; } \
    vec_t prefix ## vec ## compNum ## _Div(vec_t a, vec_t b) { for(int i = 0; i < compNum; i++) a.comp[i] /= b.comp[i]; return a; } \
    vec_t prefix ## vec ## compNum ## _Div_s(vec_t a, scal_t b) { for(int i = 0; i < compNum; i++) a.comp[i] /= b; return a; } \
    vec_t prefix ## vec ## compNum ## _s_Div(scal_t b, vec_t a) { for(int i = 0; i < compNum; i++) a.comp[i] = b / a.comp[i]; return a; } \
    scal_t prefix ## vec ## compNum ## _Max(vec_t a) { scal_t max = 0; for(int i = 0; i < compNum; i++) max = (max < a.comp[i])? a.comp[i] : max; return max; } \
    scal_t prefix ## vec ## compNum ## _Min(vec_t a) { scal_t min = prefix ## vec ## compNum ## _Max(a); for(int i = 0; i < compNum; i++) min = (min > a.comp[i])? a.comp[i] : min; return min; } \
    scal_t prefix ## vec ## compNum ## _Len(vec_t a) { scal_t lensqr = 0; for(int i = 0; i < compNum; i++) lensqr += a.comp[i] * a.comp[i]; return (scal_t)sqrt(lensqr); } \
    scal_t prefix ## vec ## compNum ## _Sum(vec_t a) { scal_t sum = 0; for(int i = 0; i < compNum; i++) sum += a.comp[i]; return sum; } \
    scal_t prefix ## vec ## compNum ## _Dot(vec_t a, vec_t b) { return prefix ## vec ## compNum ## _Sum(prefix ## vec ## compNum ## _Mul(a, b)); }
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
prefix ## vec2 new_ ## prefix ## vec2(scal_t x, scal_t y) { prefix ## vec2 vec; vec.x = x; vec.y = y; return vec; } \
prefix ## vec3 new_ ## prefix ## vec3(scal_t x, scal_t y, scal_t z) { prefix ## vec3 vec; vec.x = x; vec.y = y; vec.z = z; return vec; } \
prefix ## vec4 new_ ## prefix ## vec4(scal_t x, scal_t y, scal_t z, scal_t w) { prefix ## vec4 vec; vec.x = x; vec.y = y; vec.z = z; vec.w = w; return vec; } \
                                        \
vecOps(prefix, prefix ## vec2, scal_t, 2)      \
vecOps(prefix, prefix ## vec3, scal_t, 3)      \
vecOps(prefix, prefix ## vec4, scal_t, 4)      \
                                        \
prefix ## vec2 prefix ## vec2_Rot(prefix ## vec2 a, float angle) { return new_ ## prefix ## vec2(a.x * cos(angle) - a.y * sin(angle), a.x * sin(angle) + a.y * cos(angle)); } \
float prefix ## vec2_Angle(prefix ## vec2 a) { return atan2(a.x, a.y); } 

vecTypes(, float)
vecTypes(d, double)
vecTypes(i, int32_t)
vecTypes(u, uint32_t)

#define matDef(prefix, rows, columns)   \
typedef struct prefix ## mat ## rows ## x ## columns  \
{   \
    prefix ## vec ## columns row[rows];    \
} prefix ## mat ## rows ## x ## columns;  \

#define matOps(prefix, rows, columns, mat_t, scal_t)   \
mat_t prefix ## mat ## rows ## x ## columns ## _Add(mat_t a, mat_t b) { for(int x = 0; x < rows; x++) for(int y = 0; y < columns; y++) a.rows[x].comp[y] += b.rows[x].comp[y]; return a; }  \
mat_t prefix ## mat ## rows ## x ## columns ## _Mul_s(mat_t a, scal_t b) { for(int x = 0; x < rows; x++) for(int y = 0; y < columns; y++) a.rows[x].comp[y] *= b; return a; }  \
mat_t prefix ## mat ## rows ## x ## columns ## _s_Mul(scal_t b, mat_t a) { for(int x = 0; x < rows; x++) for(int y = 0; y < columns; y++) a.rows[x].comp[y] *= b; return a; }  \
prefix ## vec ## columns prefix ## mat ## rows ## x ## columns ## _Mul_v(mat_t a, prefix ## vec ## columns b) { prefix ## vec ## columns ret; for(int x = 0; x < rows; x++) for(int y = 0; y < columns; y++) a.rows[x].comp[y] *= b.comp[y]; return a; }  \
prefix ## mat ## columns ## x ## rows prefix ## mat ## rows ## x ## columns ## _Transpose(mat_t a) { prefix ## mat ## columns ## x ## rows ret; for(int x = 0; x < rows; x++) for(int y = 0; y < columns; y++) ret.rows[y].comp[x] = a.rows[x].comp[y]; return ret; }

#define matMul(prefix, m, n, p) \
prefix ## mat ## m ## x ## p prefix ## mat ## m ## n ## x ## p ## _Mul(prefix ## mat ## m ## n a, prefix ## mat ## n ## x ## p b) \
{ \
    prefix ## mat ## m ## x ## p ret; \
    for(int i = 0; i < m; i++) \
        for(int j = 0; j < p; j++) \
        { \
            ret.rows[i].comp[j] = 0; \
            for(int r = 0; r < n; r++) ret.rows[i].comp[j] += a.rows[i].columns[r] * b.rows[r].columns[j]; \
        }; \
    return ret; \
}

matDef(, 2, 2);
matDef(, 2, 3);
matDef(, 3, 2);
matDef(, 3, 3);
matDef(, 2, 4);
matDef(, 4, 2);
matDef(, 3, 4);
matDef(, 4, 3);
matDef(, 4, 4);

typedef mat4x4 mat4;
typedef mat3x3 mat3;
typedef mat2x2 mat2;

#define PI 

