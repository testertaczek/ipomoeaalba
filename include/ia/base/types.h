#pragma once
/** @file ia/base/types.h
 *  @brief TODO docs
 */
#include <ia/base/targets.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;
typedef double long f80;

typedef intptr_t    iptr;
typedef uintptr_t   uptr;
typedef ptrdiff_t   isize;
typedef size_t      usize;

/** Declares API function visibility for DLL builds. */
#if !defined(IA_API)
    #ifdef IA_BUILD_DLL_EXPORT
        #if defined(IA_PLATFORM_WINDOWS)
            #define IA_API extern __declspec(dllexport)
        #elif defined(IA_CC_GNUC_VERSION)
            #define IA_API extern __attribute__((visibility("default")))
        #else
            #define IA_API extern
        #endif
    #else
        #define IA_API extern
    #endif
#endif

/* declares the calling convention */
#if !defined(IA_CALL)
    #ifdef IA_PLATFORM_WINDOWS
        /* on windows use the stdcall convention */
        #define IA_CALL __stdcall
    #else
        /* on other platforms use the default calling convention */
        #define IA_CALL
    #endif
#endif

#ifndef __cplusplus
    #define ia_static_cast(T, V)      ((T)(V))
    #define ia_const_cast(T, V)       ((T)(V))
    #define ia_reinterpret_cast(T, V) ((T)((uintptr_t)(V)))
#else
    #define ia_static_cast(T, V)      (static_cast<T>(V))
    #define ia_const_cast(T, V)       (const_cast<T>(V))
    #define ia_reinterpret_cast(T, V) (reinterpret_cast<T>(V))
#endif

#if IA_HAS_BUILTIN(__builtin_offsetof)
    #define ia_offsetof(ST, M) __builtin_offsetof(ST, M)
#else
    #define ia_offsetof(ST, M) ((size_t)((uint8_t *)&((ST *)0)->M - (uint8_t *)0))
#endif

#define IA_EPSILON      DBL_EPSILON
#define IA_EPSILONf     FLT_EPSILON
                        
#define IA_E            2.71828182845904523536 /* e          */
#define IA_LOG2E        1.44269504088896340735 /* log2(e)    */
#define IA_LOG10E       0.43429448190325182765 /* log10(e)   */
#define IA_LN2          0.69314718055994530941 /* loge(2)    */
#define IA_LN10         2.30258509299404568401 /* loge(10)   */
#define IA_TAU          6.28318530717958647692 /* 2pi        */
#define IA_INV_TAU      0.15915494309189533577 /* 1/2pi      */
#define IA_PI           3.14159265358979323846 /* pi         */
#define IA_PI_2         1.57079632679489661923 /* pi/2       */
#define IA_PI_4         0.78539816339744830961 /* pi/4       */
#define IA_1_PI         0.31830988618379067153 /* 1/pi       */
#define IA_2_PI         0.63661977236758134307 /* 2/pi       */
#define IA_2_SQRTPI     1.12837916709551257389 /* 2/sqrt(pi) */
#define IA_SQRT2        1.41421356237309504880 /* sqrt(2)    */
#define IA_SQRT1_2      0.70710678118654752440 /* 1/sqrt(2)  */
                        
#define IA_Ef           ((f32)IA_E)
#define IA_LOG2Ef       ((f32)IA_LOG2E)
#define IA_LOG10Ef      ((f32)IA_LOG10E)
#define IA_LN2f         ((f32)IA_LN2)
#define IA_LN10f        ((f32)IA_LN10)
#define IA_TAUf         ((f32)IA_TAU)
#define IA_INV_TAUf     ((f32)IA_INV_TAU)
#define IA_PIf          ((f32)IA_PI)
#define IA_PI_2f        ((f32)IA_PI_2)
#define IA_PI_4f        ((f32)IA_PI_4)
#define IA_1_PIf        ((f32)IA_1_PI)
#define IA_2_PIf        ((f32)IA_2_PI)
#define IA_2_SQRTPIf    ((f32)IA_2_SQRTPI)
#define IA_SQRT2f       ((f32)IA_SQRT2)
#define IA_SQRT1_2f     ((f32)IA_SQRT1_2)
