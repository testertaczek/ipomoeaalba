#pragma once
/** @file ia/render_graph.h
 *  @brief Graph for rendering logic.
 *
 *  The purpose of a render graph is to handle transient resources, manage lifetimes
 *  efficiently, include the proper resource barriers and transitions, schedule the 
 *  command queues, manage device memory, parallelize work submission and execution
 *  on different frequencies (command queues). It's supposed to be constructed at 
 *  every frame, or assembled from a cached precompiled graph.
 *
 *  A render graph has two stages in his lifecycle:
 *
 *  - Construction, that records render passes and resources used as inputs and outputs.
 *    Its by design a sequential operation.
 *
 *  - Compilation, that evaluates the render graph and assembles a list of compiled commands.
 *    Its work that can be very much parallelized.
 * 
 *  TODO docs, frostbite render graph
 */
#include <ia/foundation.h>
#include <ia/render.h>
#include <ia/render_commands.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A render pass can specify valid scheduling, and also request transfers 
 *  in or out of the pass. The render graph controls them. */
typedef enum ia_render_schedule : u8 {
    ia_render_schedule_no_devices               = (1u << 0), /**< The pass is disabled. */
    ia_render_schedule_all_devices              = (1u << 1), /**< Pass runs on all devices. */
    ia_render_schedule_primary_device           = (1u << 2), /**< Pass only runs on the primary device. */
    ia_render_schedule_secondary_devices        = (1u << 3), /**< Pass runs on secondaries if GPU count > 1, otherwise primary. */
    ia_render_schedule_only_secondary_devices   = (1u << 4), /**< Pass only runs on secondaries, disabled unless mGPU. */
} ia_render_schedule;

/** With sort-first approach, work is divided between devices as partitions on the X-axis (width). 
 *  This specifies rules for the transfer for both the source and the destination GPU(s),
 *  allowing for a variety of transfer patterns:
 *
 *  partition_all -> partition_all
 *  - Copies full resource on one GPU to full resource on all specified GPUs.
 *
 *  partition_all -> partition_isolated 
 *  - Copies full resource on one GPU to isolated regions on all specified GPUs (partial copies).
 *
 *  partition_isolated -> partition_all 
 *  - (invalid configuration, will be asserted).
 *
 *  partition_isolated -> partition_isolated 
 *  - Copies isolated region on one GPU to isolated regions on all specified GPUs (partial copies). */
typedef enum ia_render_transfer_partition : u8 {
    ia_render_transfer_partition_all            = (1u << 0), /**< Select all partitions from device. */
    ia_render_transfer_partition_isolated       = (1u << 1), /**< Select isolated region from device. */
} ia_render_transfer_partition;

/** Transfers specify what partitions are copied from source to destination, and specify 
 *  rules for transfer destination, allowing for basic copies, multi-cast copies, etc. */
typedef enum ia_render_transfer_filter : u8 {
    ia_render_transfer_filter_all_devices       = (1u << 0), /**< Transfer completes on all devices. */
    ia_render_transfer_filter_primary_device    = (1u << 1), /**< Transfer completes on the primary device. */
    ia_render_transfer_filter_secondary_devices = (1u << 2), /**< Transfer completes on all secondary devices. */
} ia_render_transfer_filter;

/** TODO docs */
typedef struct ia_render_graph {
    ia_render_library   library;
    ia_dagraph          dag;
} ia_render_graph;

#ifdef __cplusplus
}
#endif /* __cplusplus */
