#pragma once
/** @file ia/render.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/foundation.h>

/* ia_render_library: ia_render_adapter, ia_render_interface */
IA_DECL_LIBRARY(render);

/* Represents GPU resources and device objects, supports many-to-one cardinality on explicit heterogenous mGPU setups. */
typedef struct { ia_identifier id; } ia_render_buffer;
typedef struct { ia_identifier id; } ia_render_texture;
typedef struct { ia_identifier id; } ia_render_sampler;
typedef struct { ia_identifier id; } ia_render_bvh;
typedef struct { ia_identifier id; } ia_render_pipeline;
typedef struct { ia_identifier id; } ia_render_swapchain;
typedef struct { ia_identifier id; } ia_render_staged_commands;
typedef struct { ia_identifier id; } ia_render_query;

#define IA_RENDER_MAX_COMPUTE_QUEUE_COUNT       (8)
#define IA_RENDER_MAX_TRANSFER_QUEUE_COUNT      (2)

#define IA_RENDER_QUEUE_MAIN_BEGIN_INDEX        (0)
#define IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX     (1)
#define IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX    (IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + IA_RENDER_MAX_COMPUTE_QUEUE_COUNT)

typedef i8 ia_render_queue_mask;
typedef enum ia_render_queue_type : ia_render_queue_mask {
    ia_render_queue_type_main,
    ia_render_queue_type_compute,
    ia_render_queue_type_transfer,
    ia_render_queue_type_none = -1,
} ia_render_queue_type;

typedef struct ia_render_queue {
    ia_render_queue_type    type;
    i8                      index;
} ia_render_queue;

static constexpr ia_render_queue IA_RENDER_QUEUE_MAIN = { ia_render_queue_type_main, IA_RENDER_QUEUE_MAIN_BEGIN_INDEX };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_0 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 0 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_1 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 1 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_2 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 2 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_3 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 3 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_4 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 4 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_5 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 5 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_6 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 6 };
static constexpr ia_render_queue IA_RENDER_QUEUE_COMPUTE_7 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 7 };
static constexpr ia_render_queue IA_RENDER_QUEUE_TRANSFER_0 = { ia_render_queue_type_transfer, IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX + 0 };
static constexpr ia_render_queue IA_RENDER_QUEUE_TRANSFER_1 = { ia_render_queue_type_transfer, IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX + 1 };

typedef struct ia_render_device_info {
    char name[256];
} ia_render_device_info;

struct ia_render_interface {
    ia_interface_header     header;
};

enum : i32 {
    ia_render_adapter_dummy = 0,
    ia_render_adapter_vulkan,
    ia_render_adapter_d3d12,
    ia_render_adapter_metal,
    ia_render_adapter_webgpu,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IA_RENDER_VULKAN
IA_API IA_DECL_LIBRARY_IMPL(render, vulkan);
#endif /* IA_RENDER_VULKAN */
#ifdef IA_RENDER_D3D12
IA_API IA_DECL_LIBRARY_IMPL(render, d3d12);
#endif /* IA_RENDER_D3D12 */
#ifdef IA_RENDER_METAL
IA_API IA_DECL_LIBRARY_IMPL(render, metal);
#endif /* IA_RENDER_METAL */
#ifdef IA_RENDER_WEBGPU
IA_API IA_DECL_LIBRARY_IMPL(render, webgpu);
#endif /* IA_RENDER_WEBGPU */

/** The dummy rendering backend is always available. */
IA_API IA_DECL_LIBRARY_IMPL(render, dummy);

/** Picks the first working backend in an order optimal for the host platform.
 *  The `dummy` backend is ignored, and if any other backend fails, must be called explicitly. */
IA_API IA_DECL_LIBRARY_IMPL(render, auto);

#ifdef __cplusplus
}
#endif /* __cplusplus */
