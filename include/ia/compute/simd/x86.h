#define _IA_SIMD_H_// TODO DELETE
#ifndef _IA_SIMD_H_
#error The `x86` SIMD header must not be included outside of `ia/compute/simd.h`
#endif
#define IA_SIMD_X86 1

#include <ia/base/types.h>

typedef __m128  s128f;
typedef __m128d s128d;
typedef __m128i s128i;
#define IA_SIMD_128 1

#ifdef IA_ARCH_X86_AVX
    typedef __m256  s256f;
    typedef __m256d s256d;
    typedef __m256i s256i;
    #define IA_SIMD_256 1
#else
    #define IA_SIMD_256 0
#endif /* IA_ARCH_X86_AVX */

#ifdef IA_SIMD_UNALIGNED
    #define ia_simd_read(p)     _mm_loadu_ps(p)
    #define ia_simd_write(p,a)  _mm_storeu_ps(p,a)
#else
    #define ia_simd_read(p)     _mm_load_ps(p)
    #define ia_simd_write(p,a)  _mm_store_ps(p,a)
#endif /* IA_SIMD_UNALIGNED */

#ifdef IA_ARCH_X86_AVX
    #define ia_simd_shuffle1(xmm, z, y, x, w) \
        _mm_permute_ps((xmm), _MM_SHUFFLE(z, y, x, w))
#else
    #if defined(IA_SIMD_USE_INT_DOMAIN) && IA_ARCH_X86_SSE
        #define ia_simd_shuffle1(xmm, z, y, x, w) \
            _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(xmm), _MM_SHUFFLE(z, y, x, w)))
    #else
        #define ia_simd_shuffle1(xmm, z, y, x, w) \
            _mm_shuffle_ps(xmm, xmm, _MM_SHUFFLE(z, y, x, w))
    #endif
#endif /* IA_ARCH_X86_AVX */

#define ia_simd_splat(x, lane) \
    ia_simd_shuffle1(x, lane, lane, lane, lane)

#ifdef IA_ARCH_X86_AVX
    #define ia_simd_set1(x)         _mm_broadcast_ss(&x)
    #define ia_simd_set1_ptr(x)     _mm_broadcast_ss(x)
    #define ia_simd_set1_rval(x)    _mm_set1_ps(x)

    #ifdef IA_ARCH_X86_AVX2
        #define ia_simd_splat_x(x)  _mm_broadcastss_ps(x)
    #else
        #define ia_simd_splat_x(x)  _mm_permute_ps(x, _MM_SHUFFLE(0, 0, 0, 0))
    #endif 
    #define ia_simd_splat_y(y)      _mm_permute_ps(y, _MM_SHUFFLE(1, 1, 1, 1))
    #define ia_simd_splat_z(z)      _mm_permute_ps(z, _MM_SHUFFLE(2, 2, 2, 2))
    #define ia_simd_splat_w(w)      _mm_permute_ps(w, _MM_SHUFFLE(3, 3, 3, 3))
#else
#define ia_simd_set1(x)             _mm_set1_ps(x)
#define ia_simd_set1_ptr(x)         _mm_set1_ps(*x)
#define ia_simd_set1_rval(x)        _mm_set1_ps(x)
#define ia_simd_splat_x(x)          ia_simd_splat(x, 0)
#define ia_simd_splat_y(y)          ia_simd_splat(y, 1)
#define ia_simd_splat_z(z)          ia_simd_splat(z, 2)
#define ia_simd_splat_w(w)          ia_simd_splat(w, 3)
#endif /* IA_ARCH_X86_AVX */

#define ia_simd_shuffle2(a, b, z0, y0, x0, w0, z1, y1, x1, w1) \
    ia_simd_shuffle1(_mm_shuffle_ps(a, b, _MM_SHUFFLE(z0, y0, x0, w0)), z1, y1, x1, w1)

#ifdef IA_ARCH_X86_AVX
    #ifdef IA_SIMD_UNALIGNED
        #define ia_simd256_read(p)  _mm256_loadu_ps(p)
        #define ia_simd256_write(p) _mm256_storeu_ps(p)
    #endif
    #define ia_simd256_read(p)      _mm256_load_ps(p)
    #define ia_simd256_write(p,a)   _mm256_store_ps(p,a)
#endif /* IA_ARCH_X86_AVX */

/* note that `0x80000000` corresponds to `INT_MIN` for a 32-bit int */
#ifdef IA_ARCH_X86_SSE2
    #define IA_SIMD_NEGZEROf        ((i32)0x80000000) /* -> -0.0f */
    #define IA_SIMD_POSZEROf        ((i32)0x00000000) /* -> +0.0f */
