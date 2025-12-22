#pragma once
/** @file ia/compute/bits.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Count trailing zeroes. */
IA_FORCE_INLINE IA_PURE_FN
i32 ia_ctz(u32 x)
{
#if IA_HAS_BUILTIN(__builtin_ctz)
    return __builtin_ctz(x);
#elif defined(IA_CC_MSVC_VERSION)
    u32 index;
    return _BitScanForward(&index, x) ? index : 32;
#else
    if (x == 0)
        return 32;
    u32 count = 0;
    while ((x & 1) == 0) {
        count++;
        x >>= 1;
    }
    return count;
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
