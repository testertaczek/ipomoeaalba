#pragma once
/** @file ia/render.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/foundation.h>

/* ia_render_library: ia_render_adapter, ia_render_interface */
IA_DECL_LIBRARY(render);

/** Controls per-device work, we limit possible rendering devices to less or equal 32. */
typedef ia_bitmask ia_render_device_mask;
#define ia_render_device_mask_all           (0xffffffffu)
#define ia_render_device_mask_primary       (1u << 0)
#define ia_render_device_mask_secondaries   (0xfffffffeu)

#ifndef IA_RENDER_MAX_DEVICES
#define IA_RENDER_MAX_DEVICES (sizeof(ia_render_device_mask)*8)
#endif
static_assert(ia_is_pow2(IA_RENDER_MAX_DEVICES), "maximum rendering device count must be a power of 2");

/* Represents GPU resources and device objects, supports many-to-one cardinality on explicit heterogenous mGPU setups. */
typedef struct { ia_identifier id; } ia_render_buffer;
typedef struct { ia_identifier id; } ia_render_texture;
typedef struct { ia_identifier id; } ia_render_texture_view;
typedef struct { ia_identifier id; } ia_render_sampler;
typedef struct { ia_identifier id; } ia_render_bvh;
typedef struct { ia_identifier id; } ia_shader;
typedef struct { ia_identifier id; } ia_shader_pipeline;
typedef struct { ia_identifier id; } ia_swapchain;
typedef struct { ia_identifier id; } ia_render_query_pool;
typedef struct { ia_identifier id; } ia_compiled_commands;

#define IA_RENDER_MAX_COMPUTE_QUEUE_COUNT           (8)
#define IA_RENDER_MAX_TRANSFER_QUEUE_COUNT          (2)

#define IA_RENDER_QUEUE_MAIN_BEGIN_INDEX            (0)
#define IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX         (1)
#define IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX        (IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + IA_RENDER_MAX_COMPUTE_QUEUE_COUNT)
#define IA_RENDER_QUEUE_SPARSE_BINDING_BEGIN_INDEX  (IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX + IA_RENDER_MAX_TRANSFER_QUEUE_COUNT)
#define IA_RENDER_QUEUE_VIDEO_DECODE_BEGIN_INDEX    (IA_RENDER_QUEUE_SPARSE_BINDING_BEGIN_INDEX + 1)
#define IA_RENDER_QUEUE_VIDEO_ENCODE_BEGIN_INDEX    (IA_RENDER_QUEUE_SPARSE_BINDING_BEGIN_INDEX + 2)

/** Can be composed into masks for scheduling command buffers. */
typedef u8 ia_render_queue_mask;
typedef enum ia_render_queue_type : ia_render_queue_mask {
    ia_render_queue_type_any = 0,
    ia_render_queue_type_main = (1u << 0),
    ia_render_queue_type_compute = (1u << 1),
    ia_render_queue_type_transfer = (1u << 2),
    ia_render_queue_type_sparse_binding = (1u << 3),
    ia_render_queue_type_video_decode = (1u << 4),
    ia_render_queue_type_video_encode = (1u << 5),
} ia_render_queue_type;

typedef struct ia_render_queue {
    ia_render_queue_type    type;
    i8                      index;
} ia_render_queue;
/* forward declaration */
struct ia_render_command_stream;

static constexpr ia_render_queue ia_render_queue_main = { ia_render_queue_type_main, IA_RENDER_QUEUE_MAIN_BEGIN_INDEX };
static constexpr ia_render_queue ia_render_queue_compute_0 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 0 };
static constexpr ia_render_queue ia_render_queue_compute_1 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 1 };
static constexpr ia_render_queue ia_render_queue_compute_2 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 2 };
static constexpr ia_render_queue ia_render_queue_compute_3 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 3 };
static constexpr ia_render_queue ia_render_queue_compute_4 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 4 };
static constexpr ia_render_queue ia_render_queue_compute_5 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 5 };
static constexpr ia_render_queue ia_render_queue_compute_6 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 6 };
static constexpr ia_render_queue ia_render_queue_compute_7 = { ia_render_queue_type_compute, IA_RENDER_QUEUE_COMPUTE_BEGIN_INDEX + 7 };
static constexpr ia_render_queue ia_render_queue_transfer_0 = { ia_render_queue_type_transfer, IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX + 0 };
static constexpr ia_render_queue ia_render_queue_transfer_1 = { ia_render_queue_type_transfer, IA_RENDER_QUEUE_TRANSFER_BEGIN_INDEX + 1 };

