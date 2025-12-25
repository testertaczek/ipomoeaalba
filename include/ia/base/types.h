#pragma once
/** @file ia/base/types.h
 *  @brief Basic typedefs and defines for common operations.
 */
#include <ia/base/targets.h>

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdalign.h>
#include <complex.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include <uchar.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef IA_CACHELINE_SIZE
    #if defined(IA_ARCH_POWER) || defined(IA_ARCH_SPARC) || (defined(IA_PLATFORM_APPLE) && defined(IA_ARCH_AARCH64))
        #define IA_CACHELINE_SIZE 128
    #else /* should be valid for most systems */
        #define IA_CACHELINE_SIZE 64
    #endif
#endif /* IA_CACHELINE_SIZE */

#ifndef IA_ALIGN_MAXIMUM
    #ifdef IA_CC_MSVC_VERSION
        #if IA_CC_MSVC_VERSION_CHECK(19,16,0)
            // Visual Studio 2017 or newer don't need an alignment cap
            #define _IA_ALIGN_MAXIMUM 0
        #else 
            #if defined(IA_ARCH_X86) || defined(IA_ARCH_AMD64)
                #if IA_CC_MSVC_VERSION_CHECK(19,14,0)
                    #define _IA_ALIGN_MAXIMUM 64
                #elif IA_CC_MSVC_VERSION_CHECK(16,0,0)
                    #define _IA_ALIGN_MAXIMUM 32
                #else
                    #define _IA_ALIGN_MAXIMUM 16
                #endif
            #elif defined(IA_ARCH_ARM) || defined(IA_ARCH_AARCH64)
                #define _IA_ALIGN_MAXIMUM 8
            #endif
        #endif
    #elif defined(IA_IBM_VERSION)
        #define _IA_ALIGN_MAXIMUM 16
    #else
        #define _IA_ALIGN_MAXIMUM 0
    #endif
#endif /* _IA_ALIGN_MAXIMUM */
#if _IA_ALIGN_MAXIMUM
    #define _IA_ALIGN_CAP(alignment) (((alignment) < (_IA_ALIGN_MAXIMUM)) ? (alignment) : (_IA_ALIGN_MAXIMUM))
#else
    #define _IA_ALIGN_CAP(alignment) (alignment)
#endif 

/* Set alignment rules for a data type. */
#ifndef IA_ALIGNMENT
    #if IA_HAS_ATTRIBUTE(__aligned__)
        #define IA_ALIGNMENT(alignment) __attribute__((__aligned__(_IA_ALIGN_CAP(alignment))))
    #elif defined(IA_CC_MSVC_VERSION)
        #define IA_ALIGNMENT(alignment) __declspec(align(_IA_ALIGN_CAP(alignment)))
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
        #define IA_ALIGNMENT(alignment) _Alignas(_IA_ALIGN_CAP(alignment))
    #else
        #define IA_ALIGNMENT(alignment) /* no alignment */
    #endif
#endif /* IA_ALIGNMENT */
/* Set alignment rules to match the vector extension register size divided by 8. */
#ifndef IA_SIMD_ALIGNMENT
    #ifdef IA_ARCH_X86_AVX
        /* matrix to be used by AVX 256-bit registers */
        #define IA_SIMD_ALIGNMENT IA_ALIGNMENT(32)
    #elif !defined(IA_SIMD_DISABLED)
        /* by default use 16-byte alignment rules for SIMD data */
        #define IA_SIMD_ALIGNMENT IA_ALIGNMENT(16)
    #else /* fallback to no alignment */
        #define IA_SIMD_ALIGNMENT 
    #endif
#endif /* IA_SIMD_ALIGNMENT */
/* Set alignment rules to match the size of the cacheline (64 or 128 bytes). */
#define IA_CACHELINE_ALIGNMENT  IA_ALIGNMENT(IA_CACHELINE_SIZE)

typedef int8_t              i8;
typedef int16_t             i16;
typedef int32_t             i32;
typedef int64_t             i64;

typedef uint8_t             u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

typedef intptr_t            iptr;
typedef uintptr_t           uptr;
typedef ptrdiff_t           isize;
typedef size_t              usize;

typedef float               f32;
typedef double              f64;
typedef double long         f80;

#ifndef ia_assume_aligned
    #if IA_HAS_BUILTIN(__builtin_assume_aligned)
        #define ia_assume_aligned(expr, alignment) __builtin_assume_aligned((expr), (alignment))
    #else
        #define ia_assume_aligned(expr, alignment) (expr)
    #endif
#endif /* ia_assume_aligned */

