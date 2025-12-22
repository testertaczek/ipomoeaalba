#pragma once
/** @file ia/base/system.h
 *  @brief TODO docs
 */
#include <ia/base/types.h>
#include <ia/datastructures/strbuf.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Returns the counter of the real-time clock. */
IA_HOT_FN IA_API u64 IA_CALL
ia_rtc_counter(void);

/** Returns the frequency of the real-time clock. */
IA_API u64 IA_CALL
ia_rtc_frequency(void);

/** Open a shared library module. */
IA_NONNULL_ALL IA_API void *IA_CALL
ia_module_open(char const *libname);

/** Close a shared library module. */
IA_NONNULL_ALL IA_API void IA_CALL
ia_module_close(void *lib);

/** Get a procedure address from a shared library. */
IA_NONNULL_ALL IA_HOT_FN IA_API void *IA_CALL
ia_get_proc_address(void *lib, char const *procname);

/** Defines a system thread identifier. */
typedef struct { u64 handle; } ia_thread_id;

/** Returns the current thread ID. */
IA_HOT_FN IA_API ia_thread_id IA_CALL
ia_thread_id_current(void);

/** Creates and runs a system thread, save's it's identifier to the given thread handle. */
IA_NONNULL_ALL IA_API void IA_CALL
ia_thread_create(
    ia_thread_id   *out_thread, 
    usize           stacksize,
    void           *stackaddr,
    void           *(*proc)(void *), 
    void           *argument);

/** Joins a thread, it will wait for the thread to finish it's work before continuing. */
IA_API void IA_CALL
ia_thread_join(ia_thread_id thread);

/** Tries to assign CPU affinity for an array of threads. 
 *  Threads are mapped to the first cpu_count logical CPUs.
 *  May fail silently if unsupported by the host platform. */
IA_API void IA_CALL
ia_thread_affinity(
    i32                 cpu_count,
    i32                 thread_count, 
    ia_thread_id const *threads);

/** Dump the stack trace into a string buffer.
 *  @return Number of bytes written to the string buffer. */
IA_NONNULL_ALL IA_API i32 IA_CALL
ia_dump_stack_trace(ia_strbuf *buf);

/** Different sizes of hugetlb entries, varying between CPU's and kernel configurations. */
typedef u32 ia_hugepage_sizes;
typedef enum ia_hugepage_size_bits : ia_hugepage_sizes {
    ia_hugepage_size_none   = 0,
    ia_hugepage_size_4k     = (4u << 10),
    ia_hugepage_size_8k     = (8u << 10),
    ia_hugepage_size_16k    = (16u << 10),
    ia_hugepage_size_64k    = (64u << 10),
    ia_hugepage_size_256k   = (256u << 10),
    ia_hugepage_size_1m     = (1u << 20),
    ia_hugepage_size_2m     = (2u << 20),
    ia_hugepage_size_4m     = (4u << 20),
    ia_hugepage_size_16m    = (16u << 20),
    ia_hugepage_size_256m   = (256u << 20),
    ia_hugepage_size_1g     = (1u << 30),
} ia_hugepage_size_bits;

/** translates bytes to a specific hugepage_sizes bit. */
IA_FORCE_INLINE ia_hugepage_size_bits
ia_hugepage_size_from_bytes(usize bytes)
{
    if (!ia_is_pow2(bytes) || bytes < 4096)
        return ia_hugepage_size_none;
    return (ia_hugepage_size_bits)bytes;
}

/** Queries available sizes for hugepage entries in memory, including default page size.
 *  Also output's the total ram in the host system. */
IA_API ia_hugepage_sizes IA_CALL
ia_hugetlbinfo(usize *out_total_ram);

/** Queries system info about the CPU. TODO read CPU flags */
IA_API void IA_CALL
ia_cpuinfo(
    i32 *out_thread_count, 
    i32 *out_core_count,
    i32 *out_package_count);

/** TODO docs */
IA_API void *IA_CALL
ia_mmap(void);

/** TODO docs */
IA_API void IA_CALL
ia_munmap(
    void *mapped, 
    usize page_aligned);

#ifdef __cplusplus
}
#endif /* __cplusplus */