#define IA_RENDER_MAX_ROOT_CONSTANT_WORD_SIZE   (32u)
#define IA_RENDER_MAX_ROOT_CONSTANT_BYTE_SIZE   (IA_RENDER_MAX_ROOT_CONSTANT_WORD_SIZE * 4)
#define IA_RENDER_PIPELINE_LAYOUT_COUNT         (IA_RENDER_MAX_ROOT_CONSTANT_WORD_SIZE + 1)
#define IA_RENDER_SHADER_UNUSED                 (UINT32_MAX)

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on hardware and backend implementations. */
typedef enum ia_format : i16 {
    ia_format_unknown = 0,
    ia_format_r1_unorm,     /**< 1-bit no format */
    ia_format_a8_unorm,     /**< 8-bit alpha */
    /** depth formats */
    ia_format_d16_unorm,    /**< 16-bit unorm depth */
    ia_format_d16s8_unorm,  /**< 24-bit unorm depth/stencil */
    ia_format_d24s8_unorm,  /**< 32-bit unorm depth/stencil */
    ia_format_d32_sfloat,   /**< 32-bit float depth */
    ia_format_d0s8_unorm,   /**< 8-bit unorm stencil */
    /** 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
    ia_format_r8_snorm,
    ia_format_r8_unorm,
    ia_format_r8_sint,
    ia_format_r8_uint,
    /** 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
    ia_format_r16_sfloat,
    ia_format_r16_snorm,
    ia_format_r16_unorm,
    ia_format_r16_sint,
    ia_format_r16_uint,
    ia_format_rg8_snorm,
    ia_format_rg8_unorm,
    ia_format_rg8_sint,
    ia_format_rg8_uint,
    /** 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
    ia_format_rgb8_snorm,
    ia_format_rgb8_unorm,
    ia_format_rgb8_sint,
    ia_format_rgb8_uint,
    /** 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
    ia_format_r32_sfloat,
    ia_format_r32_sint,
    ia_format_r32_uint,
    ia_format_rg16_sfloat,
    ia_format_rg16_snorm,
    ia_format_rg16_unorm,
    ia_format_rg16_sint,
    ia_format_rg16_uint,
    ia_format_rgba8_srgb,
    ia_format_rgba8_unorm,
    ia_format_rgba8_sint,
    ia_format_rgba8_uint,
    ia_format_bgra8_snorm,
    ia_format_bgra8_unorm,
    ia_format_bgra8_sint,
    ia_format_bgra8_uint,
    ia_format_rgb10a2,
    ia_format_bgr10a2,
    /** 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_rgb16_sfloat,
    ia_format_rgb16_snorm,
    ia_format_rgb16_unorm,
    ia_format_rgb16_sint,
    ia_format_rgb16_uint,
    /** 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_r64_sfloat,
    ia_format_r64_sint,
    ia_format_r64_uint,
    ia_format_rg32_sfloat,
    ia_format_rg32_sint,
    ia_format_rg32_uint,
    ia_format_rgba16_sfloat,
    ia_format_rgba16_snorm,
    ia_format_rgba16_unorm,
    ia_format_rgba16_sint,
    ia_format_rgba16_uint,
    /** 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_rgb32_sfloat,
    ia_format_rgb32_sint,
    ia_format_rgb32_uint,
    /** 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_rgba32_sfloat,
    ia_format_rgba32_sint,
    ia_format_rgba32_uint,
    ia_format_rg64_float,
    ia_format_rg64_sint,
    ia_format_rg64_uint,
    /** 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_rgb64_sfloat,
    ia_format_rgb64_sint,
    ia_format_rgb64_uint,
    /** 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
    ia_format_rgba64_sfloat,
    ia_format_rgba64_sint,
    ia_format_rgba64_uint,
    /** compressed texture formats */
    ia_format_bc1,          /**< DXT1 r5g6b5a1 */
    ia_format_bc2,          /**< DXT3 r5g6b6a4 */
    ia_format_bc3,          /**< DXT5 r5g6b5a8 */
    ia_format_bc4,          /**< LATC1/ATI1 r8 */
    ia_format_bc5,          /**< LATC2/ATI2 r8g8 */
    ia_format_bc6h,         /**< BC6H rgb16_sfloat */
    ia_format_bc7,          /**< BC7 rgb 4-7 bits per color channel */
    ia_format_etc1,         /**< ETC1 rgb8 */
    ia_format_etc2,         /**< ETC2 rgb8 */
    ia_format_etc2a,        /**< ETC2 rgba8 */
    ia_format_etc2a1,       /**< ETC2 rgb8a1 */
    ia_format_ptc12,        /**< PVRTC1 rgb 2BPP */
    ia_format_ptc14,        /**< PVRTC1 rgb 4BPP */
    ia_format_ptc22,        /**< PVRTC2 rgba 2BPP */
    ia_format_ptc24,        /**< PVRTC2 rgba 4BPP */
    ia_format_astc4x4,      /**< ASTC 4x4 8.0 BPP */
    ia_format_astc5x4,      /**< ASTC 5x4 6.40 BPP */
    ia_format_astc5x5,      /**< ASTC 5x5 5.12 BPP */
    ia_format_astc6x5,      /**< ASTC 6x5 4.27 BPP */
    ia_format_astc6x6,      /**< ASTC 6x6 3.56 BPP */
    ia_format_astc8x5,      /**< ASTC 8x5 3.20 BPP */
    ia_format_astc8x6,      /**< ASTC 8x6 2.67 BPP */
    ia_format_astc8x8,      /**< ASTC 8x8 2.0 BPP */
    ia_format_astc10x5,     /**< ASTC 10x5 2.56 BPP */
    ia_format_astc10x6,     /**< ASTC 10x6 2.13 BPP */
    ia_format_astc10x8,     /**< ASTC 10x8 1.60 BPP */
    ia_format_astc10x10,    /**< ASTC 10x10 1.28 BPP */
    ia_format_astc12x10,    /**< ASTC 12x10 1.07 BPP */
    ia_format_astc12x12,    /**< ASTC 12x12 0.89 BPP */
    /* other */
    ia_format_count,
} ia_format;

typedef u8 ia_gpu_memory_property; 
typedef enum ia_gpu_memory_property_bits : ia_gpu_memory_property {
    ia_gpu_memory_property_none                     = 0,
    ia_gpu_memory_property_device_local             = (1u << 0), 
    ia_gpu_memory_property_device_coherent          = (1u << 1), 
    ia_gpu_memory_property_device_uncached          = (1u << 2), 
    ia_gpu_memory_property_host_visible             = (1u << 3), 
    ia_gpu_memory_property_host_coherent            = (1u << 4), 
    ia_gpu_memory_property_host_cached              = (1u << 5), 
    ia_gpu_memory_property_lazily_allocated         = (1u << 6), 
    ia_gpu_memory_property_protected                = (1u << 7), 
} ia_gpu_memory_property_bits;

typedef u8 ia_gpu_memory_flags;
typedef enum ia_gpu_memory_flag_bits : ia_gpu_memory_flags {
    ia_gpu_memory_flag_none                         = 0,
    ia_gpu_memory_flag_dedicated_memory             = (1u << 0),
    ia_gpu_memory_flag_can_alias                    = (1u << 1),
    ia_gpu_memory_flag_host_access_sequential_write = (1u << 2),
    ia_gpu_memory_flag_host_access_random           = (1u << 3),
    ia_gpu_memory_flag_strategy_min_memory          = (1u << 4),
    ia_gpu_memory_flag_strategy_min_time            = (1u << 5),
} ia_gpu_memory_flag_bits;