#ifndef __cplusplus
#define ia_static_cast(T, V)        ((T)(V))
#define ia_const_cast(T, V)         ((T)(V))
#define ia_reinterpret_cast(T, V)   ((T)((uintptr_t)(V)))
#else
#define ia_static_cast(T, V)        (static_cast<T>(V))
#define ia_const_cast(T, V)         (const_cast<T>(V))
#define ia_reinterpret_cast(T, V)   (reinterpret_cast<T>(V))
#endif
#define ia_assume_aligned_cast(T, V) \
    ((T *)(ia_assume_aligned((V),   ia_static_cast(ptrdiff_t,alignof(T)))))

/* Align to a power of 2. */
#define ia_align(x, a)              (ptrdiff_t)(((x) + (a)-1) & ~((a)-1))
/* Align to any alignment. */
#define ia_align_any(x, a)          (ptrdiff_t)((((((size_t)x) - 1) / ((size_t)a)) + 1) * ((size_t)a))

/* 64-bit signed sizeof and alignof */
#define ia_ssizeof(T)               ia_static_cast(ptrdiff_t, sizeof(T))
#define ia_salignof(T)              ia_static_cast(ptrdiff_t, alignof(T))

#if IA_HAS_BUILTIN(__builtin_offsetof)
    #define ia_offsetof(ST, M)      __builtin_offsetof(ST, M)
#else
    #define ia_offsetof(ST, M)      ((size_t)((uint8_t *)&((ST *)0)->M - (uint8_t *)0))
#endif

/* Return a pointer moved by `offset` bytes. */
#define ia_offset_(o, offset)       ia_reinterpret_cast(void *, ia_reinterpret_cast(uintptr_t, (o)) + (offset))
/* Return a pointer moved by `sizeof(T)` bytes. */
#define ia_offset(o, T)             ia_reinterpret_cast(T *, ia_offset_((o), ia_ssizeof(T)))

/* Return a pointer at index times `stride`. */
#define ia_elem_(o, stride, idx)    ia_offset_((o), (stride) * (idx))
/* Return a pointer at index times `sizeof(T)`. */
#define ia_elem(o, T, idx)          ia_reinterpret_cast(T *, ia_elem_((o), ia_ssizeof(T), (idx)))

#define ia_shuffle4(z, y, x, w)     (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))
#define ia_shuffle3(z, y, x)        (((z) << 4) | ((y) << 2) | (x))
#define ia_shuffle2(y, x)           (((y) << 2) | (x))

#define IA_MS_PER_SECOND            1000
#define IA_US_PER_SECOND            1000000
#define IA_NS_PER_SECOND            1000000000LL
#define IA_NS_PER_MS                1000000
#define IA_NS_PER_US                1000
#define IA_SECONDS_TO_NS(s)         (((uint64_t)(s)) * IA_NS_PER_SECOND)
#define IA_NS_TO_SECONDS(ns)        ((ns) / IA_NS_PER_SECOND)
#define IA_MS_TO_NS(ms)             (((uint64_t)(ms)) * IA_NS_PER_MS)
#define IA_NS_TO_MS(ns)             ((ns) / IA_NS_PER_MS)
#define IA_US_TO_NS(us)             (((uint64_t)(us)) * IA_NS_PER_US)
#define IA_NS_TO_US(ns)             ((ns) / IA_NS_PER_US)

#define IA_TIME_OVERFLOW            86400000
#define IA_TIME_LESS(a, b)          ((a) - (b) >= IA_TIME_OVERFLOW)
#define IA_TIME_GREATER(a, b)       ((b) - (a) >= IA_TIME_OVERFLOW)
#define IA_TIME_LESS_EQUAL(a, b)    (!IA_TIME_GREATER(a, b))
#define IA_TIME_GREATER_EQUAL(a, b) (!IA_TIME_LESS(a, b))
#define IA_TIME_DIFFERENCE(a, b)    ((a) - (b) >= IA_TIME_OVERFLOW ? (b) - (a) : (a) - (b))

#ifdef IA_CC_MSVC_VERSION
typedef _Fcomplex                   cmplxf;
typedef _Dcomplex                   cmplx;
typedef _Lcomplex                   cmplxl;
#else
typedef _Complex float              cmplxf;
typedef _Complex double             cmplx;
typedef _Complex double long        cmplxl;
#endif

#define ia_arraysize(a)             (ia_ssizeof(a) / ia_ssizeof(a[0]))
#define ia_lengthof(s)              (ia_arraysize(s) - 1)
#define ia_is_pow2(x)               ((x != 0) && ((x & (x - 1)) == 0))
#define ia_swap(a,b)                { typeof(a) temp = a; a = b; b = temp; }
#define ia_min(x,y)                 (((x) < (y)) ? (x) : (y))
#define ia_max(x,y)                 (((x) > (y)) ? (x) : (y))
#define ia_clamp(x,a,b)             (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define ia_clamp_zo(x)              (ia_clamp(x, 0, 1))