#else
    #ifdef IA_SIMD_FAST_MATH
        union { i32 i; f32 f; } static IA_SIMD_NEGZEROf_TU = { .i = (i32)0x80000000 };
        #define IA_SIMD_NEGZEROf    IA_SIMD_NEGZEROf_TU.f
        #define IA_SIMD_POSZEROf    0.0f
    #else
        #define IA_SIMD_NEGZEROf    -0.0f
        #define IA_SIMD_POSZEROf    0.0f
    #endif
#endif /* IA_ARCH_X86_SSE2 */

#ifdef IA_ARCH_X86_SSE2
    #define IA_SIMD_SIGNMASKf(x,y,z,w) \
        _mm_castsi128_ps(_mm_set_epi32(x,y,z,w))
#else
    #define IA_SIMD_SIGNMASKf(x,y,z,w) \
        _mm_set_ps(x,y,z,w)
#endif /* IA_ARCH_X86_SSE2 */

#define ia_simd_float32x4_SIGNMASK_PNPN \
    IA_SIMD_SIGNMASKf(IA_SIMD_POSZEROf, IA_SIMD_NEGZEROf, IA_SIMD_POSZEROf, IA_SIMD_NEGZEROf)
#define ia_simd_float32x4_SIGNMASK_NPNP \
    IA_SIMD_SIGNMASKf(IA_SIMD_NEGZEROf, IA_SIMD_POSZEROf, IA_SIMD_NEGZEROf, IA_SIMD_POSZEROf)
#define ia_simd_float32x4_SIGNMASK_NPPN \
    IA_SIMD_SIGNMASKf(IA_SIMD_NEGZEROf, IA_SIMD_POSZEROf, IA_SIMD_POSZEROf, IA_SIMD_NEGZEROf)

/* fast math prevents -0.0f to work */
#if defined(IA_ARCH_X86_SSE2)
    #define ia_simd_float32x4_SIGNMASK_NEG  _mm_castsi128_ps(_mm_set1_epi32(IA_SIMD_NEGZEROf))
#else
    #define ia_simd_float32x4_SIGNMASK_NEG  _mm_set1_ps(IA_SIMD_NEGZEROf)
#endif /* IA_ARCH_X86_SSE2 */
#define ia_simd_float32x8_SIGNMASK_NEG      _mm256_castsi256_ps(_mm256_set1_epi32(IA_SIMD_NEGZEROf))

IA_FORCE_INLINE s128f ia_simd_abs(s128f x) 
{ return _mm_andnot_ps(ia_simd_float32x4_SIGNMASK_NEG, x); }

IA_FORCE_INLINE s128f ia_simd_min(s128f a, s128f b) 
{ return _mm_min_ps(a, b); }

IA_FORCE_INLINE s128f ia_simd_max(s128f a, s128f b) 
{ return _mm_max_ps(a, b); }

IA_FORCE_INLINE s128f ia_simd_vhadd(s128f v) 
{
    s128f x0;
    x0 = _mm_add_ps(v, ia_simd_shuffle1(v, 0, 1, 2, 3));
    x0 = _mm_add_ps(x0, ia_simd_shuffle1(x0, 1, 0, 0, 1));
    return x0;
}

IA_FORCE_INLINE s128f ia_simd_vhadds(s128f v)
{ 
    s128f shuf, sums;
#if IA_ARCH_X86_SSE3
    shuf = _mm_movehdup_ps(v);
#else
    shuf = ia_simd_shuffle1(v, 2, 3, 0, 1);
#endif /* IA_ARCH_X86_SSE3 */
    sums = _mm_add_ps(v, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return sums;
}

IA_FORCE_INLINE f32 ia_simd_hadd(s128f v)
{ return _mm_cvtss_f32(ia_simd_vhadds(v)); }

IA_FORCE_INLINE s128f ia_simd_vhmin(s128f v)
{
    s128f x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);   /* [ 2,  3,  2,  3 ] */
    x1 = _mm_min_ps(x0, v);     /* [0|2 1|3 2|3 3|3] */
    x2 = ia_simd_splat(x1, 1);  /* [1|3 1|3 1|3 1|3] */
    return _mm_min_ss(x1, x2);
}

IA_FORCE_INLINE f32 ia_simd_hmin(s128f v)
{ return _mm_cvtss_f32(ia_simd_vhmin(v)); }

IA_FORCE_INLINE s128f ia_simd_vhmax(s128f v)
{
    s128f x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);    /* [ 2,  3,  2,  3 ] */
    x1 = _mm_max_ps(x0, v);      /* [0|2 1|3 2|3 3|3] */
    x2 = ia_simd_splat(x1, 1); /* [1|3 1|3 1|3 1|3] */
    return _mm_max_ss(x1, x2);
}

IA_FORCE_INLINE f32 ia_simd_hmax(s128f v)
{ return _mm_cvtss_f32(ia_simd_vhmax(v)); }

