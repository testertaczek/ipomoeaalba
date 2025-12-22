#pragma once
/** @file ia/base/atomic.h
 *  @brief Functions for atomic sequences, access and writes of data that will be shared between CPU threads.
 * 
 *  Atomic operations are necessary to guarantee safe data access in a multithreaded environment, and if used 
 *  correctly will prevent data races, deatlocks and other pesky issues. Most modern architectures have a 
 *  pretty straightforward lockless implementation of atomic operations and we'll rely on compiler intrinsics 
 *  to simulate the atomic behaviour with simple locks on targets that may have a problem with these. There 
 *  is a possibility to write architecture-specific code, but there is no need for that I guess.
 * 
 *  Whenever the macro `ia_atomic_is_lock_free(T)` is non-zero, this guarantees that the data type is lockless. 
 *  Even if arrays or structures can be defined as atomic, they won't have a lock free implementation. The macro 
 *  `IA_ATOMIC(T)` is used to enforce some core rules of how a variable is expected to be used within the context
 *  of atomic operations, and a hint for how it should be handled by the compiler, the CPU, and the programmer.
 * 
 *  @enum ia_atomic_model
 *      An atomic operation can both constrain code motion and be mapped to hardware instructions for 
 *      synchronization between threads. To which extent this happens is controlled by the memory models,
 *      which are listed in this enum in approximately ascending order of strength. The descriptions of 
 *      each model is only meant to roughly illustrate the effects and is not a specification. 
 *      C++11 memory models can be seen for precise semantics. The `atomic_order_consume` is 
 *      effectively broken, as compilers implement it as the stronger "acquire" memory model. 
 *      Because of this order consume has been excluded from this atomic wrapper.
 * 
 *      ia_atomic_model_monotonic   Implies no inter-thread ordering constraints.
 * 
 *      ia_atomic_model_acquire     Creates an inter-thread happens-before constraint from the release 
 *                                  (or stronger) semantic store to this acquire load. Can prevent hoisting 
 *                                  of code to before the operation.
 * 
 *      ia_atomic_model_release     Creates an inter-thread happens-before constraint to acquire (or stronger)
 *                                  semantic loads that read from this release store/write. Can prevent sinking
 *                                  of code to after the operation.
 * 
 *      ia_atomic_model_acq_rel     Combines the effects of both `acquire` and `release` models.
 * 
 *      ia_atomic_model_seq_cst     Enforces total ordering with all other `seq_cst` operations.
 * 
 *  All atomic operation wrappers are explicit, that they accept an `ia_atomic_model` argument, to specify 
 *  a memory model constraint for an operation. Every operation has an unordered variant defined, because they 
 *  are probably the most common. This is contraty to what the standard C11/C++11 atomic headers define, that 
 *  there is no "explicit" in any atomic operation signature. Additional constraints come from a weak and 
 *  strong memory model for the read-modify-write compare exchange operations. This is only destinct for 
 *  platforms with a weak memory ordering, thus on platforms like x86/amd64 the weak compare exchange gets
 *  redefined in the strong memory model instead - it's standard between available implementations.
 *  
 *  @def ia_atomic_init(*obj)
 *  @brief Initializes an atomic variable.
 * 
 *  @def ia_atomic_thread_fence(model)
 *  @brief Generic memory order-dependent fence synchronization primitive.
 * 
 *  @def ia_atomic_signal_fence(model)
 *  @brief Fence between a thread and a signal handler executed in the same thread.
 * 
 *  @def ia_atomic_write(*obj, value, model)
 *  @brief Atomically replaces the value of the atomic object with a non-atomic argument.
 * 
 *  @def ia_atomic_read(*obj, model)
 *  @brief Atomically obtains the value stored in an atomic object.
 * 
 *  @def ia_atomic_cmpxchg_strong(*rmw, *expected, value, model_success, model_failure)
 *  @brief Atomically compares the value of the atomic object with non-atomic argument and performs 
 *      atomic exchange if equal or atomic load if not. Strong memory model.
 * 
 *  @def ia_atomic_cmpxchg_weak(*rmw, *expected, value, model_success, model_failure)
 *  @brief Atomically compares the value of the atomic object with non-atomic argument and performs 
 *      atomic exchange if equal or atomic load if not. Used in loops, as it may fail even with true
 *      conditions on platforms with a weak memory model (e.g. aarch64, rv64gc).
 * 
 *  @def ia_atomic_cmpxchg(*rmw, *expected, value, is_weak, model_success, model_failure)
 *  @brief Atomically compares the value of the atomic object with non-atomic argument and performs 
 *      atomic exchange if equal or atomic load if not. Variant is chosen by a boolean `is_weak`.
 * 
 *  @def ia_atomic_xchg(*rmw, value, model)
 *  @brief Atomically replaces the value of the atomic object with non-atomic argument and returns
 *      the old value of the atomic.
 * 
 *  @def ia_atomic_add(*rmw, value, model)
 *  @brief Adds a non-atomic value to an atomic object and obtains the previous value of the atomic.
 * 
 *  @def ia_atomic_sub(*rmw, value, model)
 *  @brief Subtracts a non-atomic value to an atomic object and obtains the previous value of the atomic.
 * 
 *  @def ia_atomic_and(*rmw, value, model)
 *  @brief Replaces the atomic object with the result of bitwise AND with a non-atomic argument 
 *      and obtains the previous value of the atomic.
 * 
 *  @def ia_atomic_xor(*rmw, value, model)
 *  @brief Replaces the atomic object with the result of bitwise XOR with a non-atomic argument 
 *      and obtains the previous value of the atomic.
 * 
 *  @def ia_atomic_or(*rmw, value, model)
 *  @brief Replaces the atomic object with the result of bitwise OR with a non-atomic argument 
 *      and obtains the previous value of the atomic.
 * 
 *  @def ia_atomic_nand(*rmw, value, model)
 *  @brief Replaces the atomic object with the result of bitwise NAND with a non-atomic argument 
 *      and obtains the previous value of the atomic.
 */
