#pragma once
/** @file ia/datastructures/arena.h
 *  @brief Arena allocator.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_arena_page {
    u8                     *v;
    struct ia_arena_page   *next;    
    i32                     offset;
    i32                     alloc;
} ia_arena_page;

typedef struct ia_arena_scratch {
    ia_arena_page          *tail;
    isize                   offset;
} ia_arena_scratch;

typedef struct ia_arena {
    ia_arena_page          *head;
    ia_arena_page          *tail;
} ia_arena;

#ifdef __cplusplus
}
#endif /* __cplusplus */