typedef struct ia_gpu_memory_requirements {
    u64 size;
    u64 alignment;
} ia_gpu_memory_requirements;

typedef u8 ia_render_buffer_usage;
typedef enum ia_render_buffer_usage_bits : ia_render_buffer_usage {
    ia_render_buffer_usage_none = 0,
    ia_render_buffer_usage_vertex           = (1u << 0),
    ia_render_buffer_usage_index            = (1u << 1),
    ia_render_buffer_usage_uniform          = (1u << 2),
    ia_render_buffer_usage_storage          = (1u << 3),
    ia_render_buffer_usage_device_address   = (1u << 4),
    ia_render_buffer_usage_transfer_src     = (1u << 5),
    ia_render_buffer_usage_transfer_dst     = (1u << 6),
} ia_render_buffer_usage_bits;

typedef struct ia_render_buffer_info {
    u64                     size;
    ia_render_buffer_usage  usage;
    ia_gpu_memory_flags     memory_flags;
    char const             *name;
} ia_render_buffer_info;

static constexpr ia_render_buffer_info ia_render_buffer_info_init = {
    .size = 0llu,
    .usage = ia_render_buffer_usage_none,
    .memory_flags = ia_gpu_memory_flag_none,
    .name = nullptr,
};

typedef ia_flags ia_render_texture_flags;
typedef enum ia_render_texture_flag_bits : ia_render_texture_flags {
    ia_render_texture_flag_none                 = 0u,
    ia_render_texture_flag_allow_mutable_format = (1u << 0),
    ia_render_texture_flag_compatible_cube      = (1u << 1),
    ia_render_texture_flag_compatible_2d_array  = (1u << 2),
    ia_render_texture_flag_allow_aliasing       = (1u << 3),
} ia_render_texture_flag_bits;

typedef u16 ia_render_texture_aspect;
typedef enum ia_render_texture_aspect_bits : ia_render_texture_aspect {
    ia_render_texture_aspect_none               = (1u << 0),
    ia_render_texture_aspect_color              = (1u << 1),
    ia_render_texture_aspect_depth              = (1u << 2),
    ia_render_texture_aspect_stencil            = (1u << 3),
    ia_render_texture_aspect_metadata           = (1u << 4),
    ia_render_texture_aspect_plane_0            = (1u << 5),
    ia_render_texture_aspect_plane_1            = (1u << 6),
    ia_render_texture_aspect_plane_2            = (1u << 7),
    ia_render_texture_aspect_memory_plane_0     = (1u << 8),
    ia_render_texture_aspect_memory_plane_1     = (1u << 9),
    ia_render_texture_aspect_memory_plane_2     = (1u << 10),
    ia_render_texture_aspect_memory_plane_3     = (1u << 11),
} ia_render_texture_aspect_bits;

typedef u16 ia_render_texture_usage;
typedef enum ia_render_texture_usage_bits : ia_render_texture_usage {
    ia_render_texture_usage_none                                = 0,
    ia_render_texture_usage_transfer_src                        = (1u << 0),
    ia_render_texture_usage_transfer_dst                        = (1u << 1),
    ia_render_texture_usage_shader_sampled                      = (1u << 2),
    ia_render_texture_usage_shader_storage                      = (1u << 3),
    ia_render_texture_usage_color_attachment                    = (1u << 4),
    ia_render_texture_usage_depth_stencil_attachment            = (1u << 5),
    ia_render_texture_usage_transient_attachment                = (1u << 6),
    ia_render_texture_usage_fragment_density_map                = (1u << 7),
    ia_render_texture_usage_fragment_shading_rate_attachment    = (1u << 8),
} ia_render_texture_usage_bits;

typedef u8 ia_render_resolve_mode;
typedef enum ia_render_resolve_mode_bits : ia_render_resolve_mode {
    ia_render_resolve_mode_none                         = 0,
    ia_render_resolve_mode_sample_zero                  = (1u << 0),
    ia_render_resolve_mode_average                      = (1u << 1),
    ia_render_resolve_mode_min                          = (1u << 2),
    ia_render_resolve_mode_max                          = (1u << 3),
    ia_render_resolve_mode_external_downsample_android  = (1u << 4),
} ia_render_resolve_mode_bits;

typedef struct ia_render_texture_mip_array_slice {
    u32 base_mip_level;
    u32 level_count;
    u32 base_array_layer;
    u32 layer_count;
} ia_render_texture_mip_array_slice;

typedef struct ia_render_texture_array_slice {
    u32 mip_level;
    u32 base_array_layer;
    u32 layer_count;
} ia_render_texture_array_slice;

typedef struct ia_render_texture_slice {
    u32 mip_level;
    u32 array_layer;
} ia_render_texture_slice;

typedef struct ia_render_texture_info {
    ia_render_texture_usage             usage;
    ia_format                           format;
    ia_render_texture_flags             flags;
    ia_gpu_memory_flags                 memory_flags;
    ia_extent3d                         extent;
    u32                                 mip_level_count;
    u32                                 array_layer_count;
    u8                                  dimensions;
    ia_sharing_mode                     sharing_mode;
    ia_sample_count                     sample_count;
    char const                         *name;
} ia_render_texture_info;

static constexpr ia_render_texture_info ia_render_texture_info_init = {
    .usage = ia_render_texture_usage_none,
    .format = ia_format_rgba8_srgb,
    .flags = ia_render_texture_flag_none,
    .memory_flags = ia_gpu_memory_flag_none,
    .extent = {},
    .mip_level_count = 1,
    .array_layer_count = 1,
    .dimensions = 2,
    .sharing_mode = ia_sharing_mode_concurrent,
    .sample_count = ia_sample_count_1,
    .name = nullptr,
};

