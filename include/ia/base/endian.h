#pragma once
/** @file ia/base/endian.h
 *  @brief CPU endianness.
 * 
 *  TODO docs
 */
#include <ia/base/types.h>

#define _IA_BYTEORDER_LE 1234
#define _IA_BYTEORDER_BE 4321

#ifndef IA_BYTEORDER
    #if defined(IA_PLATFORM_LINUX)
        #include <endian.h>
        #define IA_BYTEORDER __BYTE_ORDER
    #elif defined(IA_PLATFORM_SOLARIS)
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define IA_BYTEORDER _IA_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define IA_BYTEORDER _IA_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(IA_PLATFORM_OPENBSD) || defined(__DragonFly__)
        #include <endian.h>
        #define IA_BYTEORDER BYTE_ORDER
    #elif defined(IA_PLATFORM_FREEBSD) || defined(IA_PLATFORM_NETBSD)
        #include <sys/endian.h>
        #define IA_BYTEORDER BYTE_ORDER
    // predefs from newer GCC and CLANG versions
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define IA_BYTEORDER _IA_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define IA_BYTEORDER _IA_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        #if defined(IA_ARCH_MIPS) || defined(IA_ARCH_POWER) || defined(IA_ARCH_SPARC)
            #define IA_BYTEORDER _IA_BYTEORDER_BE
        #else
            #define IA_BYTEORDER _IA_BYTEORDER_LE
        #endif
    #endif
#endif

IA_FORCE_INLINE u16 
ia_bswap16(u16 x) 
{
#if IA_HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif defined(IA_ARCH_AMD64)
    __asm__("xchgb 0, %h0" : "=Q" (x) : "0" (x));
    return x;
#elif defined(IA_ARCH_X86)
    __asm__("rorw $8, %0" : "+r" (x));
    return x;
#else
    return ia_static_cast(u16, ((x << 8) | (x >> 8)));
#endif
}

