#pragma once
/** @file ia/base/work.h
 *  @brief A fiber-based job system.
 *
 *  This job system is implemented based on ideas presented by Naughty Dog in the GDC2015 talk
 *  "Parallelizing the Naughty Dog Engine Using Fibers".
 *
 *  [Naughty Dog Video]
 *  https://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
 *
 *  [Naughty Dog Slides]
 *  https://media.gdcvault.com/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf
 *
 *  Since the job system is multithreaded, the work is not guaranteed to be executed in the order 
 *  that it was submitted. Work runs in parallel. Yielding jobs doesn't "block" in the traditional
 *  sense, instead the current context of execution is switched with a fiber that contains details 
 *  of a new job to run. Only when all work of a single submission is done, a yielding fiber may
 *  resume and return from an `ia_yield` call. Context switching is very CPU specific. This is 
 *  implemented in assembly for every architecture and platform ABI that is supported. A great 
 *  deal of help in this task is the source code of Boost C++ fiber context library. It has most 
 *  of this stuff figured out.
 *
 *  [Boost::Context]
 *  https://github.com/boostorg/context
 *
 *  [Fibers, Oh My!]
 *  https://graphitemaster.github.io/fibers/
 *
 *  Work is defined with the following function signature: `void fn(void *)`. The typedef `ia_work_fn`
 *  can be used to cast compatible functions of different pointer argument types, into a signature that 
 *  the job system may accept, and implicitly perform the type cast during a function call. The only 
 *  assumption is that the work function must use a `stdcall` calling convention, enforced by `IA_CALL`.
 *  A work submit is defined by an array of details for every job to run, and an atomic counter 
 *  `ia_work_chain` is given in return for the caller to wait on when yielding the fiber.
 *
 *  May get into improving context switching branch prediction, if this can improve speed: 
 *  http://www.crystalclearsoftware.com/soc/coroutine/coroutine/linuxasm.html
 */
#include <ia/base/types.h>
#include <ia/base/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Defines a job function. Prototype defined by `IA_WORK_FN`. */
typedef void (IA_CALL *ia_work_fn)(void *work);

/** Redefines the work typedef with a compatible pointer type. */
#define IA_WORK_FN_TYPEDEF(pfn, T) \
    typedef void (IA_CALL *pfn)(T *work)

/** Defines an `ia_work_fn` prototype. */
#define IA_WORK_FN(fn, T) \
    void IA_CALL fn(T *work)

/** Controls how the internal scheduler distributes this work. */
typedef enum ia_work_schedule : i8 {
    ia_work_schedule_default = 0,   /**< No implications for the scheduler, generally low priority. */
    ia_work_schedule_aggressive,    /**< Is important and should run on a higher-priority queue. */
    ia_work_schedule_main_affinity, /**< May only ever run from the main thread, OS implications, highest priority. */
} ia_work_schedule;

/** Details of a work term to be executed by the job system. */
typedef struct ia_work_details {
    ia_work_fn          fn;         /**< Work to run. */
    void               *data;       /**< Data for the work. */
    u32                 stacksize;  /**< Minimal stack size required to run this work, value 0 set's it to default. */
    ia_work_schedule    schedule;   /**< If true work may only be done from the main thread. */
    char const         *name;       /**< A fiber will adopt this name for profiling. */
} ia_work_details;

/** An atomic counter bound to a work submission. It's always returned by the job system after a submit. 
 *  The chain is used to "wait" for the submitted work. A yielding fiber while waiting for the submitted 
 *  work to finish, instead of blocking or busy-waiting, will implicitly perform a context switch.
 *  This synchronization mechanism is completely hidden from the user of the job system. */
typedef atomic_isize *ia_work_chain;

/** Returns an index of the worker thread the caller's fiber context is running on. This index can be used 
 *  to access per-thread transient data, persistent thread-local assumptions must not be made.
 *
 *  The returned value is only valid for the duration of the current execution slice. 
 *  A fiber may resume execution on a different worker thread after any call to `ia_yield()`.
 *  This is because fibers may migrate between threads when work is resumed.
 *
 *  @return Current worker thread index in the range [0..thread_count]. */
IA_HOT_FN IA_API i32 IA_CALL
ia_worker_thread_index(void);

/** @return `true` if called from the main thread, otherwise `false`. */
IA_FORCE_INLINE bool 
ia_worker_thread_main_affinity(void)
{ return ia_worker_thread_index() == 0; }

/** Submits `work_count` of work to the queue, using details provided by the array `work`. This function 
 *  will return IMMEDIATELY, and the given work will be resolved in the background running on any other thread.
 *
 *  @return A chain set to a value that can be used to synchronize with the work to finish. */
IA_NONNULL_ALL IA_API ia_work_chain IA_CALL
ia_submit_work(
    i32                     work_count,
    ia_work_details const  *work);

/** If chain is not nullptr, the fiber will yield and won't resume until the completion of work it chained.
 *  Otherwise, if no valid chain is given, then the fiber may or may not yield to the job system before returning. 
 *  The chain becomes invalidated and any more yields will be asserted, as they indicate innapropriate synchronization 
 *  effort. Because fibers may migrate between threads, the thread of execution may change after the yield. */
IA_NONNULL_ALL IA_API void IA_CALL
ia_yield(ia_work_chain chain);

#ifdef __cplusplus
}
#endif /* __cplusplus */
