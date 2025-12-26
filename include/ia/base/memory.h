#pragma once
/** @file ia/base/memory.h
 *  @brief TODO docs
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

IA_HOT_FN IA_API void *IA_CALL 
ia_drift_alloc(isize size, isize align);

IA_HOT_FN IA_API void IA_CALL ia_drift_shift(void);
IA_HOT_FN IA_API void IA_CALL ia_drift_unshift(void);

/** Typed drift allocation. */
#define ia_drift_alloc_as(T, n) \
    ia_reinterpret_cast(T *, ia_drift_alloc(ia_ssizeof(T) * (n), ia_salignof(T)))

/** Discards the allocator struct for macros that may want one. */
#define ia_drift_alloc_nil(nil, size, align) \
    ia_drift_alloc(size, align)

/** Used in macro expansions for allocate/deallocate pairs. */
#define ia_drift_allocator      ia_drift_alloc_nil, (void)
#define ia_drift_allocator_init nullptr

#ifdef __cplusplus
}
#endif /* __cplusplus */
