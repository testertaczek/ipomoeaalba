#pragma once
/** @file ia/base/targets.h
 *  @brief Definitions for different platform targets and toolchains.
 *
 *  MSVC although mentioned in some places, isn't really supported in this engine.
 *  The compatibility code for MSVC is written as is, without the assumption that 
 *  it will work. There are some C23 features and GNU extensions in use that can't 
 *  be easily replicated with the limitations of MSVC, so support is dropped.
 */

/** Defines a numeric version. */
#define IA_VERSION_NUM(ma, mi, rev)     ((ma) * 10000000 + (mi) * 10000 + (rev))
/** Acquires the major, minor or revision part of a numeric version. */
#define IA_VERSION_NUM_MAJOR(ver)       ((ver) / 10000000)
#define IA_VERSION_NUM_MINOR(ver)       (((ver) / 10000) % 1000)
#define IA_VERSION_NUM_REVISION(ver)    ((ver) % 10000)

/** Including system's headers inside `extern "C"` { ... } is not safe, as system 
 *  headers may have C++ code in them, and C++ code inside extern "C" leads to 
 *  syntactically incorrect code. 
 *
 *  This is because putting code inside extern "C" won't make __cplusplus define 
 *  go away, that is, the system header being included thinks it is free to 
 *  use C++ as it sees fits. 
 *
 *  Including non-system headers inside extern "C" is not safe either, because 
 *  non-system headers end up including system headers, hence fall in the above
 *  case too. Conclusion, includes inside extern "C" are simply not portable.
 *
 *  This template helps surface these issues. */
#ifdef __cplusplus
template<class T> class _IncludeInsideExternCNotPortable;
#endif

#if !defined(IA_NDEGUG) && defined(NDEBUG)
    #define IA_NDEBUG 1
#elif !defined(IA_DEBUG) && defined(DEBUG)
    #define IA_DEBUG 1
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
#define IA_PLATFORM_UNIX 1
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#define IA_PLATFORM_LINUX 1
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#define IA_PLATFORM_ANDROID 1
#endif

#if defined(bsdi) || defined(__bsdi) || defined(__bsdi__)
#define IA_PLATFORM_BSDI 1
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__DragonFly__)
#define IA_PLATFORM_FREEBSD 1
#endif

#ifdef __NetBSD__
#define IA_PLATFORM_NETBSD 1
#endif

#ifdef __OpenBSD__
#define IA_PLATFORM_OPENBSD 1
#endif

#ifdef __HAIKU__
#define IA_PLATFORM_HAIKU 1
#endif

#if defined(__APPLE__)
#define IA_PLATFORM_APPLE 1
#include <AvailabilityMacros.h>

#ifndef __has_extension
    #define __has_extension(x) 0
    #include <TargetConditionals.h>
    #undef __has_extension
#else
    #include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_VISION
#define IA_PLATFORM_VISIONOS 1
#endif
#ifdef TARGET_OS_IPHONE
#define IA_PLATFORM_IOS 1
#else
#define IA_PLATFORM_MACOS 1
#endif
#endif /* APPLE */

