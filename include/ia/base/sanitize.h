#pragma once
/** @file ia/base/sanitize.h
 *  @brief Compiler- and build-configuration-dependent instrumentation helpers.
 *
 *  Provides macros and attributes used to:
 *  - Detect and enable sanitizer analysis (ASAN, TSAN, MSAN, UBSAN)
 *  - Annotate code for static and dynamic analysis
 *  - Express assumptions, unreachable code branches, and debug-only expressions
 *  - Integrate Clang thread safety analysis where available
 *
 *  All constructs in this header are compile-time only. They must not be relied upon
 *  for correctness in release builds.
 */
#include <ia/base/targets.h>

#ifndef IA_HAS_VALGRIND
    #define IA_HAS_VALGRIND (0)
    #define IA_IF_VALGRIND(A,B) B
#else
    #define IA_IF_VALGRIND(A,B) A
#endif

#define IA_HAS_ASAN    IA_HAS_FEATURE(address_sanitizer)
#define IA_HAS_TSAN    IA_HAS_FEATURE(thread_sanitizer)
#define IA_HAS_MSAN    IA_HAS_FEATURE(memory_sanitizer)
#define IA_HAS_UBSAN   IA_HAS_FEATURE(undefined_behaviour_sanitizer)

#if !defined(IA_SANITIZE) && (IA_HAS_ASAN || IA_HAS_TSAN || IA_HAS_MSAN || IA_HAS_UBSAN || IA_HAS_VALGRIND)
    #define IA_SANITIZE 1
#endif /* IA_SANITIZE */

#if defined(IA_CC_CLANG_VERSION) || defined(IA_CC_GNUC_VERSION)
    #define IA_NO_SANITIZE_ADDRESS      __attribute__((no_sanitize("address")))
    #define IA_NO_SANITIZE_THREAD       __attribute__((no_sanitize("thread")))
    #define IA_NO_SANITIZE_UNDEFINED    __attribute__((no_sanitize("undefined")))
#else
    #define IA_NO_SANITIZE_ADDRESS
    #define IA_NO_SANITIZE_THREAD
    #define IA_NO_SANITIZE_UNDEFINED
#endif

/* unreachable code */
#if defined(IA_CC_GNUC_VERSION) || defined(IA_CC_CLANG_VERSION)
    #define IA_UNREACHABLE __builtin_unreachable()
#elifdef IA_CC_MSVC_VERSION
    #define IA_UNREACHABLE __assume(false)
#else
    IA_NORETURN inline void __ia_unreachable_impl() {}
    #define IA_UNREACHABLE (__ia_unreachable_impl())
#endif
/* assumptions */
#if IA_HAS_BUILTIN(__builtin_assume)
    #define IA_ASSUME(x) __builtin_assume(x)
#elifdef IA_CC_MSVC_VERSION
    #define IA_ASSUME(x) __assume(x)
#else
    #define IA_ASSUME(x) do { if(!(x)) { IA_UNREACHABLE; } } while(0)
#endif

/* To enable analysis, set these environment variables before running meson:
 *     export CC=clang
 *     export CFLAGS="-DIA_THREAD_SAFETY_ANALYSIS -Wthread-safety" */
#if defined(IA_THREAD_SAFETY_ANALYSIS) && defined(IA_CC_CLANG_VERSION) && (!defined(SWIG))
    #define _IA_THREAD_SAFETY_ATTRIBUTE(x) __attribute__((x))
#else
    #define _IA_THREAD_SAFETY_ATTRIBUTE(x)
#endif