#if IA_HAS_BUILTIN(__builtin_complex)
    #define IA_CMPLXF(re, im)       __builtin_complex((float)(re), (float)(im))
#elif defined(IA_CC_MSVC_VERSION)
    #define IA_CMPLXF(re, im)       (_Fcomplex){(float)(re), (float)(im)}
#elif defined(IA_CC_CLANG_VERSION)
    #define IA_CMPLXF(re, im)       (__extension__(_Complex float){(float)(re), (float)(im)})
#elif defined(_Imaginary_I)
    #define IA_CMPLXF(re, im)       (_Complex float)((float)(re) + _Imaginary_I * (float)(im))
#else
    #define IA_CMPLXF(re, im)       (_Complex float)((float)(re) + _Complex_I * (float)(im))
#endif /* IA_CMPLXF */

#if IA_HAS_BUILTIN(__builtin_complex)
    #define IA_CMPLX(re, im)        __builtin_complex((double)(re), (double)(im))
#elif defined(IA_CC_MSVC_VERSION)
    #define IA_CMPLX(re, im)        (_Dcomplex){(double)(re), (double)(im)}
#elif defined(IA_CC_CLANG_VERSION)
    #define IA_CMPLX(re, im)        (__extension__(_Complex double){(double)(re), (double)(im)})
#elif defined(_Imaginary_I)
    #define IA_CMPLX(re, im)        (_Complex double)((double)(re) + _Imaginary_I * (double)(im))
#else
    #define IA_CMPLX(re, im)        (_Complex double)((double)(re) + _Complex_I * (double)(im))
#endif /* IA_CMPLX */

#if IA_HAS_BUILTIN(__builtin_complex)
    #define IA_CMPLXL(re, im)       __builtin_complex((long double)(re), (long double)(im))
#elif defined(IA_CC_MSVC_VERSION)
    #define IA_CMPLXL(re, im)       (_Lcomplex){(long double)(re), (long double)(im)}
#elif defined(IA_CC_CLANG_VERSION)
    #define IA_CMPLXL(re, im)       (__extension__(_Complex long double){(long double)(re), (long double)(im)})
#elif defined(_Imaginary_I)
    #define IA_CMPLXL(re, im)       (_Complex long double)((long double)(re) + _Imaginary_I * (long double)(im))
#else
    #define IA_CMPLXL(re, im)       (_Complex long double)((long double)(re) + _Complex_I * (long double)(im))
#endif /* IA_CMPLXL */

/* These macros assume an ID representation of 32-bits for index and 32-bits for version. */
#define IA_IDENTIFIER_INDEX_BITS    (32)
#define IA_IDENTIFIER_INDEX_MASK    ((1ull << IA_IDENTIFIER_INDEX_BITS) - 1ull)
#define IA_IDENTIFIER_VERSION_SHIFT IA_IDENTIFIER_INDEX_BITS

#define ia_identifier_raw(IDX, VER) \
    ((ia_identifier)(((u64)(VER) << IA_IDENTIFIER_INDEX_BITS) | ((u64)(IDX) & IA_IDENTIFIER_INDEX_MASK)))
#define ia_identifier_make(T, IDX, VER) \
    ((T){ .id = ia_identifier_raw(IDX, VER) })
#define ia_identifier_cast(T, ID) \
    ((T){ .id = (ID).id })

#define ia_identifier_get_index(ID) \
    ((u32)((ID) & IA_IDENTIFIER_INDEX_MASK))
#define ia_identifier_get_version(ID) \
    ((u32)((ID) >> IA_IDENTIFIER_INDEX_BITS))
#define ia_identifier_set_index(ID, IDX) \
    ((ID) = ((ID) & ~IA_IDENTIFIER_INDEX_MASK) | ((IDX) & IA_IDENTIFIER_INDEX_MASK))
#define ia_identifier_set_version(ID, VER) \
    ((ID) = ((ID) & IA_IDENTIFIER_INDEX_MASK) | ((u64)(VER) << IA_IDENTIFIER_VERSION_SHIFT))
#define ia_identifier_is_empty(ID) \
    (ia_identifier_get_version(ID) == 0)

/** Declares vector and matrix types:
 *  Tx2 Tx3 Tx4; Tm2x2 Tm2x3 Tm2x4; Tm3x2 Tm3x3 Tm3x4; Tm4x2 Tm4x3 Tm4x4. */
