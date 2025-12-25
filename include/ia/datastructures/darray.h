#pragma once
/** @file ia/datastructures/darray.h
 *  @brief Dynamically-allocated array data structure.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A dynamic array expected to change in size. */
typedef struct ia_darray {
    void   *v;
    i32     len, alloc;
#ifdef IA_DEBUG
    char const *dbg_name;
#endif
} ia_darray;

static constexpr ia_darray ia_darray_init = { .v = nullptr, .len = 0, .alloc = 0 };

#define ia_darray_len(da)   ((da)->len)
#define ia_darray_alloc(da) ((da)->alloc)
#define ia_darray_empty(da) ((da)->len == 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