#define IA_THREAD_SAFETY_CAPABILITY(X)                  _IA_THREAD_SAFETY_ATTRIBUTE(capability(x))
#define IA_THREAD_SAFETY_SCOPED_CAPABILITY              _IA_THREAD_SAFETY_ATTRIBUTE(scoped_lockable)
#define IA_THREAD_SAFETY_GUARDED_BY(X)                  _IA_THREAD_SAFETY_ATTRIBUTE(guarded_by(x))
#define IA_THREAD_SAFETY_PT_GUARDED_BY(X)               _IA_THREAD_SAFETY_ATTRIBUTE(pt_guarded_by(x))
#define IA_THREAD_SAFETY_ACQUIRED_BEFORE(X)             _IA_THREAD_SAFETY_ATTRIBUTE(acquired_before(x))
#define IA_THREAD_SAFETY_ACQUIRED_AFTER(X)              _IA_THREAD_SAFETY_ATTRIBUTE(acquired_after(x))
#define IA_THREAD_SAFETY_REQUIRES(X)                    _IA_THREAD_SAFETY_ATTRIBUTE(requires_capability(x))
#define IA_THREAD_SAFETY_REQUIRES_SHARED(X)             _IA_THREAD_SAFETY_ATTRIBUTE(requires_shared_capability(x))
#define IA_THREAD_SAFETY_ACQUIRE(X)                     _IA_THREAD_SAFETY_ATTRIBUTE(acquire_capability(x))
#define IA_THREAD_SAFETY_ACQUIRE_SHARED(X)              _IA_THREAD_SAFETY_ATTRIBUTE(acquire_shared_capability(x))
#define IA_THREAD_SAFETY_RELEASE(X)                     _IA_THREAD_SAFETY_ATTRIBUTE(release_capability(x))
#define IA_THREAD_SAFETY_RELEASE_SHARED(X)              _IA_THREAD_SAFETY_ATTRIBUTE(release_shared_capability(x))
#define IA_THREAD_SAFETY_RELEASE_GENERIC(X)             _IA_THREAD_SAFETY_ATTRIBUTE(release_generic_capability(x))
#define IA_THREAD_SAFETY_TRY_ACQUIRE(X)                 _IA_THREAD_SAFETY_ATTRIBUTE(try_acquire_capability(x))
#define IA_THREAD_SAFETY_TRY_ACQUIRE_SHARED(X)          _IA_THREAD_SAFETY_ATTRIBUTE(try_acquire_shared_capability(x))
#define IA_THREAD_SAFETY_EXCLUDES(X)                    _IA_THREAD_SAFETY_ATTRIBUTE(locks_excluded(x))
#define IA_THREAD_SAFETY_ASSERT_CAPABILITY(X)           _IA_THREAD_SAFETY_ATTRIBUTE(assert_capability(x))
#define IA_THREAD_SAFETY_ASSERT_SHARED_CAPABILITY(X)    _IA_THREAD_SAFETY_ATTRIBUTE(assert_shared_capability(x))
#define IA_THREAD_SAFETY_RETURN_CAPABILITY(X)           _IA_THREAD_SAFETY_ATTRIBUTE(lock_returned(x))
#define IA_NO_THREAD_SAFETY_ANALYSIS                    _IA_THREAD_SAFETY_ATTRIBUTE(no_thread_safety_analysis)

#if IA_HAS_BUILTIN(__builtin_debugtrap)
    #define ia_debugtrap() __builtin_debugtrap()
#elif IA_HAS_BUILTIN(__debugbreak)
    #define ia_debugtrap() __debugbreak()
#endif
#if !defined(ia_debugtrap)
    #if defined(IA_CC_MSVC_VERSION) || defined(IA_CC_INTEL_VERSION)
        #define ia_debugtrap() __debugbreak()
    #elif defined(IA_CC_ARM_VERSION)
        #define ia_debugtrap() __breakpoint(42)
    #elif defined(IA_CC_IBM_VERSION)
        #include <builtins.h>
        #define ia_debugtrap() __trap(42)
    #elif IA_ARCH_X86_CHECK(7) /* _M_IX86 */
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm int 3h; }
    #elif defined(IA_ARCH_X86) || defined(IA_ARCH_AMD64)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__("int $03"); }
    #elif defined(__thumb__) /* arm32 */
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__(".inst 0xde01"); }
    #elif defined(IA_ARCH_AARCH64)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__(".inst 0xd4200000"); }
    #elif defined(IA_ARCH_ARM)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__(".inst 0xe7f001f0"); }
    #elif defined(IA_ARCH_ALPHA) && !defined(__osf__)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__("bpt"); }
    #elif defined(_54_)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__("ESTOP"); }
    #elif defined(_55_)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
    #elif defined(_64P_)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__("SWBP 0"); }
    #elif defined(_6x_)
        IA_FORCE_INLINE void ia_debugtrap(void) { __asm__ __volatile__("NOP\n .word 0x10000000"); }
    #elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(IA_CC_GNUC_VERSION)
        #define ia_debugtrap() __builtin_trap()
    #else
        #include <signal.h>
        #if defined(SIGTRAP)
            #define ia_debugtrap() raise(SIGTRAP)
        #else
            #define ia_debugtrap() raise(SIGABRT)
        #endif
    #endif