#define _IA_DECL_VEC_AND_MAT_TYPES(T, v_alignment, m_alignment) \
    typedef T T ## x2[2]; \
    typedef T T ## x3[3]; \
    typedef v_alignment T T ## x4[4]; \
    typedef v_alignment T ## x2 T ## m2x2[2]; \
    typedef T ## x3 T ## m2x3[2]; \
    typedef T ## x4 T ## m2x4[2]; \
    typedef T ## x2 T ## m3x2[3]; \
    typedef T ## x3 T ## m3x3[3]; \
    typedef T ## x4 T ## m3x4[3]; \
    typedef T ## x2 T ## m4x2[4]; \
    typedef T ## x3 T ## m4x3[4]; \
    typedef m_alignment T ## x4 T ## m4x4[4];

_IA_DECL_VEC_AND_MAT_TYPES(i8,,)
_IA_DECL_VEC_AND_MAT_TYPES(i16,,)
_IA_DECL_VEC_AND_MAT_TYPES(i32, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)
_IA_DECL_VEC_AND_MAT_TYPES(i64, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)

_IA_DECL_VEC_AND_MAT_TYPES(u8,,)
_IA_DECL_VEC_AND_MAT_TYPES(u16,,)
_IA_DECL_VEC_AND_MAT_TYPES(u32, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)
_IA_DECL_VEC_AND_MAT_TYPES(u64, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)

_IA_DECL_VEC_AND_MAT_TYPES(f32, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)
_IA_DECL_VEC_AND_MAT_TYPES(f64, IA_ALIGNMENT(16), IA_SIMD_ALIGNMENT)

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
                        
#define IA_Ef           ((float)IA_E)
#define IA_LOG2Ef       ((float)IA_LOG2E)
#define IA_LOG10Ef      ((float)IA_LOG10E)
#define IA_LN2f         ((float)IA_LN2)
#define IA_LN10f        ((float)IA_LN10)
#define IA_TAUf         ((float)IA_TAU)
#define IA_INV_TAUf     ((float)IA_INV_TAU)
#define IA_PIf          ((float)IA_PI)
#define IA_PI_2f        ((float)IA_PI_2)
#define IA_PI_4f        ((float)IA_PI_4)
#define IA_1_PIf        ((float)IA_1_PI)
#define IA_2_PIf        ((float)IA_2_PI)
#define IA_2_SQRTPIf    ((float)IA_2_SQRTPI)
#define IA_SQRT2f       ((float)IA_SQRT2)
#define IA_SQRT1_2f     ((float)IA_SQRT1_2)

/** IDs may have different representation between modules, and they will be typedef'd as necessary.
 *  Most representations of IDs consist of an unique index in the lower 32-bits and a reference counter
 *  in the upper 32-bits. They may represent entities, pairs, opaque handles to data (like GPU resources)
 *  and may include optional flags. Detailed representations are specific to their module. */
typedef u64 ia_identifier;

/** Used to mask indices of unique values. */
typedef u32 ia_bitmask;

/** Represents bitsets for different systems. Live values are usually baked up as atomic. */
typedef u32 ia_flags;

/** Represents a device address, big enough to fit any CPU/GPU hardware pointer. */
typedef u64 ia_address;

typedef struct ia_strided_address_region {
    ia_address  address;
    u64         stride;
    u64         size;
} ia_strided_address_region;

IA_FORCE_INLINE void 
ia_force_evalf(f32 x) 
{ volatile f32 y; y = x; (void)y; }

IA_FORCE_INLINE void 
ia_force_eval(f64 x) 
{ volatile f64 y; y = x; (void)y; }

IA_FORCE_INLINE void 
ia_force_evall(f80 x) 
{ volatile f80 y; y = x; (void)y; }

#define IA_FORCE_EVAL(x) do {                   \
        if (sizeof(x) == sizeof(f32)) {         \
            ia_force_evalf(x);                  \
        } else if (sizeof(x) == sizeof(f64)) {  \
            ia_force_eval(x);                   \
        } else {                                \
            ia_force_evall(x);                  \
        }                                       \
    } while(0)

/** Callback used for sorting values. Prototype defined by `IA_COMPARE_OP_FN`. */
typedef i32 (IA_CALL *ia_compare_op_fn)(void const *ptr1, void const *ptr2);
#define IA_COMPARE_OP_FN(fn, T) \
    i32 IA_CALL fn(T const *ptr1, T const *ptr2)

/** Callback used for hashing values. Prototype defined by `IA_HASH_VALUE_OP_FN. */
typedef u64 (IA_CALL *ia_hash_value_op_fn)(void const *ptr);
#define IA_HASH_VALUE_OP_FN(fn, T) \
    u64 IA_CALL fn(T const *ptr)

