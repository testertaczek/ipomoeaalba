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
#include <ia/base/atomic.h>
#include <ia/base/log.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The MPMC ring buffer is limited to a size that is a power of two.
 *  This data structure is not designed for dynamic growth. */
typedef struct IA_CACHELINE_ALIGNMENT ia_mpmc {
    void           *data;
    atomic_isize   *sequence;
    isize           mask;
    u8         _pad0[IA_CACHELINE_SIZE - sizeof(void *) - sizeof(isize)];

    atomic_isize    enqueue_pos;
    u8         _pad1[IA_CACHELINE_SIZE - sizeof(atomic_isize)];

    atomic_isize    dequeue_pos;
    u8         _pad2[IA_CACHELINE_SIZE - sizeof(atomic_isize)];
} ia_mpmc;

/** Initializes the MPMC data structure. Buffer memory must be externally managed.
 *  Cell count must be a power of 2, stride must be sizeof(T). */
IA_FORCE_INLINE void
ia_mpmc_init_(
    ia_mpmc        *mpmc, 
    isize           stride,
    isize           cell_count,
    void           *data_buffer,
    atomic_isize   *sequence_buffer,
    char const     *type_name)
{
    ia_dbg_assert(ia_is_pow2(cell_count), -1, type_name);
    mpmc->mask = cell_count -1;
    mpmc->data = data_buffer;
    mpmc->sequence = sequence_buffer;
    for (isize i; i < cell_count; i++) 
        ia_atomic_write_monotonic(&mpmc->sequence[i], i);
    ia_atomic_write_monotonic(&mpmc->enqueue_pos, 0); \
    ia_atomic_write_monotonic(&mpmc->dequeue_pos, 0); \
}
/** Typed macro helper for `ia_mpmc_init_`. */
#define ia_mpmc_init(mpmc, T, cell_count, data_buffer, sequence_buffer) \
    ia_mpmc_init_(mpmc, ia_ssizeof(T), cell_count, data_buffer, sequence_buffer, "mpmc<"#T">")

/** Either enqueue into or dequeue from the MPMC ring buffer. */
IA_NONNULL_ALL IA_HOT_FN IA_API bool IA_CALL
ia_mpmc_rotate(
    ia_mpmc        *mpmc,
    atomic_isize   *in_or_out,
    isize const     pos_delta,
    isize          *out_pos);

/** The producer. Data within cells is persistent, so submissions can be made from the stack. */
IA_NONNULL_ALL IA_FORCE_INLINE bool
ia_mpmc_enqueue_(
    ia_mpmc        *mpmc,
    isize           stride,
    void           *submit)
{
    isize pos;
    bool success = ia_mpmc_rotate(mpmc, &mpmc->enqueue_pos, 0, &pos);
    if (success) {
        isize at = pos & mpmc->mask;
        memcpy(ia_elem_(mpmc->data, stride, at), submit, stride);
        ia_atomic_write(&mpmc->sequence[at], pos + 1, ia_atomic_model_release);
    }
    return success;
}
/** Macro helper for typed MPMC enqueue. */
#define ia_mpmc_enqueue(mpmc, T, submit) \
    ia_mpmc_enqueue_(mpmc, ia_ssizeof(T), ia_reinterpret_cast(void *, submit))

/** The consumer. Copies the data within the buffer into a target data of the same type. */
IA_NONNULL_ALL IA_FORCE_INLINE bool
ia_mpmc_dequeue_(
    ia_mpmc        *mpmc,
    isize           stride,
    void           *target)
{
    isize pos;
    bool success = ia_mpmc_rotate(mpmc, &mpmc->dequeue_pos, mpmc->mask, &pos);
    if (success) {
        isize at = pos & mpmc->mask;
        memcpy(target, ia_elem_(mpmc->data, stride, at), stride);
        ia_atomic_write(&mpmc->sequence[at], pos + mpmc->mask + 1, ia_atomic_model_release);
    }
    return success;
}
/** Macro helper for typed MPMC dequeue. */
#define ia_mpmc_dequeue(mpmc, T, target) \
    ia_mpmc_dequeue_(mpmc, ia_ssizeof(T), ia_reinterpret_cast(void *, target))

#ifdef __cplusplus
}
#endif /* __cplusplus */
