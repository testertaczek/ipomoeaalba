#pragma once

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
    usize               default_stack_size;
    u32                 thread_count;
    u32                 fiber_count;
    u32                 log2_work_count;
} ia_foundation_hints;

/** TODO docs */
typedef struct ia_foundation_host {
    u64                 timer_begin;
    usize               total_ram;
    usize               page_size_in_use;
    ia_hugepage_sizes   hugepage_sizes;
    i32                 cpu_thread_count, cpu_cores_count, cpu_package_count;
} ia_foundation_host;

/** TODO docs */
typedef struct ia_foundation {
    char const         *engine_name;
    char const         *app_name;
    u32                 engine_version;
    u32                 app_version;
    i32                 argc;
    char const        **argv;
    ia_foundation_hints hints;
    ia_foundation_host  host;
} ia_foundation;

/** TODO docs */
typedef void (IA_CALL *ia_foundation_main_fn)(void *data, ia_foundation const *foundation);
    
/** TODO docs */
#define IA_FOUNDATION_MAIN_FN(fn, arg) \
    void IA_CALL fn(arg, ia_foundation const *foundation)

/** TODO docs */
IA_NONNULL(1,3) IA_API i32 IA_CALL
ia_foundation_main(
    ia_foundation_main_fn   main_fn,
    void                   *main_data,
    ia_foundation          *foundation);

#ifdef __cplusplus
}
#endif /* __cplusplus */
