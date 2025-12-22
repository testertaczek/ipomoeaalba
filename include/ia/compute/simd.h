#pragma once
/** @file ia/compute/simd.h
 *  @brief Layer to unify common SIMD operations between different target architectures.
 *
 *  SIMD stands for "single instruction, multiple data". I exploit architecture-specific SIMD instructions
 *  to augment data transformations and math. This is not an effort to completely enable portable vectorized
 *  math, instead it's an utility for writing such math.
 *
 *  Supported targets include:
 *  - x86/amd64 (SSE, SSE2, SSE3, SSSE3, SSE4_1, SSE4_2, AVX, AVX2, FMA)
 *  - arm/aarch64 (NEON, TODO CRYPTO?)
 *  - rv64gcv (RISC-V Vector extension)
 *  - WebAssembly (128-bit SIMD)
 *
 *  If any of these targets have SIMD intrinsics present, `IA_SIMD` is set to 1. Each backend defines atleast
 *  128-bit (4x32-bit) float operations. Also, 28-bit integer and 256-bit types may be defined if the given
 *  implementation allows for that. Data is required to be aligned (16-byte for 128-bit, 32-byte for 256-bit)
 *  unless `IA_SIMD_UNALIGNED` is defined - then the SIMD macros don't expect the data to be aligned properly.
 * 
 *  TODO docs operations
 */
#include <ia/base/targets.h>

/* Do not use SIMD alignment for older visual studio versions. */
#if defined(IA_CC_MSVC_VERSION_CHECK) && !IA_CC_MSVC_VERSION_CHECK(19,13,0)
    /* Visual Studio 2017 version 15.6 */
    #ifndef IA_SIMD_UNALIGNED
        #define IA_SIMD_UNALIGNED
    #endif
#endif

#define _IA_SIMD_H_
#if defined(IA_DISABLE_SIMD)
    /* ignore */
#elif defined(IA_ARCH_X86) || defined(IA_ARCH_AMD64)
    #include <ia/compute/simd/x86.h>
#elif defined(IA_ARCH_ARM) || defined(IA_ARCH_AARCH64)
    #include <ia/compute/simd/neon.h>
#elif defined(IA_ARCH_RISCV)
    #include <ia/compute/simd/rvv.h>
#elif defined(IA_ARCH_WASM)
    #include <ia/compute/simd/wasm.h>
#endif
#undef _IA_SIMD_H_

#if defined(IA_SIMD_X86) || defined(IA_SIMD_NEON) || defined(IA_SIMD_RVV) || defined(IA_SIMD_WASM)
#define IA_SIMD 1
#else
#define IA_SIMD 0
#endif