typedef enum ia_render_texture_view_type : i8 {
    ia_render_texture_view_type_1d = 0,
    ia_render_texture_view_type_2d,
    ia_render_texture_view_type_3d,
    ia_render_texture_view_type_cube,
    ia_render_texture_view_type_1d_array,
    ia_render_texture_view_type_2d_array,
    ia_render_texture_view_type_cube_array,
} ia_render_texture_view_type;

typedef struct ia_render_texture_view_info {
    ia_render_texture                   texture;
    ia_render_texture_view_type         view_type;
    ia_render_texture_aspect            aspect;
    ia_format                           format;
    ia_render_texture_mip_array_slice   slice;
    char const                         *name;
} ia_render_texture_view_info;

static constexpr ia_render_texture_view_info ia_render_texture_view_info_init = {
    .texture = {},
    .view_type = ia_render_texture_view_type_2d,
    .aspect = ia_render_texture_aspect_none,
    .format = ia_format_rgba8_srgb,
    .slice = {},
    .name = nullptr,
};

typedef struct ia_render_texture_sampled {
    ia_render_texture_view  texture_view;
    ia_render_sampler       sampler;
} ia_render_texture_sampled;

typedef enum ia_render_sampler_border_color : i8 {
    ia_render_sampler_border_color_float_transparent_black = 0,
    ia_render_sampler_border_color_int_transparent_black,
    ia_render_sampler_border_color_float_opaque_black,
    ia_render_sampler_border_color_int_opaque_black,
    ia_render_sampler_border_color_float_opaque_white,
    ia_render_sampler_border_color_int_opaque_white,
} ia_render_sampler_border_color;

typedef enum ia_render_sampler_address_mode : i8 {
    ia_render_sampler_address_mode_wrap = 0,
    ia_render_sampler_address_mode_mirror,
    ia_render_sampler_address_mode_clamp_edge,
    ia_render_sampler_address_mode_clamp_border,
} ia_render_sampler_address_mode;

typedef enum ia_render_sampler_reduction_mode : i8 {
    ia_render_sampler_reduction_mode_weighted_average = 0,
    ia_render_sampler_reduction_mode_min,
    ia_render_sampler_reduction_mode_max,
} ia_render_sampler_reduction_mode;

typedef struct ia_render_sampler_info {
    ia_filter_mode                      mag_filter;
    ia_filter_mode                      min_filter;
    ia_filter_mode                      mipmap_filter;
    ia_render_sampler_reduction_mode    reduction_mode;
    ia_render_sampler_address_mode      address_mode_u;
    ia_render_sampler_address_mode      address_mode_v;
    ia_render_sampler_address_mode      address_mode_w;
    ia_render_sampler_border_color      border_color;
    ia_compare_op                       compare_op;
    bool                                enable_compare_op;
    bool                                enable_anisotrophy;
    bool                                enable_unnormalized_coords;
    f32                                 mip_lod_bias;
    f32                                 min_lod;
    f32                                 max_lod;
    f32                                 max_anisotrophy;
    char const                         *name;
} ia_render_sampler_info;

static constexpr ia_render_sampler_info ia_render_sampler_info_init = {
    .mag_filter = ia_filter_mode_linear,
    .min_filter = ia_filter_mode_linear,
    .mipmap_filter = ia_filter_mode_linear,
    .reduction_mode = ia_render_sampler_reduction_mode_weighted_average,
    .address_mode_u = ia_render_sampler_address_mode_clamp_edge,
    .address_mode_v = ia_render_sampler_address_mode_clamp_edge,
    .address_mode_w = ia_render_sampler_address_mode_clamp_edge,
    .border_color = ia_render_sampler_border_color_int_transparent_black,
    .compare_op = ia_compare_op_always,
    .enable_compare_op = false,
    .enable_anisotrophy = false,
    .enable_unnormalized_coords = false,
    .mip_lod_bias = 0.5f,
    .min_lod = 0.0f,
    .max_lod = 1000.0f,
    .max_anisotrophy = 0.0f,
    .name = nullptr,
};

typedef struct ia_shader_binding_table {
    ia_strided_address_region   raygen_region;
    ia_strided_address_region   miss_region;
    ia_strided_address_region   hit_region;
    ia_strided_address_region   callable_region;
} ia_shader_binding_table;

typedef struct ia_render_bvh_info {
    u32 TODO;
} ia_render_bvh_info;

/* TODO BVH aabb geometry & instances, swapchain, shaders, pipelines, query pools, synchronization model */

typedef ia_bitmask ia_shader_stage_mask;
typedef enum ia_shader_stage_bits : ia_shader_stage_mask {
    ia_shader_stage_none                    = 0u,
    ia_shader_stage_vertex                  = (1u << 0),
    ia_shader_stage_hull                    = (1u << 1),
    ia_shader_stage_domain                  = (1u << 2),
    ia_shader_stage_mesh                    = (1u << 3),
    ia_shader_stage_geometry                = (1u << 4),
    ia_shader_stage_fragment                = (1u << 5),
    ia_shader_stage_compute                 = (1u << 6),
    ia_shader_stage_node                    = (1u << 7),
    ia_shader_stage_amplification           = (1u << 8),
    ia_shader_stage_raygeneration           = (1u << 9),
    ia_shader_stage_miss                    = (1u << 10),
    ia_shader_stage_anyhit                  = (1u << 11),
    ia_shader_stage_closesthit              = (1u << 12),
    ia_shader_stage_intersection            = (1u << 13),
    ia_shader_stage_callable                = (1u << 14),
    ia_shader_stage_all                     = UINT32_MAX,
} ia_shader_stage_bits;

typedef enum ia_render_device_type : i32 {
    ia_render_device_type_other = 0,
    ia_render_device_type_integrated_gpu,
    ia_render_device_type_discrete_gpu,
    ia_render_device_type_virtual_gpu,
    ia_render_device_type_cpu,
} ia_render_device_type;