#ifdef __CYGWIN__
#define IA_PLATFORM_CYGWIN 1
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(IA_PLATFORM_CYGWIN)
#define IA_PLATFORM_WINDOWS 1
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define IA_CC_GNUC_VERSION IA_VERSION_NUM(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define IA_CC_GNUC_VERSION IA_VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(IA_CC_GNUC_VERSION)
    #define IA_CC_GNUC_VERSION_CHECK(ma, mi, rev) (IA_CC_GNUC_VERSION >= IA_VERSION_NUM(ma, mi, rev))
#else
    #define IA_CC_GNUC_VERSION_CHECK(ma, mi, rev) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define IA_CC_MSVC_VERSION IA_VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define IA_CC_MSVC_VERSION IA_VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define IA_CC_MSVC_VERSION IA_VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(IA_CC_MSVC_VERSION)
    #define IA_CC_MSVC_VERSION_CHECK(ma, mi, rev) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define IA_CC_MSVC_VERSION_CHECK(ma, mi, rev) (_MSC_FULL_VER >= ((ma * 10000000) + (mi * 100000) + (rev)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define IA_CC_MSVC_VERSION_CHECK(ma, mi, rev) (_MSC_FULL_VER >= ((ma * 1000000) + (mi * 10000) + (rev)))
#else
    #define IA_CC_MSVC_VERSION_CHECK(ma, mi, rev) (_MSC_VER >= ((ma * 100) + (mi)))
#endif

/* MINGW */
#if defined(__MINGW64__)
    #define IA_CC_MINGW_VERSION IA_VERSION_NUM(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, __MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32__)
    #define IA_CC_MINGW_VERSION IA_VERSION_NUM(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#endif
#if defined(IA_CC_MINGW_VERSION)
    #define IA_CC_MINGW_VERSION_CHECK(ma, mi, rev) (IA_CC_MINGW_VERSION >= IA_VERSION_NUM(ma, mi, rev))
#else
    #define IA_CC_MINGW_VERSION_CHECK(ma, mi, rev) (0)
#endif

/* EMSCRIPTEN */
#if defined(EMSCRIPTEN) || defined(__EMSCRIPTEN__)
    #define IA_PLATFORM_EMSCRIPTEN 1
    #include <emscripten.h>
    #define IA_CC_EMSCRIPTEN_VERSION IA_VERSION_NUM(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif
#if defined(IA_CC_EMSCRIPTEN_VERSION)
    #define IA_CC_EMSCRIPTEN_VERSION_CHECK(ma, mi, rev) (IA_CC_EMSCRIPTEN_VERSION >= IA_VERSION_NUM(ma, mi, rev))
#else
    #define IA_CC_EMSCRIPTEN_VERSION_CHECK(ma, mi, rev) (0)
#endif

/* ARMC */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define IA_CC_ARMC_VERSION IA_VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define IA_CC_ARMC_VERSION IA_VERSION_NUM(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(IA_CC_ARMC_VERSION)
    #define IA_CC_ARMC_VERSION_CHECK(ma, mi, rev) (IA_CC_ARMC_VERSION >= IA_VERSION_NUM(ma, mi, rev))
#else
    #define IA_CC_ARMC_VERSION_CHECK(ma, mi, rev) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(IA_CC_CLANG_VERSION)
    #if __has_warning("-Wunique-object-duplication")
        #define IA_CC_CLANG_VERSION 210000
    #elif __has_warning("-Wvarargs")
        #define IA_CC_CLANG_VERSION 200000
    #elif __has_warning("-Wmissing-designated-field-initializers")
        #define IA_CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define IA_CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define IA_CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define IA_CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define IA_CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define IA_CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define IA_CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define IA_CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define IA_CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define IA_CC_CLANG_VERSION 100000
    #else
        #define IA_CC_CLANG_VERSION 1
    #endif
#endif
#if defined(IA_CC_CLANG_VERSION)
    #define IA_CC_CLANG_VERSION_CHECK(ma, mi, rev) (IA_CC_CLANG_VERSION >= ((ma * 10000) + (mi * 1000) + (rev)))
#else
    #define IA_CC_CLANG_VERSION_CHECK(ma, mi, rev) (0)
#endif

/* GCC */
#if (defined(IA_CC_GNUC_VERSION) || defined(IA_CC_MINGW_VERSION)) && \
    !defined(IA_CC_CLANG_VERSION) && \
    !defined(IA_CC_MSVC_VERSION) && \
    !defined(IA_CC_ARMC_VERSION)
    #define IA_CC_GCC_VERSION IA_CC_GNUC_VERSION
#endif
#if defined(IA_CC_GCC_VERSION)
    #define IA_CC_GCC_VERSION_CHECK(ma, mi, rev) (IA_CC_GCC_VERSION >= IA_VERSION_NUM(ma, mi, rev))
#else
    #define IA_CC_GCC_VERSION_CHECK(ma, mi, rev) (0)
#endif

#ifdef __has_attribute
    #define IA_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define IA_HAS_ATTRIBUTE(x) (0)
#endif
#ifdef __has_builtin
    #define IA_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define IA_HAS_BUILTIN(x) (0)
#endif
#ifdef __has_extension
    #define IA_HAS_EXTENSION(x) __has_extension
#else
    #define IA_HAS_EXTENSION(x) (0)
#endif
#ifdef __has_feature
    #define IA_HAS_FEATURE(x) __has_feature(x)
#else
    #define IA_HAS_FEATURE(x) (0)
#endif
#ifdef __has_include
    #define IA_HAS_INCLUDE(x) __has_include(x)
#else
    #define IA_HAS_INCLUDE(x) (0)
#endif
#ifdef __has_warning
    #define IA_HAS_WARNING(x) __has_warning(x)
#else
    #define IA_HAS_WARNING(x) (0)
#endif

#if defined(IA_CC_MSVC_VERSION)
    /** Visual studio will complain if we define the `inline` keyword, but actually it only supports the keyword in C++. */
    #if !defined(_ALLOW_KEYWORD_MACROS)
        #define _ALLOW_KEYWORD_MACROS
    #endif

    /** MSVC has a `__restrict` keyword, but it also has a `__declspec(restrict)` modifier, so it is impossible
     *  to define a `restrict` macro without interfering with the latter. Furthermore the MSVC standard library 
     *  uses __declspec(restrict) under the _CRTRESTRICT macro. For now resolve this issue by redefining _CRTRESTRICT. */
    #include <crtdefs.h>
    #undef _CRTRESTRICT
    #define _CRTRESTRICT
#endif /* MSVC */
#ifndef inline
    #if (__STDC_VERSION__ >= 199901L) || defined(__cplusplus)
        /* supports the inline keyword */
    #elif defined(IA_CC_GNUC_VERSION)
        #define inline __inline__
    #elif defined(IA_CC_MSVC_VERSION)
        #define inline __inline
    #endif
#endif /* inline */
#ifndef restrict
    /* http://cellperformance.beyond3d.com/articles/2006/05/demystifying-the-restrict-keyword.html */
    #if (__STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
        /* C99 */
    #elif defined(IA_CC_GNUC_VERSION)
        #define restrict __restrict__
    #elif defined(IA_CC_MSVC_VERSION)
        #define restrict __restrict
    #else
        #define restrict
    #endif
#endif /* restrict */

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

#if !defined(IA_FORCE_INLINE) && !defined(IA_FORCE_NOINLINE)
    #if defined(IA_CC_CLANG_VERSION) || defined(IA_CC_GNUC_VERSION)
        #define IA_FORCE_INLINE static __attribute__((always_inline)) inline
        #define IA_FORCE_NOINLINE __attribute__(noinline)
    #elif defined(IA_CC_MSVC_VERSION)
        #define IA_FORCE_INLINE __forceinline
        #define IA_FORCE_NOINLINE __declspec(noinline)
    #else
        #define IA_FORCE_INLINE static inline
        #define IA_FORCE_NOINLINE
    #endif
#endif

/** Likely and unlikely branches. */
#if defined(IA_CC_CLANG_VERSION) || defined(IA_CC_GNUC_VERSION)
    #define IA_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define IA_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define IA_LIKELY(x)   (x)
    #define IA_UNLIKELY(x) (x)
#endif

/** The function never returns. */
#if (__STDC_VERSION__ >= 202311L) || defined(__cplusplus)
    #define IA_NORETURN [[noreturn]]
#elif IA_HAS_ATTRIBUTE(noreturn)
    #define IA_NORETURN __attribute__((noreturn))
#elif defined(IA_CC_MSVC_VERSION)
    #define IA_NORETURN __declspec(noreturn)
#else
    #define IA_NORETURN
#endif

/** Function is a cold spot and will be optimized for size. */
#if IA_HAS_ATTRIBUTE(cold)
#define IA_COLD_FN __attribute__((cold))
#else
#define IA_COLD_FN
#endif

/** Function is a hot spot and will be optimized for speed. */
#if IA_HAS_ATTRIBUTE(hot)
#define IA_HOT_FN __attribute__((hot))
#else
#define IA_HOT_FN
#endif

/** Function has no side-effects. */
#if IA_HAS_ATTRIBUTE(pure)
#define IA_PURE_FN __attribute__((pure))
#else
#define IA_PURE_FN
#endif

/** Function has no side-effects, return value depends on arguments only.
 *  Must not take pointer parameters, must not return NULL. */
#if IA_HAS_ATTRIBUTE(const)
#define IA_CONST_FN __attribute__((const))
#else
#define IA_CONST_FN
#endif

/** Function never returns NULL. */
#if IA_HAS_ATTRIBUTE(returns_nonnull)
#define IA_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define IA_RETURNS_NONNULL
#endif

/** Function must be called with NULL as the last argument (for varargs functions). */
#if IA_HAS_ATTRIBUTE(sentinel)
#define IA_SENTINEL_FN __attribute__((sentinel))
#else
#define IA_SENTINEL_FN
#endif

/** Symbol is meant to be possibly unused. */
#if IA_HAS_ATTRIBUTE(unused)
#define IA_UNUSED __attribute__((unused))
#else
#define IA_UNUSED
#endif

/** Symbol should be emitted even if it appears to be unused. */
#if IA_HAS_ATTRIBUTE(used)
#define IA_USED __attribute__((used))
#else
#define IA_USED
#endif

/** Function parameters at specified positions must not be NULL. */
#if IA_HAS_ATTRIBUTE(nonnull)
#define IA_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define IA_NONNULL(...)
#endif

/** All pointer parameters must not be NULL. */
#if IA_HAS_ATTRIBUTE(nonnull)
#define IA_NONNULL_ALL __attribute__((nonnull))
#else
#define IA_NONNULL_ALL
#endif

/** The return value of this function must not be ignored. */
#if IA_HAS_ATTRIBUTE(warn_unused_result)
#define IA_NODISCARD __attribute__((warn_unused_result))
#else
#define IA_NODISCARD
#endif

/** Function takes a printf-style format string and variadic arguments. */
#if IA_HAS_ATTRIBUTE(format)
#define IA_PRINTF(fmt, va) __attribute__((format(__printf__, fmt, va)))
#else
#define IA_PRINTF(fmt, va)
#endif

/** The pointer returned by this function cannot alias any other pointer valid when the function returns. */
#if IA_HAS_ATTRIBUTE(malloc)
#define IA_MALLOC __attribute__((malloc))
#else
#define IA_MALLOC
#endif

/** Hints to the compiler that a statement that falls through to another case label is intentional. */
#if IA_HAS_ATTRIBUTE(fallthrough)
#define IA_FALLTHROUGH __attribute__((fallthrough))
#else
#define IA_FALLTHROUGH
#endif

#if IA_HAS_ATTRIBUTE(packed)
#define IA_PACKED __attribute__((packed))
#else
#define IA_PACKED
#endif

/* https://en.wikipedia.org/wiki/X86-64 */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #if !defined(_M_ARM64EC)
        #define IA_ARCH_AMD64 1000
    #endif
#endif

/* https://en.wikipedia.org/wiki/X86 */
#if defined(_M_IX86)
    #define IA_ARCH_X86 (_M_IX86 / 100)
#elif defined(__I86__)
    #define IA_ARCH_X86 __I86__
#elif defined(i686) || defined(__i686) || defined(__i686__)
    #define IA_ARCH_X86 6
#elif defined(i586) || defined(__i586) || defined(__i586__)
    #define IA_ARCH_X86 5
#elif defined(i486) || defined(__i486) || defined(__i486__)
    #define IA_ARCH_X86 4
#elif defined(i386) || defined(__i386) || defined(__i386__)
    #define IA_ARCH_X86 3
#elif defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__)
    #define IA_ARCH_X86 3
#endif
#if defined(IA_ARCH_X86)
    #define IA_ARCH_X86_CHECK(ver) ((ver) <= IA_ARCH_X86)
#else
    #define IA_ARCH_X86_CHECK(ver) (0)
#endif

/* SIMD ISA extensions for intel x86 and amd64 */
#if defined(_M_IX86_FP)
    #define IA_ARCH_X86_MMX 1
    #include <mmintrin.h>
    #if (_M_IX86_FP >= 1)
        #define IA_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if (_M_IX86_FP >= 2)
        #define IA_ARCH_X86_SSE 1
        #define IA_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#elif defined(_M_X64)
    #define IA_ARCH_X86_SSE 1
    #define IA_ARCH_X86_SSE2 1
    #include <emmintrin.h>
#else
    #if defined(__MMX__)
        #define IA_ARCH_X86_MMX 1
        #include <mmintrin.h>
    #endif
    #if defined(__SSE__)
        #define IA_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if defined(__SSE2__)
        #define IA_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#endif
#if defined(__SSE3__)
    #define IA_ARCH_X86_SSE3 1
    #include <pmmintrin.h>
#endif
#if defined(__SSSE3__)
    #define IA_ARCH_X86_SSSE3 1
    #include <tmmintrin.h>
#endif
#if defined(__SSE4_1__)
    #define IA_ARCH_X86_SSE4_1 1
    #include <smmintrin.h>
#endif
#if defined(__SSE4_2__)
    #define IA_ARCH_X86_SSE4_2 1
    #include <nmmintrin.h>
#endif
#if defined(__SSE4A__)
    #define IA_ARCH_X86_SSE4A 1
    #include <ammintrin.h>
#endif
#if defined(__XOP__)
    #define IA_ARCH_X86_XOP 1
    #include <xopintrin.h>
#endif
#if defined(__AVX__)
    #define IA_ARCH_X86_AVX 1
    #if !defined(IA_ARCH_X86_SSE3)
        #define IA_ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if !defined(IA_ARCH_X86_SSE4_1)
        #define IA_ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if !defined(IA_ARCH_X86_SSE4_2)
        #define IA_ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX2__)
    #define IA_ARCH_X86_AVX2 1
    #if defined(IA_CC_MSVC_VERSION)
        #ifndef __FMA__
            #define __FMA__ 1
        #endif
    #endif
    #include <immintrin.h>
#endif
#if defined(__FMA__)
    #define IA_ARCH_X86_FMA 1
    #if !defined(IA_ARCH_X86_AVX)
        #define IA_ARCH_X86_AVX 1
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX512VP2INTERSECT__)
    #define IA_ARCH_X86_AVX512VP2INTERSECT 1
#endif
#if defined(__AVX512BITALG__)
    #define IA_ARCH_X86_AVX512BITALG 1
#endif
#if defined(__AVX512VPOPCNTDQ__)
    #define IA_ARCH_X86_AVX512VPOPCNTDQ 1
#endif
#if defined(__AVX512VBMI__)
    #define IA_ARCH_X86_AVX512VBMI 1
#endif
#if defined(__AVX512VBMI2__)
    #define IA_ARCH_X86_AVX512VBMI2 1
#endif
#if defined(__AVX512VNNI__)
    #define IA_ARCH_X86_AVX512VNNI 1
#endif
#if defined(__AVX5124VNNIW__)
    #define IA_ARCH_X86_AVX5124VNNIW 1
#endif
#if defined(__AVX512BW__)
    #define IA_ARCH_X86_AVX512BW 1
#endif
#if defined(__AVX512BF16__)
    #define IA_ARCH_X86_AVX512BF16 1
#endif
#if defined(__AVX512CD__)
    #define IA_ARCH_X86_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
    #define IA_ARCH_X86_AVX512DQ 1
#endif
#if defined(__AVX512F__)
    #define IA_ARCH_X86_AVX512F 1
#endif
#if defined(__AVX512VL__)
    #define IA_ARCH_X86_AVX512VL 1
#endif
#if defined(__AVX512FP16__)
    #define IA_ARCH_X86_AVX512FP16 1
#endif
#if defined(__GFNI__)
    #define IA_ARCH_X86_GFNI 1
#endif
#if defined(__PCLMUL__)
    #define IA_ARCH_X86_PCLMUL 1
#endif
#if defined(__VPCLMULQDQ__)
    #define IA_ARCH_X86_VPCLMULQDQ 1
#endif
#if (defined(__F16C__) || IA_CC_MSVC_VERSION_CHECK(19,30,0)) && defined(IA_ARCH_X86_AVX2)
    #define IA_ARCH_X86_F16C 1
#endif
#if defined(__AES__)
    #define IA_ARCH_X86_AES 1
#endif

#if IA_CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef IA_CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            _m_prefetch(void *__P) {
            static void __attribute__((__always_inline__, __nodebug__)) inline
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif IA_CC_MINGW_VERSION_CHECK(4,0,0)
    #include <intrin.h>
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define IA_ARCH_AARCH64 1000
#endif
#if defined(IA_ARCH_AARCH64)
    #define IA_ARCH_AARCH64_CHECK(ver) ((ver) <= IA_ARCH_AARCH64)
#else
    #define IA_ARCH_AARCH64_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__ARM_ARCH)
    #if __ARM_ARCH > 100
        #define IA_ARCH_ARM (__ARM_ARCH)
    #else
        #define IA_ARCH_ARM (__ARM_ARCH * 100)
    #endif
#elif defined(_M_ARM)
    #if _M_ARM > 100
        #define IA_ARCH_ARM (_M_ARM)
    #else
        #define IA_ARCH_ARM (_M_ARM * 100)
    #endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
    #define IA_ARCH_ARM 800
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARM)
    #define IA_ARCH_ARM 1
