#pragma once
/** @file ia/datastructures/sparse.h
 *  @brief Sparse set data structure.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>
#include <ia/datastructures/darray.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Dense array with indices to sparse array. The dense array stores both alive and dead
 *  sparse indices. The `count` member keeps track of which sparse indices are alive. */
typedef struct ia_sparse {
    ia_darray   dense;      /**< darray<u64> */
    ia_darray   pages;      /**< darray<internal> - chunks with sparse arrays and data. */
    i32         stride;     /**< Size of an element in the sparse array. */
    i32         count;      /**< Number of alive entries in the dense array. */
    u64         max_idx;    /**< Local max index, if no global was set. */
} ia_sparse;

#ifdef __cplusplus
}
#endif /* __cplusplus */
