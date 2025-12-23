#pragma once
/** @file ia/datastructures/deque.h
 *  @brief Type-preserving resizing circular double-ended queue.
 * 
 *  This implementation is using a resizing circular buffer. Deque operations can 
 *  proceed perpetually without reallocations. The initial capacity must be specified 
 *  and is a lower bound when shrinking. Buffer capacity is doubled at enqueue to a 
 *  full deque buffer. Shrink behaviour choices are: never shrink, shrink to minimum
 *  when the deque is empty, or shrink by half when the queue is at 20% of capacity.
 *
 *  The interface has a Ruby style of naming ;3.
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum : i32 {
    ia_deque_no_shrink = 0,
    ia_deque_shrink_if_empty,
    ia_deque_shrink_at_one_fifth,
};

/** Double-ended queue. `head` and `tail` are indices of the first and last items in deque.
 *  `len` is the distance between head and tail. `cap` is the total capacity of `v`. `min` 
 *  is the initial capacity of the deque. `shrink` is a flag to specify shrink behaviour.
 *  When shrinking, `min` is the smallest size. */
typedef struct ia_deque {
    void       *v;
    i32         head, tail, len, cap, min, shrink;
#ifdef IA_DEBUG
    char const *dbg_name;
#endif
} ia_deque;

#ifdef __cplusplus
}
#endif /* __cplusplus */