#endif
#if defined(IA_ARCH_ARM)
    #define IA_ARCH_ARM_CHECK(ma, mi) (((ma * 100) + (mi)) <= IA_ARCH_ARM)
#else
    #define IA_ARCH_ARM_CHECK(ma, mi) (0)
#endif

/* ARM SIMD ISA extensions */
#if defined(__ARM_NEON) || defined(IA_ARCH_AARCH64)
    #if defined(IA_ARCH_AARCH64)
        #define IA_ARCH_ARM_NEON IA_ARCH_AARCH64
        #include <arm64_neon.h>
    #elif defined(IA_ARCH_ARM)
        #define IA_ARCH_ARM_NEON IA_ARCH_ARM
        #include <arm_neon.h>
    #endif
#endif
#if defined(__ARM_FEATURE_AES) && __ARM_FEATURE_AES
    #define IA_ARCH_ARM_AES 1
#endif
#if defined(__ARM_FEATURE_COMPLEX) && __ARM_FEATURE_COMPLEX
    #define IA_ARCH_ARM_COMPLEX 1
#endif
#if defined(__ARM_FEATURE_CRYPTO) && __ARM_FEATURE_CRYPTO
    #define IA_ARCH_ARM_CRYPTO 1
#endif
#if defined(__ARM_FEATURE_CRC32) && __ARM_FEATURE_CRC32
    #define IA_ARCH_ARM_CRC32 1
