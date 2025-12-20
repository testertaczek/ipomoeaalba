#pragma once
/** @file ia/base/targets.h
 *  @brief TODO docs
 */

#define IA_VERSION_NUM_MAJOR(ver)       ((ver) / 10000000)
#define IA_VERSION_NUM_MINOR(ver)       (((ver) / 10000) % 1000)
#define IA_VERSION_NUM_REVISION(ver)    ((ver) % 10000)
#define IA_VERSION_NUM(ma, mi, rev)     ((ma) * 10000000 + (mi) * 10000 + (rev))

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
    #ifdef __cplusplus
        /* supports the inline keyword */
    #elif defined(IA_CC_GNUC_VERSION)
        #define inline __inline__
    #elif defined(IA_CC_MSVC_VERSION)
        #define inline __inline
    #elif defined(IA_CC_INTEL_VERSION)
        /* the intel compiler should support inline keyword */
    #elif (__STDC_VERSION__ >= 199901L)
        /* C99 supports inline keyword */
    #else
        #define inline
    #endif
#endif /* inline */
/* http://cellperformance.beyond3d.com/articles/2006/05/demystifying-the-restrict-keyword.html */
#ifndef restrict
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

/** The compiler declares a type for you. */
#ifndef typeof
    #if (__STDC_VERSION__ >= 202311L)
        /* C23 */
    #elif defined(IA_CC_MSVC_VERSION)
        #define typeof(x) decltype((x))
    #else
        #define typeof(x) __typeof__((x)) 
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