#include <ia/base/types.h>
#include <ia/base/sanitize.h>

#define _IA_ATOMIC_IMPL_CXX    0
#define _IA_ATOMIC_IMPL_C11    1
#define _IA_ATOMIC_IMPL_GNUC   2
#define _IA_ATOMIC_IMPL_CLANG  3

#ifndef IA_ATOMIC_IMPL
    #ifdef __cplusplus
        #define IA_ATOMIC_IMPL _IA_ATOMIC_IMPL_CXX
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define IA_ATOMIC_IMPL _IA_ATOMIC_IMPL_C11
    #elif IA_CC_GNUC_VERSION_CHECK(4,7,0)
        #define IA_ATOMIC_IMPL _IA_ATOMIC_IMPL_GNUC
    #elif defined(IA_CC_CLANG_VERSION) && __has_extension(c_atomic)
        #define IA_ATOMIC_IMPL _IA_ATOMIC_IMPL_CLANG
    #else
        #error Invalid default intrinsics for IA_ATOMIC_IMPL.
    #endif /* __cplusplus */
#endif /* IA_ATOMIC_IMPL */

/* C++ atomic wrapper */
#if IA_ATOMIC_IMPL == _IA_ATOMIC_IMPL_CXX
    #include <atomic>

    typedef enum ia_atomic_model {
        ia_atomic_model_monotonic = std::memory_order_relaxed,
        ia_atomic_model_acquire   = std::memory_order_acquire,
        ia_atomic_model_release   = std::memory_order_release,
        ia_atomic_model_acq_rel   = std::memory_order_acq_rel,
        ia_atomic_model_seq_cst   = std::memory_order_seq_cst,
    } ia_atomic_model;

    #define IA_ATOMIC(T) std::atomic<T>

    #define ia_atomic_init              std::atomic_init
    #define ia_atomic_thread_fence      std::atomic_thread_fence
    #define ia_atomic_signal_fence      std::atomic_signal_fence

    #define ia_atomic_write             std::atomic_store_explicit
    #define ia_atomic_read              std::atomic_load_explicit
                                        
    #define ia_atomic_cmpxchg_strong    std::atomic_compare_exchange_strong_explicit
    #define ia_atomic_cmpxchg_weak      std::atomic_compare_exchange_weak_explicit
                                        
    #define ia_atomic_xchg              std::atomic_exchange_explicit
    #define ia_atomic_add               std::atomic_fetch_add_explicit
    #define ia_atomic_sub               std::atomic_fetch_sub_explicit
    #define ia_atomic_and               std::atomic_fetch_and_explicit
    #define ia_atomic_xor               std::atomic_fetch_xor_explicit
    #define ia_atomic_or                std::atomic_fetch_or_explicit