#endif
#if defined(__ARM_FEATURE_DOTPROD) && __ARM_FEATURE_DOTPROD
    #define IA_ARCH_ARM_DOTPROD 1
#endif
#if defined(__ARM_FEATURE_FMA) && __ARM_FEATURE_FMA
    #define IA_ARCH_ARM_FMA 1
#endif
#if defined(__ARM_FEATURE_FP16_FML) && __ARM_FEATURE_FP16_FML
    #define IA_ARCH_ARM_FP16_FML 1
#endif
#if defined(__ARM_FEATURE_FRINT) && __ARM_FEATURE_FRINT
    #define IA_ARCH_ARM_FRINT 1
#endif
#if defined(__ARM_FEATURE_MATMUL_INT8) && __ARM_FEATURE_MATMUL_INT8
    #define IA_ARCH_ARM_MATMUL_INT8 1
#endif
#if defined(__ARM_FEATURE_SHA2) && __ARM_FEATURE_SHA2 && !defined(__APPLE_CC__)
    #define IA_ARCH_ARM_SHA2 1
#endif
#if defined(__ARM_FEATURE_SHA3) && __ARM_FEATURE_SHA3
    #define IA_ARCH_ARM_SHA3 1
#endif
#if defined(__ARM_FEATURE_SHA512) && __ARM_FEATURE_SHA512
    #define IA_ARCH_ARM_SHA512 1
