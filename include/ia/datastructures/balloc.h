#pragma once
/** @file ia/datastructures/balloc.h
 *  @brief Block allocator.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_balloc_page {
    u8                     *v;
    struct ia_balloc_page  *next;
} ia_balloc_page;

typedef struct ia_balloc_chunk {
    struct ia_balloc_chunk *next;
} ia_balloc_chunk;

typedef struct ia_balloc {
    i32                     data_size;
    i32                     chunk_size;
    i32                     chunks_per_block;
    i32                     block_size;
    ia_balloc_chunk        *head;
    ia_balloc_page         *block_head;
} ia_balloc;

#ifdef __cplusplus
}
#endif /* __cplusplus */