typedef struct ia_viewport { 
    f32 x, y;
    f32 width, height; 
    f32 min_depth, max_depth; 
} ia_viewport;

typedef union ia_offset2d { 
    struct { 
        i32 x, y; 
    };
    i32x2 v;
} ia_offset2d;

typedef union ia_offset3d { 
    struct {
        i32 x, y, z; 
    };
    i32x3 v;
} ia_offset3d;

typedef union ia_extent2d { 
    struct {
        u32 width, height; 
    };
    u32x2 v;
} ia_extent2d;

typedef union ia_extent3d { 
    struct {
        u32 width, height, depth; 
    };
    u32x3 v;
} ia_extent3d;

typedef struct ia_rect2d { 
    ia_offset2d offset; 
    ia_extent2d extent; 
} ia_rect2d;

typedef struct ia_list_node {
    struct ia_list_node *next;
    struct ia_list_node *prev;
} ia_list_node;

typedef ia_list_node *ia_list_iter;

typedef struct ia_list {
    ia_list_node sentinel;
} ia_list;

#define ia_list_begin(list) ((list)->sentinel.next)
#define ia_list_end(list)   (&(list)->sentinel)
#define ia_list_empty(list) (ia_list_begin(list) == ia_list_end(list))
#define ia_list_next(iter)  ((iter)->next)
#define ia_list_prev(iter)  ((iter)->prev)
#define ia_list_front(list) ((void *)(list)->sentinel.next)
#define ia_list_back(list)  ((void *)(list)->sentinel.prev)

typedef struct ia_buffer {
    void   *data;
    usize   size;   /**< Size in bytes. */
} ia_buffer;

typedef enum ia_sample_count : u32 {
    ia_sample_count_1   = (1u << 0),   
    ia_sample_count_2   = (1u << 1),   
    ia_sample_count_4   = (1u << 2),   
    ia_sample_count_8   = (1u << 3),   
    ia_sample_count_16  = (1u << 4),   
    ia_sample_count_32  = (1u << 5),   
    ia_sample_count_64  = (1u << 6),   
} ia_sample_count;

typedef union ia_color_value {
    f32 f[4];
    i32 i[4];
    u32 u[4];
} ia_color_value;

typedef struct ia_depth_stencil_value {
    f32 depth;
    u32 stencil;
} ia_depth_stencil_value;

typedef union ia_clear_value {
    ia_color_value          color;
    ia_depth_stencil_value  depth_stencil;
} ia_clear_value;

typedef struct ia_depth_bias {
    f32 constant_factor;
    f32 clamp;
    f32 slope_factor;
} ia_depth_bias;

typedef enum ia_index_format : i8 {
    ia_index_format_invalid = 0,
    ia_index_format_u16,
    ia_index_format_u32,
} ia_index_format;

typedef enum ia_compare_op : i8 {
    ia_compare_op_never = 0,
    ia_compare_op_less,
    ia_compare_op_equal,
    ia_compare_op_less_or_equal,
    ia_compare_op_not_equal,
    ia_compare_op_greater,
    ia_compare_op_greater_or_equal,
    ia_compare_op_always,
} ia_compare_op;

typedef enum ia_read_op : i8 {
    ia_read_op_load = 0,
    ia_read_op_clear,
    ia_read_op_dont_care,
    ia_read_op_none,
} ia_read_op;

typedef enum ia_write_op : i8 {
    ia_write_op_store = 0,
    ia_write_op_dont_care,
    ia_write_op_none,
} ia_store_op;

typedef enum ia_stencil_op : i8 {
    ia_stencil_op_keep = 0,
    ia_stencil_op_zero,
    ia_stencil_op_replace,
    ia_stencil_op_increment,
    ia_stencil_op_decrement,
    ia_stencil_op_increment_wrap,
    ia_stencil_op_decrement_wrap,
    ia_stencil_op_invert,
} ia_stencil_op;

typedef enum ia_stencil_face : i8 {
    ia_stencil_face_front = 0,
    ia_stencil_face_back,
} ia_stencil_face;

typedef enum ia_sharing_mode : i8 {
    ia_sharing_mode_exclusive = 0,
    ia_sharing_mode_concurrent,
} ia_sharing_mode;

typedef enum ia_filter_mode : i8 {
    ia_filter_mode_nearest = 0,
    ia_filter_mode_linear,
} ia_filter_mode;

typedef enum ia_result {
    ia_result_success = 0,
    ia_result_unknown_error = -1,
} ia_result;

#ifdef __cplusplus
}
#endif /* __cplusplus */