#endif
#if defined(__ARM_FEATURE_SM3) && __ARM_FEATURE_SM3
    #define IA_ARCH_ARM_SM3 1
#endif
#if defined(__ARM_FEATURE_SM4) && __ARM_FEATURE_SM4
    #define IA_ARCH_ARM_SM4 1
#endif
#if defined(__ARM_FEATURE_SVE) && __ARM_FEATURE_SVE
    #define IA_ARCH_ARM_SVE 1
#endif
#if defined(__ARM_FEATURE_QRDMX) && __ARM_FEATURE_QRDMX
    #define IA_ARCH_ARM_QRDMX 1
#endif
/* Availability of 16-bit floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
    #define IA_ARCH_ARM_NEON_FP16
#endif
/* Availability of 16-bit brain floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)
    #define IA_ARCH_ARM_NEON_BF16
#endif

/* https://en.wikipedia.org/wiki/RISC-V */
#if defined(__riscv) || defined(__riscv__)
    #define IA_ARCH_RISCV 1
    #if __riscv_xlen == 64
        #define IA_ARCH_RISCV64 1
    #elif __riscv_xlen == 32
        #define IA_ARCH_RISCV32 1
    #endif
#endif
/* RISC-V SIMD ISA extensions */
#if defined(__riscv_zve32x)
    #define IA_ARCH_RISCV_ZVE32X 1