typedef struct ia_render_device_ray_tracing_capabilities {
    u64 max_geometry_count;
    u64 max_instance_count;
    u64 max_primitive_count;
    u32 min_bvh_scratch_offset_alignment;
    u32 shader_group_handle_size;
    u32 shader_group_handle_capture_replay_size;
    u32 shader_group_handle_alignment;
    u32 shader_group_alignment;
    u32 max_shader_group_stride;
    u32 max_ray_recursion_depth;
    u32 max_ray_dispatch_invocation_count;
    u32 max_ray_hit_attribute_size;
    u32 invocation_reorder_hint;
} ia_render_device_ray_tracing_capabilities;

typedef struct ia_render_device_mesh_shader_capabilities {
    u32 max_vertice_count;
    u32 max_primitive_count;
    u32 max_output_component_count;
    u32 max_task_invocations;
    u32 max_task_shared_memory;
    u32 max_task_output_memory;
    u32 max_task_payload_size;
    u32 max_task_work_group_count;
    u32 max_mesh_invocations;
    u32 max_mesh_shared_memory;
    u32 max_mesh_output_memory;
    u32 max_mesh_payload_size;
    u32 max_mesh_work_group_count;
    u32 max_preferred_task_work_group_invocations;
    u32 max_preferred_mesh_work_group_invocations;
    bool prefers_local_invocation_vertex_output;
    bool prefers_local_invocation_primitive_output;
    bool prefers_compact_vertex_output;
    bool prefers_compact_primitive_output;
} ia_render_device_mesh_shader_capabilities;

typedef struct ia_render_device_work_graph_capabilities {
    u32 dispatch_address_alignment;
    u32 max_depth;
    u32 max_shader_output_nodes;
    u32 max_shader_payload_size;
    u32 max_shader_payload_count;
    u32 max_workgroup_count[3];
    u32 max_workgroups;
} ia_render_device_work_graph_capabilities;

typedef struct ia_render_device_limits {
    u32 max_2d_texture_size;
    u32 max_3d_texture_size;
    u32 max_cube_texture_size;
    u32 max_array_layer_count;
    u32 max_color_attachment_count;
    u32 max_viewport_count;
    u32 max_root_constants_size;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_vertex_attribute_count;
    u32 max_vertex_binding_count;
    u32 max_compute_shared_memory;
    u32 max_compute_invocation_count;
    u32 max_draw_indirect_count;
    u64 min_uniform_buffer_alignment;
    u64 min_storage_buffer_alignment;
    u64 min_texel_buffer_alignment;
} ia_render_device_limits;

typedef u64 ia_render_device_features;
typedef enum ia_render_device_feature_bits : ia_render_device_features {
    ia_render_device_feature_none                                   = 0llu,
    ia_render_device_feature_descriptor_indexing                    = (1llu << 0),
    ia_render_device_feature_buffer_device_address                  = (1llu << 1),
    ia_render_device_feature_buffer_device_address_capture_replay   = (1llu << 2),
    ia_render_device_feature_framebuffer_dynamic_local_dependencies = (1llu << 3),
    ia_render_device_feature_tesselation_shader                     = (1llu << 4),
    ia_render_device_feature_geometry_shader                        = (1llu << 5),
    ia_render_device_feature_mesh_shader                            = (1llu << 6),
    ia_render_device_feature_root_constants                         = (1llu << 7),
    ia_render_device_feature_basic_ray_tracing                      = (1llu << 8),
    ia_render_device_feature_basic_ray_tracing_capture_replay       = (1llu << 9),
    ia_render_device_feature_ray_tracing_pipeline                   = (1llu << 10),
    ia_render_device_feature_ray_tracing_invocation_reorder         = (1llu << 11),
    ia_render_device_feature_ray_tracing_position_fetch             = (1llu << 12),
    ia_render_device_feature_conservative_rasterization             = (1llu << 13),
    ia_render_device_feature_work_graph                             = (1llu << 14),
    ia_render_device_feature_depth_clamp                            = (1llu << 15),
    ia_render_device_feature_robust_access                          = (1llu << 16),
    ia_render_device_feature_dynamic_states                         = (1llu << 17),
    ia_render_device_feature_sampler_anisotrophy                    = (1llu << 18),
    ia_render_device_feature_fragment_stores_and_atomics            = (1llu << 19),
    ia_render_device_feature_timeline_semaphore                     = (1llu << 20),
    ia_render_device_feature_texture_cube_array                     = (1llu << 21),
    ia_render_device_feature_texture_atomic64                       = (1llu << 22),
    ia_render_device_feature_shader_atomic_float                    = (1llu << 23),
    ia_render_device_feature_shader_atomic_int64                    = (1llu << 24),
    ia_render_device_feature_shader_storage_texture                 = (1llu << 25),
    ia_render_device_feature_shader_float16                         = (1llu << 26),
    ia_render_device_feature_shader_int64                           = (1llu << 27),
    ia_render_device_feature_shader_int16                           = (1llu << 28),
    ia_render_device_feature_shader_objects                         = (1llu << 29),
    ia_render_device_feature_swapchain                              = (1llu << 30),
    ia_render_device_feature_sparse_binding                         = (1llu << 31),
    ia_render_device_feature_video_decode_queue                     = (1llu << 32),
    ia_render_device_feature_video_encode_queue                     = (1llu << 33),
    ia_render_device_feature_multi_draw_indirect                    = (1llu << 34),
    ia_render_device_feature_fill_mode_wireframe                    = (1llu << 35),
    ia_render_device_feature_resolve_host_query_data                = (1llu << 36),
    ia_render_device_feature_subgroup_size_control                  = (1llu << 37),
    ia_render_device_feature_scalar_block_layout                    = (1llu << 38),
    ia_render_device_feature_independent_blend                      = (1llu << 39),
    ia_render_device_feature_multiview_xr                           = (1llu << 40),
    ia_render_device_feature_variable_pointers                      = (1llu << 41),
    ia_render_device_feature_vulkan_memory_model                    = (1llu << 42),
    ia_render_device_feature_debug_utils                            = (1llu << 43),
} ia_render_device_feature_bits;