#endif

#if IA_HAS_MSAN
    #include <sanitizer/msan_interface.h>
    #define ia_san_undefined(p,s)           __msan_allocated_memory(p,s)
    #define ia_san_defined(p,s)             __msan_unpoison(p,s)
    #define ia_san_check_defined(p,s)       __msan_check_mem_is_initialized(p,s)
    #define ia_san_noaccess(p,s)            ((void)0)
    #define ia_san_addressable(p,s)         MEM_UNDEFINED(p,s)
    #define ia_san_check_addressable(p,s)   ((void)0)
    #define ia_san_get_vbits(a,b,s)         __msan_copy_shadow(b,a,s)
    #define ia_san_set_vbits(a,b,s)         __msan_copy_shadow(a,b,s)
    #define IA_SAN_REDZONE_SIZE             (8)
#elif IA_HAS_VALGRIND
    #include <valgrind/memcheck.h>
    #define ia_san_undefined(p,s)           VALGRIND_MAKE_MEM_UNDEFINED(p,s)
    #define ia_san_defined(p,s)             VALGRIND_MAKE_MEM_DEFINED(p,s)
    #define ia_san_check_defined(p,s)       VALGRIND_CHECK_MEM_IS_DEFINED(p,s)
    #define ia_san_noaccess(p,s)            VALGRIND_MAKE_MEM_NOACCESS(p,s)
    #define ia_san_addressable(p,s)         MEM_UNDEFINED(p,s)
    #define ia_san_check_addressable(p,s)   VALGRIND_CHECK_MEM_IS_ADDRESSABLE(p,s)
    #define ia_san_get_vbits(a,b,s)         VALGRIND_GET_VBITS(a,b,s)
    #define ia_san_set_vbits(a,b,s)         VALGRIND_SET_VBITS(a,b,s)
    #define IA_SAN_REDZONE_SIZE             (8)
#elif IA_HAS_ASAN
    #include <sanitizer/asan_interface.h>
    /* How to do manual poisoning:
     * https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning */
    #define ia_san_undefined(p,s)           ((void)0)
    #define ia_san_defined(p,s)             ASAN_UNPOISON_MEMORY_REGION(p,s)
    #define ia_san_check_defined(p,s)       ((void)0)
    #define ia_san_noaccess(p,s)            ASAN_POISON_MEMORY_REGION(p,s)
    #define ia_san_addressable(p,s)         MEM_UNDEFINED(p,s)
    #define ia_san_check_addressable(p,s)   __asan_region_is_poisoned((void *)p,s)
    #define ia_san_get_vbits(a,b,s)         ((void)0)
    #define ia_san_set_vbits(a,b,s)         ((void)0)
    #define IA_SAN_REDZONE_SIZE             (8)
#else
    #define ia_san_undefined(p,s)           ((void)0)
    #define ia_san_defined(p,s)             ((void)0)
    #define ia_san_check_defined(p,s)       ((void)0)
    #define ia_san_noaccess(p,s)            ((void)0)
    #define ia_san_addressable(p,s)         ((void)0)
    #define ia_san_check_addressable(p,s)   ((void)0)
    #define ia_san_get_vbits(a,b,s)         ((void)0)
    #define ia_san_set_vbits(a,b,s)         ((void)0)
    #define IA_SAN_REDZONE_SIZE             (0)
#endif