#endif
#if defined(__riscv_zve32f)
    #define IA_ARCH_RISCV_ZVE32F 1
#endif
#if defined(__riscv_zve64x)
    #define IA_ARCH_RISCV_ZVE64X 1
#endif
#if defined(__riscv_zve64f)
    #define IA_ARCH_RISCV_ZVE64F 1
#endif
#if defined(__riscv_zve64d)
    #define IA_ARCH_RISCV_ZVE64D 1
#endif
#if defined(__riscv_v)
    #define IA_ARCH_RISCV_V 1
#endif
#if defined(__riscv_zvfh)
    #define IA_ARCH_RISCV_ZVFH 1
#endif
#if defined(__riscv_zvfhmin)
    #define IA_ARCH_RISCV_ZVFHMIN 1
#endif

/* https://en.wikipedia.org/wiki/IBM_POWER_Instruction_Set_Architecture */
#if defined(_M_PPC)
    #define IA_ARCH_POWER _M_PPC
#elif defined(_ARCH_PWR9)
    #define IA_ARCH_POWER 900
#elif defined(_ARCH_PWR8)
    #define IA_ARCH_POWER 800
#elif defined(_ARCH_PWR7)
    #define IA_ARCH_POWER 700
#elif defined(_ARCH_PWR6)
    #define IA_ARCH_POWER 600