/** Hardware capabilities of a device specified by the backend driver. */
typedef struct ia_render_device_capabilities {
    ia_render_device_type                       type;
    /** Internal index to reference the physical device, not related to logical devices. */
    i32                                         index;
    u32                                         total_score;
    u16                                         vendor_id, device_id;
    i32                                         compute_queue_count;
    i32                                         transfer_queue_count;
    ia_render_device_features                   features;
    ia_render_device_limits                     limits;
    ia_render_device_ray_tracing_capabilities   ray_tracing;
    ia_render_device_mesh_shader_capabilities   mesh_shader;
    ia_render_device_work_graph_capabilities    work_graph;
    char                                        name[256];
} ia_render_device_capabilities;

/** Additional control over the rendering logic is application-specific. The render graph 
 *  may be used to tightly control the rendering, and enforce strategies listed below, 
 *  with some additional ones like alternating GPUs between frames. */
typedef enum ia_render_mgpu_strategy : i32 {
    /** Allow the context initialization process to figure out what strategy is best. */
    ia_render_mgpu_strategy_auto = 0,
    /** Rendering is done on a single primary device, no mGPU-related scheduling or transfer overhead.
     *  It's the default strategy if any other strategies were deemed unoptimal. Most systems that 
     *  only have a single GPU in their system will be using this strategy. */
    ia_render_mgpu_strategy_primary_optimal,
    /** Rendering on the discrete GPU is supported by the less-powerful integrated GPU.
     *  The rendering work is distributed such that the integrated GPU nor work transfers 
     *  must not be a bottleneck. Only secondary-priority work is distributed to the iGPU. */
    ia_render_mgpu_strategy_assymetric,
    /** Multi-device rendering, explicit primary device and multiple secondary devices. 
     *  Sort first redistributes primitives early in the graphics pipeline, during geometry 
     *  processing. This is achieved by dividing the screenspace such that each GPU renders 
     *  it's own region, and the final image is composed from the contributing regions. */
    ia_render_mgpu_strategy_sort_first,
    /** Multi-device rendering, explicit primary device and multiple secondary devices.
     *  Sort last is defined as deferring primitive redistribution until the end of the graphics 
     *  pipeline. It divides primitives such that each GPU renders it's own portion of mesh data 
     *  into the framebuffer. Then these are composed into the final image. */
    ia_render_mgpu_strategy_sort_last,
} ia_render_mgpu_strategy;

/** Acceptable values for frames-in-flight. TODO docs */
typedef enum ia_render_frames_in_flight : i32 {
    ia_render_frames_in_flight_2 = 2,
    ia_render_frames_in_flight_3 = 3,
    ia_render_frames_in_flight_4 = 4,
} ia_render_frames_in_flight;

/** Used to create a rendering context. */
typedef struct ia_render_context_info {
    /** Create this many devices for this rendering context, if 0 pick all valid physical devices (ignore CPU). */
    i32                                         device_count;
    /** A list of indices to physical devices. Creating virtual devices (multiple logical 
     *  devices mapped to one physical device) is not allowed and asserted in NDEBUG builds. */
    i32                                         device_indices[IA_RENDER_MAX_DEVICES];
    /** By default, rendering work will be done using this strategy.
     *  This may explicitly change on a frame-to-frame basis if the user desires. */
    ia_render_mgpu_strategy                     mgpu_strategy;
    /** Controls buffering of per-frame render targets, sets a minimum image count for swapchains. */
    ia_render_frames_in_flight                  frames_in_flight;
    /** Set's a target size for the resource registry. These values are required to be powers of 2, 
     *  for convenience. A device will be discarded if it doesn't meet these limits at minimum. */
    u8                                          log2_max_allowed_buffers;
    u8                                          log2_max_allowed_textures;
    u8                                          log2_max_allowed_texture_views;
    u8                                          log2_max_allowed_samplers;
    u8                                          log2_max_allowed_bvhs;
    /** Require every device to support these features, or fail if any of these are missing. 
     *  Will be OR'ed together with the features required by the backend. */
    ia_render_device_features                   required_features;
    /** These features will be enabled if possible, it's acceptable for any of them to be missing. */
    ia_render_device_features                   optional_features;
    /** For every device, an error status is written whenever creating the context failed.
     *  Function for creating the context returns success only if all per-device work succeeded. */
    ia_result                                   results[IA_RENDER_MAX_DEVICES];
} ia_render_context_info;

static constexpr ia_render_context_info ia_render_context_info_init = {
    .device_count = 1,
    .device_indices = {0},
    .mgpu_strategy = ia_render_mgpu_strategy_auto,
    .frames_in_flight = ia_render_frames_in_flight_2,
    .required_features = ia_render_device_feature_none,
    .optional_features = ia_render_device_feature_none,
    .log2_max_allowed_buffers = 13, // (1u << 13) == 8192
    .log2_max_allowed_textures = 13, // (1u << 13) == 8192
    .log2_max_allowed_texture_views = 13, // (1u << 13) == 8192
    .log2_max_allowed_samplers = 9, // (1u << 9) == 512
    .log2_max_allowed_bvhs = 13, // (1u << 13) == 8192; set to 0 if basic_ray_tracing disabled
    .results = {},
};

/** Controls how rendering is done in the context of a device group. */
typedef struct ia_render_context {
    ia_render_library                           library;
    /** A mask of all logical device indices, created from `(ia_render_device_mask)((1llu << device_count) - 1)`. */
    ia_render_device_mask                       device_mask;
    i32                                         device_count; /**< (device_mask + 1) == popcnt(device_mask) */
    /** Rendering work will be preferrably done using this strategy.
     *  This may explicitly change on a frame-to-frame basis if the user so desires. */
    ia_render_mgpu_strategy                     mgpu_strategy;
    /** Controls buffering of per-frame render targets, sets a minimum image count for swapchains. */
    ia_render_frames_in_flight                  frames_in_flight;
    /** All devices in this context support these features, computed from backend- 
     *  and user-specified required features and features optionally enabled. */
    ia_render_device_features                   enabled_features_lowest_common;
    /** Exact per-device enabled features, at minimum it's `required_features`,
     *  at maximum it's `capabilities[device].features`. [device_count] */
    ia_render_device_features                   enabled_features[IA_RENDER_MAX_DEVICES];
    /** Capabilities are for physical devices, this array may be accessed through logical 
     *  device index to get their underlying physical device. [device_count] */
    ia_render_device_capabilities const        *capabilities[IA_RENDER_MAX_DEVICES];
} ia_render_context;

