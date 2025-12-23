#pragma once
/** @file ia/datastructures/bitset.h
 *  @brief Bitset data structure. 
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_bitset {
    u64    *v;
    i32     size;
    i32     alloc;
} ia_bitset;

#ifdef __cplusplus
}
#endif /* __cplusplus */