#elif defined(_ARCH_PWR5)
    #define IA_ARCH_POWER 500
#elif defined(_ARCH_PWR4)
    #define IA_ARCH_POWER 400
#elif defined(_ARCH_440) || defined(__ppc440__)
    #define IA_ARCH_POWER 440
#elif defined(_ARCH_450) || defined(__ppc450__)
    #define IA_ARCH_POWER 450
#elif defined(_ARCH_601) || defined(__ppc601__)
    #define IA_ARCH_POWER 601
#elif defined(_ARCH_603) || defined(__ppc603__)
    #define IA_ARCH_POWER 603
#elif defined(_ARCH_604) || defined(__ppc604__)
    #define IA_ARCH_POWER 604
#elif defined(_ARCH_605) || defined(__ppc605__)
    #define IA_ARCH_POWER 605
#elif defined(_ARCH_620) || defined(__ppc620__)
    #define IA_ARCH_POWER 620
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC) || defined(__ppc)
    #define IA_ARCH_POWER 1
#endif
#if defined(IA_ARCH_POWER)
    #define IA_ARCH_POWER_CHECK(ver) ((ver) <= IA_ARCH_POWER)
#else
    #define IA_ARCH_POWER_CHECK(ver) (0)
#endif
#if defined(__ALTIVEC__)
    #define IA_ARCH_POWER_ALTIVEC            IA_ARCH_POWER
    #define IA_ARCH_POWER_ALTIVEC_CHECK(ver) ((ver) <= IA_ARCH_POWER)
