#pragma once
/** @file ia/datastructures/stack.h
 *  @brief Stack allocator.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_stack_page {
    void                   *v;
    struct ia_stack_page   *next;
    i16                     sp;
    u32                     idx;
} ia_stack_page;

typedef struct ia_stack_cursor {
    struct ia_stack_cursor *prev;
    ia_stack_page          *stack;
    i16                     sp;
    bool                    is_free;
} ia_stack_cursor;

typedef struct ia_stack {
    ia_stack_page          *head;
    ia_stack_page          *tail_page;
    ia_stack_cursor        *tail_cursor;
} ia_stack;

#ifdef __cplusplus
}
#endif /* __cplusplus */