/** Acquire capabilities of physical devices available through this adapter.
 *  The `out_device_capabilities` array is backend-allocated through the drifter. */
typedef ia_result (IA_CALL *ia_render_enumerate_device_capabilities_fn)(
    ia_render_adapter              *adapter,
    i32                            *out_device_count,
    ia_render_device_capabilities **out_device_capabilities);
#define IA_RENDER_ENUMERATE_DEVICE_CAPABILITIES_FN(X) \
    ia_result IA_CALL _ia_##X##_render_enumerate_device_capabilities( \
        ia_render_adapter              *adapter, \
        i32                            *out_device_count, \
        ia_render_device_capabilities **out_device_capabilities)

/** Creates a rendering context from a provided specification: devices, queues, resource registry, allocators. */
typedef ia_result (IA_CALL *ia_render_create_context_fn)(
    ia_render_adapter              *adapter,
    ia_render_context_info const   *info,
    ia_render_context             **out_context);
#define IA_RENDER_CREATE_CONTEXT_FN(X) \
    ia_result IA_CALL _ia_##X##_render_create_context( \
        ia_render_adapter              *adapter, \
        ia_render_context_info const   *info, \
        ia_render_context             **out_context)

/** Destroys a rendering context, it has the adapter information saved already. */
IA_WORK_FN_TYPEDEF(ia_render_destroy_context_fn, ia_render_context *context);
#define IA_RENDER_DESTROY_CONTEXT_FN(X) \
    IA_WORK_FN(_ia_##X##_render_destroy_context, ia_render_context *context)

/** FIXME `out_compiled_commands` is one handle for now... TODO docs */
typedef ia_result (IA_CALL *ia_render_compile_command_streams_fn)(
    ia_render_context                      *context,
    i32                                     command_stream_count,
    struct ia_render_command_stream const  *command_streams, // [command_stream_count]
    ia_compiled_commands                   *out_compiled_commands);
#define IA_RENDER_COMPILE_COMMAND_STREAMS_FN(X) \
    ia_result IA_CALL _ia_##X##_render_compile_command_streams( \
        ia_render_context                      *context, \
        i32                                     command_stream_count, \
        struct ia_render_command_stream const  *command_streams, \
        ia_compiled_commands                   *out_compiled_commands)

/** Returns the host address of a mapped buffer handle loaded in a given device mask.
 *  The size of the `out_host_addresses` array must be `popcnt(device_mask)`.
 *  If any device within the device_mask has not this buffer loaded, it's corresponding 
 *  host address is written as NULL, and an error status will be returned, 
 *  other returned addresses will be valid if the resource is. */
typedef ia_result (IA_CALL *ia_render_buffer_host_addresses_fn)(
    ia_render_context      *context,
    ia_render_buffer        buffer,
    ia_render_device_mask   device_mask,
    uptr                   *out_host_addresses);
#define IA_RENDER_BUFFER_HOST_ADDRESSES_FN(X) \
    ia_result IA_CALL _ia_##X##_render_buffer_host_addresses( \
        ia_render_context      *context, \
        ia_render_buffer        buffer, \
        ia_render_device_mask   device_mask, \
        uptr                   *out_host_addresses)

/** Returns the device addresses of a buffer handle loaded in a given device mask.
 *  The size of the `out_device_addresses` array must be `popcnt(device_mask)`.
 *  If any device within the device_mask has not this buffer loaded, it's corresponding 
 *  device address is written as 0llu, and an error status will be returned,
 *  other returned addresses will be valid if the resource is. */
typedef ia_result (IA_CALL *ia_render_buffer_device_addresses_fn)(
    ia_render_context          *context,
    ia_render_buffer            buffer,
    ia_render_device_mask       device_mask,
    ia_address                 *out_device_addresses);
#define IA_RENDER_BUFFER_DEVICE_ADDRESSES_FN(X) \
    ia_result IA_CALL _ia_##X##_render_buffer_device_addresses( \
        ia_render_context      *context, \
        ia_render_buffer        buffer, \
        ia_render_device_mask   device_mask, \
        ia_address             *out_device_addresses)

/** Returns the device addresses of a BVH handle loaded in a given device mask.
 *  The size of the `out_device_addresses` array must be `popcnt(device_mask)`.
 *  If any device within the device_mask has not this BVH loaded, it's corresponding 
 *  device address is written as 0llu, and an error status will be returned,
 *  other returned addresses will be valid if the resource is. All addresses 
 *  are invalid for devices that don't support basic_ray_tracing. */
typedef ia_result (IA_CALL *ia_render_bvh_device_addresses_fn)(
    ia_render_context          *context,
    ia_render_bvh               bvh,
    ia_render_device_mask       device_mask,
    ia_address                 *out_device_addresses);
#define IA_RENDER_BVH_DEVICE_ADDRESSES_FN(X) \
    ia_result IA_CALL _ia_##X##_render_bvh_device_addresses( \
        ia_render_context      *context, \
        ia_render_bvh           bvh, \
        ia_render_device_mask   device_mask, \
        ia_address             *out_device_addresses)

