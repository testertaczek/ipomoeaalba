#pragma once
/** @file ia/foundation.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/base/targets.h>
#include <ia/base/types.h>
#include <ia/base/magic.h>
#include <ia/base/atomic.h>
#include <ia/base/endian.h>
#include <ia/base/sanitize.h>
#include <ia/base/defer.h>
#include <ia/base/keycodes.h>
#include <ia/base/scancodes.h>
#include <ia/base/system.h>
#include <ia/base/filesystem.h>
#include <ia/base/memory.h>
#include <ia/base/work.h>
#include <ia/base/log.h>

#include <ia/compute/simd.h>
#include <ia/compute/aabb.h>
#include <ia/compute/bits.h>
#include <ia/compute/camera.h>
#include <ia/compute/crypto.h>
#include <ia/compute/lz4.h>
#include <ia/compute/matrix.h>
#include <ia/compute/quaternion.h>
#include <ia/compute/trigonometry.h>
#include <ia/compute/vector.h>

#include <ia/datastructures/arena.h>
#include <ia/datastructures/balloc.h>
#include <ia/datastructures/bitset.h>
#include <ia/datastructures/dagraph.h>
#include <ia/datastructures/darray.h>
#include <ia/datastructures/deque.h>
#include <ia/datastructures/hashmap.h>
#include <ia/datastructures/map.h>
#include <ia/datastructures/mpmc.h>
#include <ia/datastructures/sparse.h>
#include <ia/datastructures/stack.h>
#include <ia/datastructures/strbuf.h>
#include <ia/datastructures/switch.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** TODO docs */
typedef struct ia_foundation_hints {
    usize                   default_stack_size;
    u32                     thread_count;
    u32                     fiber_count;
    u32                     log2_work_count;
} ia_foundation_hints;

/** TODO docs */
typedef struct ia_foundation_host {
    u64                     timer_begin;
    usize                   total_ram;
    usize                   page_size_in_use;
    ia_hugepage_sizes       hugepage_sizes;
    i32                     cpu_thread_count, cpu_cores_count, cpu_package_count;
} ia_foundation_host;

/** TODO docs */
typedef struct ia_foundation {
    char const             *engine_name;
    char const             *app_name;
    u32                     engine_version;
    u32                     app_version;
    i32                     argc;
    char const            **argv;
    ia_foundation_hints     hints;
    ia_foundation_host      host;
} ia_foundation;

/** TODO docs */
typedef struct ia_interface_header {
    ia_foundation const    *foundation;    
    i32                     adapter_impl;
    u32                     version;
    ia_work_fn              destructor;
    char const             *name;
} ia_interface_header;

/** TODO docs */
#define IA_DECL_LIBRARY(X) \
    typedef union ia_##X##_library { \
        struct ia_##X##_adapter    *a; \
        struct ia_##X##_interface const  *i; \
        ia_interface_header        *h; \
        void                       *v; \
    } ia_##X##_library; \
    typedef struct ia_##X##_adapter     ia_##X##_adapter; \
    typedef struct ia_##X##_interface   ia_##X##_interface

/** TODO docs */
typedef ia_result (IA_CALL *ia_library_impl_fn)(ia_foundation const *foundation, void *out_adapter);
#define IA_DECL_LIBRARY_IMPL(X, KIND) \
    ia_result IA_CALL ia_##X##_impl_##KIND(ia_foundation const *foundation, ia_##X##_adapter *out_adapter)

/** TODO docs */
typedef i32 (IA_CALL *ia_foundation_main_fn)(void *data, ia_foundation const *foundation);
#define IA_FOUNDATION_MAIN_FN(fn, arg) \
    i32 IA_CALL fn(arg, ia_foundation const *foundation)

/** TODO docs */
IA_NONNULL(1,3) IA_API i32 IA_CALL
ia_foundation_main(
    ia_foundation_main_fn   main_fn,
    void                   *main_data,
    ia_foundation          *foundation);

#ifdef __cplusplus
}
#endif /* __cplusplus */