IA_FORCE_INLINE u32
ia_bswap32(u32 x) 
{
#if IA_HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif defined(IA_ARCH_AMD64)
    __asm__("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(IA_ARCH_X86)
    #if IA_ARCH_X86_CHECK(4)
        __asm__("bswap %0" : "+r" (x));
        return x;
    #else /* i386 has broken bswap */
        __asm__("rorw $8, %w0 ; rorl $16, %0 ; rorw $8, %w0" : "+r" (x));
        return x;
    #endif
#else
    return ia_static_cast(u32, ((x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24)));
#endif
}

IA_FORCE_INLINE u64
ia_bswap64(u64 x) 
{
#if IA_HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif defined(IA_ARCH_AMD64)
    __asm__("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(IA_ARCH_X86)
    #if IA_ARCH_X86_CHECK(4)
        union {
            struct {
                u32 a, b;
            } s;
            u64 u;
        } v;
        v.u = x;
        __asm__("bswapl %0 ; bswapl %1 ; xchgl %0, %1"
                : "=r"(v.s.a), "=r" (v.s.b)
                : "0"(v.s.a), "1" (v.s.b));
    #else // i386 has broken bswap
        u32 hi, lo;
        lo = (u32)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (u32)(x & 0xFFFFFFFF);
        x = ia_bswap32(lo);
        x <<= 32;
        x |= ia_bswap32(hi);
        return x;
    #endif
#else
    u32 hi, lo;
    lo = ia_static_cast(u32, x & 0xffffffff);
    x >>= 32;
    hi = ia_static_cast(u32, x & 0xffffffff);
    x = ia_bswap32(lo);
    x <<= 32;
    x |= ia_bswap32(hi);
    return x;
#endif
}

#if IA_HAS_ATTRIBUTE(force) || defined(__CHECKER__)
#define IA_ENDIAN_FORCE __attribute__((force))
#else
#define IA_ENDIAN_FORCE
#endif

#if IA_HAS_ATTRIBUTE(bitwise) || defined(__CHECKER__)
#define IA_ENDIAN_TYPE __attribute__((bitwise))
#else
#define IA_ENDIAN_TYPE
#endif

typedef u64 IA_ENDIAN_TYPE le64;
typedef u32 IA_ENDIAN_TYPE le32;
typedef u16 IA_ENDIAN_TYPE le16;

typedef u64 IA_ENDIAN_TYPE be64;
typedef u32 IA_ENDIAN_TYPE be32;
typedef u16 IA_ENDIAN_TYPE be16;

#if IA_BYTEORDER == _IA_BYTEORDER_LE
#define IA_LITTLE_ENDIAN 1
#define IA_CPU_TO_LE64(native) (ia_static_cast(IA_ENDIAN_FORCE le64, native))
#define IA_CPU_TO_LE32(native) (ia_static_cast(IA_ENDIAN_FORCE le32, native))
#define IA_CPU_TO_LE16(native) (ia_static_cast(IA_ENDIAN_FORCE le16, native))
#define IA_LE64_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u64, le_val)) 
#define IA_LE32_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u32, le_val)) 
#define IA_LE16_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u16, le_val)) 

#define IA_CPU_TO_BE64(native) (ia_static_cast(IA_ENDIAN_FORCE be64, ia_bswap64(native)))
#define IA_CPU_TO_BE32(native) (ia_static_cast(IA_ENDIAN_FORCE be32, ia_bswap32(native)))
#define IA_CPU_TO_BE16(native) (ia_static_cast(IA_ENDIAN_FORCE be16, ia_bswap16(native)))
#define IA_BE64_TO_CPU(be_val) ia_bswap64(ia_static_cast(IA_ENDIAN_FORCE u64, be_val)) 
#define IA_BE32_TO_CPU(be_val) ia_bswap32(ia_static_cast(IA_ENDIAN_FORCE u32, be_val)) 
#define IA_BE16_TO_CPU(be_val) ia_bswap16(ia_static_cast(IA_ENDIAN_FORCE u16, be_val)) 
#else /* big endian */
#define IA_BIG_ENDIAN 1
#define IA_CPU_TO_LE64(native) (ia_static_cast(IA_ENDIAN_FORCE le64, ia_bswap64(native)))
#define IA_CPU_TO_LE32(native) (ia_static_cast(IA_ENDIAN_FORCE le32, ia_bswap32(native)))
#define IA_CPU_TO_LE16(native) (ia_static_cast(IA_ENDIAN_FORCE le16, ia_bswap16(native)))
#define IA_LE64_TO_CPU(le_val) ia_bswap64(ia_static_cast(IA_ENDIAN_FORCE u64, le_val)) 
#define IA_LE32_TO_CPU(le_val) ia_bswap32(ia_static_cast(IA_ENDIAN_FORCE u32, le_val)) 
#define IA_LE16_TO_CPU(le_val) ia_bswap16(ia_static_cast(IA_ENDIAN_FORCE u16, le_val)) 

#define IA_CPU_TO_BE64(native) (ia_static_cast(IA_ENDIAN_FORCE be64, native))
#define IA_CPU_TO_BE32(native) (ia_static_cast(IA_ENDIAN_FORCE be32, native))
#define IA_CPU_TO_BE16(native) (ia_static_cast(IA_ENDIAN_FORCE be16, native))
#define IA_BE64_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u64, be_val)) 
#define IA_BE32_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u32, be_val)) 
#define IA_BE16_TO_CPU(le_val) (ia_static_cast(IA_ENDIAN_FORCE u16, be_val)) 
#endif

/** Convert an u64 to 64-bit little-endian. */
IA_FORCE_INLINE le64 
ia_cpu_to_le64(u64 native)
{ return IA_CPU_TO_LE64(native); }

/** Convert an u32 to 32-bit little-endian. */
IA_FORCE_INLINE le32
ia_cpu_to_le32(u32 native)
{ return IA_CPU_TO_LE32(native); }

/** Convert an u16 to 16-bit little-endian. */
IA_FORCE_INLINE le16
ia_cpu_to_le16(u16 native)
{ return IA_CPU_TO_LE16(native); }

/** Convert a 64-bit little-endian to u64. */
IA_FORCE_INLINE u64
ia_le64_to_cpu(le64 val)
{ return IA_LE64_TO_CPU(val); }

/** Convert a 32-bit little-endian to u32. */
IA_FORCE_INLINE u32 
ia_le32_to_cpu(le32 val)
{ return IA_LE32_TO_CPU(val); }

/** Convert a 16-bit little-endian to u16. */
IA_FORCE_INLINE u16 
ia_le16_to_cpu(le16 val)
{ return IA_LE16_TO_CPU(val); }

/** Convert an u64 to 64-bit big-endian. */
IA_FORCE_INLINE be64 
ia_cpu_to_be64(u64 native)
{ return IA_CPU_TO_BE64(native); }

/** Convert an u32 to 32-bit big-endian. */
IA_FORCE_INLINE be32 
ia_cpu_to_be32(u32 native)
{ return IA_CPU_TO_BE32(native); }

/** Convert an u16 to 16-bit big-endian. */
IA_FORCE_INLINE be16 
ia_cpu_to_be16(u16 native)
{ return IA_CPU_TO_BE16(native); }

/** Convert a 64-bit big-endian to u64. */
IA_FORCE_INLINE u64 
ia_be64_to_cpu(be64 val)
{ return IA_BE64_TO_CPU(val); }

/** Convert a 32-bit big-endian to u32. */
IA_FORCE_INLINE u32 
ia_be32_to_cpu(be32 val)
{ return IA_BE32_TO_CPU(val); }

/** Convert a 16-bit big-endian to u16. */
IA_FORCE_INLINE u16 
ia_be16_to_cpu(be16 val)
{ return IA_BE16_TO_CPU(val); }
