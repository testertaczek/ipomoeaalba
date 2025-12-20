#pragma once
/** @file ia/datastructures/mpmc.h
 *  @brief Multiple-producer multiple-consumer ring buffer.
 * 
 *  The MPMC ring buffer was implemented from the multiple-producer, multiple-consumer 
 *  queue as described by Dmitry Vyuko on 1024cores. 
 * 
 *  [High Speed Atomic MPMC Queue]
 *  http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 * 
 *  Read and write operations on an atomic object are free from data races.
 *  However, if one thread writes to it, all cache lines occupied by the object 
 *  are invalidated. If another thread is reading from an unrelated object that
 *  shares the same cache line, it incures unnecesary overhead. This is called
 *  false sharing, and we pad the MPMC structure to avoid that.
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// TODO

#ifdef __cplusplus
}
#endif /* __cplusplus */