IA_FORCE_INLINE s128f ia_simd_vdots(s128f a, s128f b)
{
#if defined(IA_ARCH_X86_SSE4_1) || defined(IA_ARCH_X86_SSE4_2)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(IA_ARCH_X86_SSE3)
    s128f x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    return ia_simd_vhadds(_mm_mul_ps(a, b));
#endif /* SSE4_1 SSE4_2 */
}

IA_FORCE_INLINE s128f ia_simd_vdot(s128f a, s128f b)
{
#if defined(IA_ARCH_X86_SSE4_1) || defined(IA_ARCH_X86_SSE4_2)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(IA_ARCH_X86_SSE3)
    s128f x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    s128f x0;
    x0 = _mm_mul_ps(a, b);
    x0 = _mm_add_ps(x0, ia_simd_shuffle1(x0, 1, 0, 3, 2));
    return _mm_add_ps(x0, ia_simd_shuffle1(x0, 0, 1, 0, 1));
#endif /* SSE4_1 SSE4_2 */
}

IA_FORCE_INLINE f32 ia_simd_dot(s128f a, s128f b) 
{ return _mm_cvtss_f32(ia_simd_vdots(a, b)); }

IA_FORCE_INLINE f32 ia_simd_norm(s128f a)
{ return _mm_cvtss_f32(_mm_sqrt_ss(ia_simd_vhadds(_mm_mul_ps(a, a)))); }

IA_FORCE_INLINE f32 ia_simd_norm2(s128f a)
{ return _mm_cvtss_f32(ia_simd_vhadds(_mm_mul_ps(a, a))); }

IA_FORCE_INLINE f32 ia_simd_norm_one(s128f a)
{ return _mm_cvtss_f32(ia_simd_vhadds(ia_simd_abs(a))); }

IA_FORCE_INLINE f32 ia_simd_norm_inf(s128f a)
{ return _mm_cvtss_f32(ia_simd_vhmax(ia_simd_abs(a))); }

#if defined(IA_ARCH_X86_SSE2)
IA_FORCE_INLINE s128f ia_simd_read3f(f32x3 v)
{
    s128i xy;
    s128f z;
    xy = _mm_loadl_epi64(ia_assume_aligned_cast(s128i const, v));
    z = _mm_load_ss(&v[2]);
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}

IA_FORCE_INLINE void ia_simd_write3f(f32x3 v, s128f vx)
{
    _mm_storel_pi(ia_assume_aligned_cast(__m64, v), vx);
    _mm_store_ss(&v[2], ia_simd_shuffle1(vx, 2, 2, 2, 2));
}
#endif /* IA_ARCH_X86_SSE2 */

IA_FORCE_INLINE s128f ia_simd_div(s128f a, s128f b)
{ return _mm_div_ps(a, b); }

IA_FORCE_INLINE s128f ia_simd_fmadd(s128f a, s128f b, s128f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm_fmadd_ps(a, b, c);
#else
    return _mm_add_ps(c, _mm_mul_ps(a, b));
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s128f ia_simd_fnmadd(s128f a, s128f b, s128f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm_fnmadd_ps(a, b, c);
#else
    return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s128f ia_simd_fmsub(s128f a, s128f b, s128f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm_fmsub_ps(a, b, c);
#else
    return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s128f ia_simd_fnmsub(s128f a, s128f b, s128f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm_fnmsub_ps(a, b, c);
#else
    return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(a, b), c), ia_simd_float32x4_SIGNMASK_NEG);
#endif /* IA_ARCH_X86_FMA */
}

#ifdef IA_ARCH_X86_AVX
IA_FORCE_INLINE s256f ia_simd256_fmadd(s256f a, s256f b, s256f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm256_fmadd_ps(a, b, c);
#else
    return _mm256_add_ps(c, _mm256_mul_ps(a, b));
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s256f ia_simd256_fnmadd(s256f a, s256f b, s256f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm256_fnmadd_ps(a, b, c);
#else
    return _mm256_sub_ps(c, _mm256_mul_ps(a, b));
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s256f ia_simd256_fmsub(s256f a, s256f b, s256f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm256_fmsub_ps(a, b, c);
#else
    return _mm256_sub_ps(_mm256_mul_ps(a, b), c);
#endif /* IA_ARCH_X86_FMA */
}

IA_FORCE_INLINE s256f ia_simd256_fnmsub(s256f a, s256f b, s256f c)
{
#ifdef IA_ARCH_X86_FMA
    return _mm256_fnmsub_ps(a, b, c);
#else
    return _mm256_xor_ps(_mm256_add_ps(_mm256_mul_ps(a, b), c), ia_simd_float32x8_SIGNMASK_NEG);
#endif /* IA_ARCH_X86_FMA */
}
#endif /* IA_ARCH_X86_AVX */