/* C11 stdatomic wrapper */
#elif IA_ATOMIC_IMPL == _IA_ATOMIC_IMPL_C11
    #include <stdatomic.h>

    typedef enum ia_atomic_model {
        ia_atomic_model_monotonic = memory_order_relaxed,
        ia_atomic_model_acquire   = memory_order_acquire,
        ia_atomic_model_release   = memory_order_release,
        ia_atomic_model_acq_rel   = memory_order_acq_rel,
        ia_atomic_model_seq_cst   = memory_order_seq_cst,
    } ia_atomic_model;

    #define IA_ATOMIC(T) _Atomic T

    #define ia_atomic_init              atomic_init
    #define ia_atomic_thread_fence      atomic_thread_fence
    #define ia_atomic_signal_fence      atomic_signal_fence
                                       
    #define ia_atomic_write             atomic_store_explicit
    #define ia_atomic_read              atomic_load_explicit
                                       
    #define ia_atomic_cmpxchg_strong    atomic_compare_exchange_strong_explicit
    #define ia_atomic_cmpxchg_weak      atomic_compare_exchange_weak_explicit
                                       
    #define ia_atomic_xchg              atomic_exchange_explicit
    #define ia_atomic_add               atomic_fetch_add_explicit
    #define ia_atomic_sub               atomic_fetch_sub_explicit
    #define ia_atomic_and               atomic_fetch_and_explicit
    #define ia_atomic_xor               atomic_fetch_xor_explicit
    #define ia_atomic_or                atomic_fetch_or_explicit

/* Clang __c11_atomic wrapper */
#elif IA_ATOMIC_IMPL == _IA_ATOMIC_IMPL_CLANG
    typedef enum ia_atomic_model {
        ia_atomic_model_monotonic = __ATOMIC_RELAXED,
        ia_atomic_model_acquire   = __ATOMIC_ACQUIRE,
        ia_atomic_model_release   = __ATOMIC_RELEASE,
        ia_atomic_model_acq_rel   = __ATOMIC_ACQ_REL,
        ia_atomic_model_seq_cst   = __ATOMIC_SEQ_CST,
    } ia_atomic_model;

    #define IA_ATOMIC(T) T

    #define ia_atomic_init              __c11_atomic_init
    #define ia_atomic_thread_fence      __c11_atomic_thread_fence
    #define ia_atomic_signal_fence      __c11_atomic_signal_fence
                                        
    #define ia_atomic_write             __c11_atomic_store
    #define ia_atomic_read              __c11_atomic_load
                                        
    #define ia_atomic_cmpxchg_strong    __c11_atomic_compare_exchange_strong
    #define ia_atomic_cmpxchg_weak      __c11_atomic_compare_exchange_weak
                                        
    #define ia_atomic_xchg              __c11_atomic_exchange
    #define ia_atomic_add               __c11_atomic_fetch_add
    #define ia_atomic_sub               __c11_atomic_fetch_sub
    #define ia_atomic_and               __c11_atomic_fetch_and
    #define ia_atomic_xor               __c11_atomic_fetch_xor
    #define ia_atomic_or                __c11_atomic_fetch_or
    #define ia_atomic_nand              __c11_atomic_fetch_nand

/* GCC __atomic wrapper */
#elif IA_ATOMIC_IMPL == _IA_ATOMIC_IMPL_GCC
    typedef enum ia_atomic_model {
        ia_atomic_model_monotonic = __ATOMIC_RELAXED,
        ia_atomic_model_acquire   = __ATOMIC_ACQUIRE,
        ia_atomic_model_release   = __ATOMIC_RELEASE,
        ia_atomic_model_acq_rel   = __ATOMIC_ACQ_REL,
        ia_atomic_model_seq_cst   = __ATOMIC_SEQ_CST,
    } ia_atomic_model;

    #define IA_ATOMIC(T) T

    #define ia_atomic_init(o, val)      (*(o) = (val))
    #define ia_atomic_thread_fence      __atomic_thread_fence
    #define ia_atomic_signal_fence      __atomic_signal_fence
                                        
    #define ia_atomic_write             __atomic_store_n
    #define ia_atomic_read              __atomic_load_n

    #define ia_atomic_cmpxchg_strong(dst, exp, src, model1, model2) \
        __atomic_compare_exchange_n(dst, exp, src, 0, model1, model2)
    #define ia_atomic_cmpxchg_weak(dst, exp, src, model1, model2) \
        __atomic_compare_exchange_n(dst, exp, src, 1, model1, model2)
    #define ia_atomic_cmpxchg           __atomic_compare_exchange_n

    #define ia_atomic_xchg              __atomic_exchange_n
    #define ia_atomic_add               __atomic_fetch_add
    #define ia_atomic_sub               __atomic_fetch_sub
    #define ia_atomic_and               __atomic_fetch_and
    #define ia_atomic_xor               __atomic_fetch_xor
    #define ia_atomic_or                __atomic_fetch_or
    #define ia_atomic_nand              __atomic_fetch_nand
