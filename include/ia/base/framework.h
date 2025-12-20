#pragma once
/** @file ia/base/framework.h
 *  @brief TODO docs
 */
#include <ia/base/types.h>
#include <ia/base/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** TODO docs */
typedef void (IA_CALL *PFN_ia_work)(void *userdata);
/** TODO docs */
#define PFN_IA_WORK(pfn, arg) typedef void (IA_CALL *pfn)(arg)
/** TODO docs */
#define FN_IA_WORK(fn, arg) void IA_CALL fn(arg)

/** TODO docs */
typedef struct ia_work_details {
    PFN_ia_work     procedure;
    void           *argument;
    usize           stack_size;
    char const     *name;
} ia_work_details;

/** TODO docs */
typedef atomic_isize *ia_work_chain;

/** TODO docs */
IA_API ia_work_chain IA_CALL
ia_submit_work(
    u32                     work_count,
    ia_work_details const  *work);

/** TODO docs */
IA_API void IA_CALL
ia_yield(ia_work_chain chain);

/** TODO docs */
IA_FORCE_INLINE void
ia_submit_work_and_yield(
    u32                     work_count,
    ia_work_details const  *work)
{
    ia_yield(ia_submit_work(work_count, work));
}

/** TODO docs */
typedef struct ia_framework_hints {
    usize               default_stack_size;
    u32                 thread_count;
    u32                 fiber_count;
    u32                 log2_work_count;
} ia_framework_hints;

/** TODO docs */
typedef struct ia_framework_host {
    usize               total_ram;
    usize               page_size, page_size_in_use;
    i32                 cpu_thread_count, cpu_cores_count, cpu_package_count;
} ia_framework_host;

/** TODO docs */
typedef struct ia_framework {
    char const         *engine_name;
    char const         *app_name;
    u32                 engine_version;
    u32                 app_version;
    u64                 timer_begin;
    i32                 argc;
    char const        **argv;
    ia_framework_hints  hints;
    ia_framework_host   host;
} ia_framework;

/** TODO docs */
typedef void (IA_CALL *PFN_ia_framework_main)(void *userdata, ia_framework const *framework);
/** TODO docs */
#define FN_IA_FRAMEWORK_MAIN(fn, arg) \
    void IA_CALL fn(arg, ia_framework const *framework)

/** TODO docs */
IA_API i32 IA_CALL
ia_framework_main(
    PFN_ia_framework_main   entry_point,
    void                   *usedata,
    ia_framework           *framework);

#ifdef __cplusplus
}
#endif /* __cplusplus */
