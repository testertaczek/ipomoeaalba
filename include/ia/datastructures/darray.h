#pragma once
/** @file ia/datastructures/darray.h
 *  @brief Dynamically-allocated array data structure.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>
#include <ia/base/log.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A dynamic array expected to change in size. */
typedef struct ia_darray {
    void   *v;
    i32     len, alloc;
    void   *allocator;
} ia_darray;

static constexpr ia_darray ia_darray_init = { .v = nullptr, .len = 0, .alloc = 0 };

#define ia_darray_as(T, da)     ia_reinterpret_cast(T *, (da)->v)
#define ia_darray_len(da)       ((da)->len)
#define ia_darray_alloc(da)     ((da)->alloc)
#define ia_darray_is_empty(da)  ((da)->len == 0)

#define __ia_darray_resize_dbg_w_allocator(da, stride, align, n, Tname, allocate, deallocate) \
    ({ \
        ia_san_assert((stride) >= 0 && (n) >= 0 && ia_is_pow2(align), "darray<"Tname">"); \
        i32 __len = n; \
        if (__len < ia_darray_len(da)) \
            __len = ia_darray_len(da); \
        if (IA_UNLIKELY(ia_darray_is_empty(da))) { \
            (da)->v = allocate((da)->allocator, (stride) * (n), (align)); \
            (da)->len = 0; \
            (da)->alloc = n; \
        } else if (IA_LIKELY(ia_darray_alloc(da) != __len)) { \
            (da)->alloc = (stride) * (n); \
            void *__v = memcpy(allocate((da)->allocator, (da)->alloc, (align)), (da)->v, (da)->len); \
            deallocate((da)->allocator, (da)->v, (da)->alloc * (stride)); \
            (da)->v = __v; \
        } \
    })

#define ia_darray_resize_as_bytes(da, size, align, ...) \
    __ia_darray_resize_dbg_w_allocator(da, ia_ssizeof(u8), align, size, "bytes", __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