#else
    #error An invalid atomic implementation via IA_ATOMIC_IMPL was requested.
#endif /* IA_ATOMIC_IMPL */

/** Generic wrapper for compare exchange, where weakness is a boolean argument. */
#ifndef ia_atomic_cmpxchg
#define ia_atomic_cmpxchg(dst, exp, src, is_weak, success_model, failure_model) \
    (is_weak ? ia_atomic_cmpxchg_weak(dst, exp, src, success_model, failure_model) \
             : ia_atomic_cmpxchg_strong(dst, exp, src, success_model, failure_model))
#endif /* ia_atomic_cmpxchg */

/** Generic wrapper for NAND read-modify-write operation. */
#ifndef ia_atomic_nand
#define ia_atomic_nand(rmw, operand, model) \
    do { \
        typeof(operand) __old = ia_atomic_read(rmw, ia_atomic_model_monotonic); \
        typeof(operand) __new = ~(__old & operand); \
    } while (!ia_atomic_cmpxchg_weak(rmw, &__old, __new, model, model))
#endif /* ia_atomic_nand */

typedef IA_ATOMIC(i8)      atomic_i8;
typedef IA_ATOMIC(i16)     atomic_i16;
typedef IA_ATOMIC(i32)     atomic_i32;
typedef IA_ATOMIC(i64)     atomic_i64;

typedef IA_ATOMIC(u8)      atomic_u8;
typedef IA_ATOMIC(u16)     atomic_u16;
typedef IA_ATOMIC(u32)     atomic_u32;
typedef IA_ATOMIC(u64)     atomic_u64;

typedef IA_ATOMIC(iptr)    atomic_iptr;
typedef IA_ATOMIC(uptr)    atomic_uptr;
typedef IA_ATOMIC(isize)   atomic_isize;
typedef IA_ATOMIC(usize)   atomic_usize;
typedef IA_ATOMIC(bool)    atomic_bool;

/** When an operation on an atomic variable is not expected to synchronize
 *  with operations on other (atomic or non-atomic) variables, no memory barriers
 *  are needed and the relaxed memory ordering can be used. These macros make 
 *  writing such monotonic operations less daunting, but not invisible. */

#define ia_atomic_write_monotonic(dst, val) \
    ia_atomic_write(dst, val, ia_atomic_model_monotonic)
#define ia_atomic_read_monotonic(dst) \
    ia_atomic_read(dst, ia_atomic_model_monotonic)

#define ia_atomic_cmpxchg_monotonic(dst, exp, val, is_weak) \
    ia_atomic_cmpxchg(dst, exp, val, is_weak, ia_atomic_model_monotonic, ia_atomic_model_monotonic)
#define ia_atomic_cmpxchg_strong_monotonic(dst, exp, val) \
    ia_atomic_cmpxchg_strong(dst, exp, val, ia_atomic_model_monotonic, ia_atomic_model_monotonic)
#define ia_atomic_cmpxchg_weak_monotonic(dst, exp, val) \
    ia_atomic_cmpxchg_weak(dst, exp, val, ia_atomic_model_monotonic, ia_atomic_model_monotonic)

/** These implicitly monotonic RMW operations always return the value that had previously been in `rmw`:
 *      T ia_atomic_OP(T *rmw, T val)
 *      { tmp = *rmw; *rmw op= val; return tmp; }
 *      { tmp = *rmw; *rmw = ~(*rmw & val); return tmp; } [nand] */