/** Creates an array of GPU resources in all devices under the `device_mask`. */
#define _IA_RENDER_CREATE_GPU_RESOURCES_PFN(T) \
    typedef ia_result (IA_CALL *ia_render_create_##T##s_fn)( \
        ia_render_context          *context, \
        ia_render_device_mask       device_mask, \
        i32                         T##_count, \
        ia_render_##T##_info const *T##_infos, \
        ia_render_##T              *out_##T##s)

_IA_RENDER_CREATE_GPU_RESOURCES_PFN(buffer);
_IA_RENDER_CREATE_GPU_RESOURCES_PFN(texture);
_IA_RENDER_CREATE_GPU_RESOURCES_PFN(texture_view);
_IA_RENDER_CREATE_GPU_RESOURCES_PFN(sampler);
_IA_RENDER_CREATE_GPU_RESOURCES_PFN(bvh);

/** Implements a `_IA_RENDER_CREATE_GPU_RESOURCES_PFN` function. */
#define _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, T) \
    ia_result IA_CALL _ia_##X##_render_create_##T##s_fn( \
        ia_render_context          *context, \
        ia_render_device_mask       device_mask, \
        i32                         T##_count, \
        ia_render_##T##_info const *T##_infos, \
        ia_render_##T              *out_##T##s)

#define IA_RENDER_CREATE_BUFFERS_FN         _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, buffer);
#define IA_RENDER_CREATE_TEXTURES_FN        _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, texture);
#define IA_RENDER_CREATE_TEXTURE_VIEWS_FN   _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, texture_view);
#define IA_RENDER_CREATE_SAMPLERS_FN        _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, sampler);
#define IA_RENDER_CREATE_BVHS_FN            _IA_RENDER_CREATE_GPU_RESOURCES_FN(X, bvh);

/** For every valid handle, replicates the GPU resources in all devices under the `device_mask`.
 *  If a resource already exists in a device's memory, this operation is ignored for this device.
 *  In most cases this operation might be expensive and it's completely unnecessary for one GPU setups.
 *  The data transfer will be performed from a given source device, -1 value is acceptable for auto. */
#define _IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(T) \
    typedef ia_result (IA_CALL *ia_render_replicate_##T##s_fn)( \
        ia_render_context      *context, \
        i32                     src_device, \
        ia_render_device_mask   device_mask, \
        i32                     T##_count, \
        ia_render_##T const    *T##s)

_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(buffer);
_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(texture);
_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(texture_view);
_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(sampler);
_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN(bvh);

/** Implements a `_IA_RENDER_REPLICATE_GPU_RESOURCES_PFN` function. */
#define _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, T) \
    ia_result IA_CALL _ia_##X##_render_replicate_##T##s_fn( \
        ia_render_context      *context, \
        i32                     src_device, \
        ia_render_device_mask   device_mask, \
        i32                     T##_count, \
        ia_render_##T const    *T##s)

#define IA_RENDER_REPLICATE_BUFFERS_FN          _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, buffer);
#define IA_RENDER_REPLICATE_TEXTURES_FN         _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, texture);
#define IA_RENDER_REPLICATE_TEXTURE_VIEWS_FN    _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, texture_view);
#define IA_RENDER_REPLICATE_SAMPLERS_FN         _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, sampler);
#define IA_RENDER_REPLICATE_BVHS_FN             _IA_RENDER_REPLICATE_GPU_RESOURCES_FN(X, bvh);

/** Destroys an array of GPU resources instances in all devices under `device_mask`. 
 *  If this completely destroys all instances of a resource, it's slot will be recycled. */
#define _IA_RENDER_DESTROY_GPU_RESOURCES_PFN(T) \
    typedef void (IA_CALL *ia_render_destroy_##T##s_fn)( \
        ia_render_context      *context, \
        ia_render_device_mask   device_mask, \
        i32                     T##_count, \
        ia_render_##T const    *T##s)

_IA_RENDER_DESTROY_GPU_RESOURCES_PFN(buffer);
_IA_RENDER_DESTROY_GPU_RESOURCES_PFN(texture);
_IA_RENDER_DESTROY_GPU_RESOURCES_PFN(texture_view);
_IA_RENDER_DESTROY_GPU_RESOURCES_PFN(sampler);
_IA_RENDER_DESTROY_GPU_RESOURCES_PFN(bvh);

/** Implements a `_IA_RENDER_DESTROY_GPU_RESOURCES_PFN` function. */
#define _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, T) \
    void IA_CALL _ia_##X##_render_destroy_##T##s_fn( \
        ia_render_context      *context, \
        ia_render_device_mask   device_mask, \
        i32                     T##_count, \
        ia_render_##T const    *T##s)

#define IA_RENDER_DESTROY_BUFFERS_FN        _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, buffer);
#define IA_RENDER_DESTROY_TEXTURES_FN       _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, texture);
#define IA_RENDER_DESTROY_TEXTURE_VIEWS_FN  _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, texture_view);
#define IA_RENDER_DESTROY_SAMPLERS_FN       _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, sampler);
#define IA_RENDER_DESTROY_BVHS_FN           _IA_RENDER_DESTROY_GPU_RESOURCES_FN(X, bvh);

struct ia_render_interface {
    ia_interface_header                         header;
    ia_render_enumerate_device_capabilities_fn  enumerate_device_capabilities;
    ia_render_create_context_fn                 create_context;
    ia_render_destroy_context_fn                destroy_context;
    ia_render_compile_command_streams_fn        compile_command_streams;
    ia_render_buffer_host_addresses_fn          buffer_host_addresses;
    ia_render_buffer_device_addresses_fn        buffer_device_addresses;
    ia_render_bvh_device_addresses_fn           bvh_device_addresses;

    ia_render_create_buffers_fn                 create_buffers;
    ia_render_create_textures_fn                create_textures;
    ia_render_create_texture_views_fn           create_texture_views;
    ia_render_create_samplers_fn                create_samplers;
    ia_render_create_bvhs_fn                    create_bvhs;

    ia_render_replicate_buffers_fn              replicate_buffers;
    ia_render_replicate_textures_fn             replicate_textures;
    ia_render_replicate_texture_views_fn        replicate_texture_views;
    ia_render_replicate_samplers_fn             replicate_samplers;
    ia_render_replicate_bvhs_fn                 replicate_bvhs;

    ia_render_destroy_buffers_fn                destroy_buffers;
    ia_render_destroy_textures_fn               destroy_textures;
    ia_render_destroy_texture_views_fn          destroy_texture_views;
    ia_render_destroy_samplers_fn               destroy_samplers;
    ia_render_destroy_bvhs_fn                   destroy_bvhs;
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
