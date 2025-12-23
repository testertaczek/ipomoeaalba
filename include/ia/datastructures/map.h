#pragma once
/** @file ia/datastructures/map.h
 *  @brief Map data structure.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef u64         ia_map_data;
typedef ia_map_data ia_map_key;
typedef ia_map_data ia_map_value;

typedef struct ia_bucket_entry {
    ia_map_key              key;
    ia_map_value            value;
    struct ia_bucket_entry *next;
} ia_bucket_entry;

typedef struct ia_bucket {
    ia_bucket_entry        *first;
} ia_bucket;

typedef struct ia_map {
    ia_bucket              *buckets;
    u32                     bucket_count;
    u32                     count : 26;
    u32                     bucket_shift : 6;
} ia_map;

typedef struct ia_map_iter {
    ia_map const           *map;
    ia_bucket              *bucket;
    ia_bucket_entry        *entry;
    ia_map_data            *res;
} ia_map_iter;

#ifdef __cplusplus
}
#endif /* __cplusplus */