#define ia_atomic_xchg_monotonic(rmw, val) \
    ia_atomic_xchg(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_add_monotonic(rmw, val) \
    ia_atomic_add(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_sub_monotonic(rmw, val) \
    ia_atomic_sub(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_and_monotonic(rmw, val) \
    ia_atomic_and(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_xor_monotonic(rmw, val) \
    ia_atomic_xor(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_or_monotonic(rmw, val) \
    ia_atomic_or(rmw, val, ia_atomic_model_monotonic)
#define ia_atomic_nand_monotonic(rmw, val) \
    ia_atomic_nand(rmw, val, ia_atomic_model_monotonic)

/** Hint to the CPU that the current thread is in a busy-wait spin loop. */
IA_FORCE_INLINE void
ia_cpu_relax(void) {
#if defined(IA_ARCH_X86) || defined(IA_ARCH_AMD64)
    /* pause improves HT fairness, reduces power */
    __asm__ __volatile__("pause" ::: "memory");
#elif defined(IA_ARCH_ARM) || defined(IA_ARCH_AARCH64)
    /* ARM yield hint */
    __asm__ __volatile__("yield" ::: "memory");
#elif defined(IA_ARCH_RISCV)
    /* RISC-V pause hint (Zihintpause if available) */
    __asm__ __volatile__(".insn i 0x0f, 0, x0, x0, 0x010" ::: "memory");
#else
    /* fallback to compiler barrier */
    __asm__ __volatile__("" ::: "memory");
#endif
}

/** A spinlock type, used for busy-wait synchronization between threads. */
typedef atomic_i32 IA_THREAD_SAFETY_CAPABILITY("mutex") ia_spinlock;
#define ia_spinlock_init {0}

/** Acquires a spinlock, tries an optimal acquire before spinning. */
IA_FORCE_INLINE void
ia_spinlock_acquire(ia_spinlock *lock)
    IA_THREAD_SAFETY_ACQUIRE(lock)
{
    i32 expected = 0;
    if (IA_LIKELY(ia_atomic_cmpxchg_weak(lock, &expected, 1, ia_atomic_model_acquire, ia_atomic_model_monotonic)))
        return;
    for (;;) {
        while (ia_atomic_read_monotonic(lock) != 0)
            ia_cpu_relax();
        expected = 0;
        if (ia_atomic_cmpxchg_weak(lock, &expected, 1, ia_atomic_model_acquire, ia_atomic_model_monotonic))
            return;
    }
}

/** Tries to acquire a spinlock, may fail. */
IA_FORCE_INLINE bool
ia_spinlock_try_acquire(ia_spinlock *lock)
    IA_THREAD_SAFETY_TRY_ACQUIRE(lock)
{ 
    i32 expected = 0;
    return ia_atomic_cmpxchg_weak(lock, &expected, 1, ia_atomic_model_acquire, ia_atomic_model_monotonic); 
}

/** Releases a spinlock acquired with `ia_spinlock_acquire`. */
IA_FORCE_INLINE void
ia_spinlock_release(ia_spinlock *lock) 
    IA_THREAD_SAFETY_RELEASE(lock)
{
#ifdef IA_DEBUG
    /* tries to catch double-unlock */
    i32 prev = ia_atomic_xchg(lock, 0, ia_atomic_model_release);
    IA_ASSUME(prev == 1 && "spinlock unlock without a locked state");
#else
    ia_atomic_write(lock, 0, ia_atomic_model_release);
#endif
}

/** The scoped spinlock has no implications on performance or synchronization and 
 *  no runtime cost over a normal spinlock. It has but compile-time implications.
 *  A scoped guard forces a visible structure acquisition happens in one expression,
 *  release is tied to a concrete object, and it becomes harder to forget to release,
 *  or mismatch acquire/release pairs. It's advised to use spinlocks this way.
 *
 *  Example usage:
 *      struct data_type {
 *          ia_spinlock lock;
 *          i32         counter IA_THREAD_SAFETY_GUARDED_BY(lock);
 *      };
 *
 *      void f1(struct data_type *d) {
 *          ia_spinlock_scoped guard = ia_spinlock_scoped_acquire(&d->lock);
 *          ...
 *          d->counter++;
 *          if (fail) goto out;
 *          ...
 *          out:
 *          ia_spinlock_scoped_release(&guard);
 *      } 
 *
 *      void f2(struct data_type *d) {
 *          ia_defer_begin
 *          ia_spinlock_scoped guard = ia_spinlock_scoped_acquire(&d->lock);
 *          ia_defer({ ia_spinlock_scoped_release(&guard); })
 *          ...
 *          d->counter++;
 *          ...
 *          ia_defer_return;
 *      } */
typedef struct IA_THREAD_SAFETY_SCOPED_CAPABILITY ia_spinlock_scoped {
    ia_spinlock *lock;
} ia_spinlock_scoped;

/** Acquires a spinlock, returns a scoped guard lock. */
IA_FORCE_INLINE ia_spinlock_scoped
ia_spinlock_scoped_acquire(ia_spinlock *lock)
    IA_THREAD_SAFETY_ACQUIRE(lock)
{
    ia_spinlock_acquire(lock);
    return (ia_spinlock_scoped){ lock };
}

/** Releases a spinlock acquired with `ia_spinlock_scoped_acquire`. */
IA_FORCE_INLINE void
ia_spinlock_scoped_release(ia_spinlock_scoped *s)
    IA_THREAD_SAFETY_RELEASE(s->lock)
{
    ia_spinlock_release(s->lock);
}