#else
    #define IA_ARCH_POWER_ALTIVEC_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/MIPS_architecture */
#if defined(_MIPS_ISA_MIPS64R2)
    #define IA_ARCH_MIPS 642
#elif defined(_MIPS_ISA_MIPS64)
    #define IA_ARCH_MIPS 640
#elif defined(_MIPS_ISA_MIPS32R2)
    #define IA_ARCH_MIPS 322
#elif defined(_MIPS_ISA_MIPS32)
    #define IA_ARCH_MIPS 320
#elif defined(_MIPS_ISA_MIPS4)
    #define IA_ARCH_MIPS 4
#elif defined(_MIPS_ISA_MIPS3)
    #define IA_ARCH_MIPS 3
#elif defined(_MIPS_ISA_MIPS2)
    #define IA_ARCH_MIPS 2
#elif defined(_MIPS_ISA_MIPS1)
    #define IA_ARCH_MIPS 1
#elif defined(_MIPS_ISA_MIPS) || defined(__mips) || defined(__MIPS__)
    #define IA_ARCH_MIPS 1
#endif
#if defined(IA_ARCH_MIPS)
    #define IA_ARCH_MIPS_CHECK(ver) ((ver) <= IA_ARCH_MIPS)
#else
    #define IA_ARCH_MIPS_CHECK(ver) (0)
#endif
#if defined(__mips_loongson_mmi)
    #define IA_ARCH_MIPS_LOONGSON_MMI 1
#endif
#if defined(__mips_msa)
    #define IA_ARCH_MIPS_MSA 1
#endif

/* https://en.wikipedia.org/wiki/SPARC */
#if defined(__sparc_v9__) || defined(__sparcv9)
    #define IA_ARCH_SPARC 9
#elif defined(__sparc_v8__) || defined(__sparcv8)
    #define IA_ARCH_SPARC 8
#elif defined(__sparc_v7__) || defined(__sparcv7)
    #define IA_ARCH_SPARC 7
#elif defined(__sparc_v6__) || defined(__sparcv6)
    #define IA_ARCH_SPARC 6
#elif defined(__sparc_v5__) || defined(__sparcv5)
    #define IA_ARCH_SPARC 5
#elif defined(__sparc_v4__) || defined(__sparcv4)
    #define IA_ARCH_SPARC 4
#elif defined(__sparc_v3__) || defined(__sparcv3)
    #define IA_ARCH_SPARC 3
#elif defined(__sparc_v2__) || defined(__sparcv2)
    #define IA_ARCH_SPARC 2
#elif defined(__sparc_v1__) || defined(__sparcv1)
    #define IA_ARCH_SPARC 1
#elif defined(__sparc__) || defined(__sparc)
    #define IA_ARCH_SPARC 1
#endif
#if defined(IA_ARCH_SPARC)
    #define IA_ARCH_SPARC_CHECK(ver) ((ver) <= IA_ARCH_SPARC)
#else
    #define IA_ARCH_SPARC_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/Loongson#LoongArch */
#if defined(__loongarch32)
    #define IA_ARCH_LOONGARCH 1
#elif defined(__loongarch64)
    #define IA_ARCH_LOONGARCH 2
#endif
/* LSX: LoongArch 128-bits SIMD extension */
#if defined(__loongarch_sx)
    #define IA_ARCH_LOONGARCH_LSX 1
    #include <lsxintrin.h>
#endif
/* LASX: LoongArch 256-bits SIMD extension */
#if defined(__loongarch_asx)
    #define IA_ARCH_LOONGARCH_LASX 2
    #include <lasxintrin.h>
#endif

/* https://en.wikipedia.org/wiki/WebAssembly */
#if defined(__wasm__)
    #define IA_ARCH_WASM 1
#endif
#if defined(IA_ARCH_WASM) && defined(__wasm_simd128__)
    #define IA_ARCH_WASM_SIMD128
    #include <wasm_simd128.h>
#endif
#if defined(IA_ARCH_WASM) && defined(__wasm_relaxed_simd__)
    #define IA_ARCH_WASM_RELAXED_SIMD
#endif
