#pragma once
/** @file ia/datastructures/hashmap.h
 *  @brief Hashmap data structure.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>
#include <ia/datastructures/darray.h>
#include <ia/datastructures/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_hashmap_bucket {
    ia_darray           keys;       /**< darray<ia_map_key> */
    ia_darray           values;     /**< darray<ia_map_value> */
} ia_hashmap_bucket;

typedef struct ia_hashmap {
    ia_hash_value_op_fn hash;
    ia_compare_op_fn    compare;
    i32                 key_size;
    i32                 value_size;
    ia_map              impl;
} ia_hashmap;

typedef struct ia_hashmap_iter {
    ia_hashmap_bucket  *bucket;
    isize               index;
    ia_map_iter         impl;
} ia_hashmap_iter;

typedef struct ia_hashmap_result {
    void               *v;
    void               *key;
    u64                 hash;
} ia_hashmap_result;

#ifdef __cplusplus
}
#endif /* __cplusplus */
