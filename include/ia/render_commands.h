#pragma once
/** @file ia/render_commands.h
 *  @brief TODO docs
 *
 *  TODO docs
 */
#include <ia/foundation.h>
#include <ia/render.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Resource access, used for the internal synchronization model. */
typedef enum ia_render_access : i8 {
    ia_render_access_none = 0,
    ia_render_access_shader_uniform_read,
    ia_render_access_shader_sampled_read,
    ia_render_access_shader_storage_read,
    ia_render_access_shader_storage_write,
    ia_render_access_color_attachment_read,
    ia_render_access_color_attachment_write,
    ia_render_access_depth_stencil_read,
    ia_render_access_depth_stencil_write,
    ia_render_access_transfer_read,
    ia_render_access_transfer_write,
    ia_render_access_host_read,
    ia_render_access_host_write,
    ia_render_access_present,
} ia_render_access;

/** Types of high-level render commands. A command stream allows the application to 
 *  express how a scene should be updated and rendered in a stateless way, but allowing
 *  each rendering backend to control how this is done. This includes render state 
 *  changes, resource barriers and transitions. */
typedef enum ia_render_command_type : i8 {
    ia_render_command_type_none = 0,
    // TODO SHADER OBJECTS
    // ia_render_command_type_bind_shaders,
    ia_render_command_type_bind_shader_pipeline,
    // TODO BVH
    // ia_render_command_type_build_bvhs,
    ia_render_command_type_fill_buffer,
    ia_render_command_type_copy_buffer,
    ia_render_command_type_copy_buffer_to_texture,
    ia_render_command_type_copy_texture,
    ia_render_command_type_copy_texture_to_buffer,
    ia_render_command_type_blit_texture,
    ia_render_command_type_clear_texture,
    ia_render_command_type_resolve_texture,
    ia_render_command_type_destroy_buffer_deferred,
    ia_render_command_type_destroy_texture_deferred,
    ia_render_command_type_destroy_texture_view_deferred,
    ia_render_command_type_destroy_sampler_deferred,
    ia_render_command_type_discard_buffer,
    ia_render_command_type_discard_texture,
    ia_render_command_type_root_constants,
    // TODO EVENTS
    // ia_render_command_type_signal_event,
    // ia_render_command_type_wait_events,
    // ia_render_command_type_reset_event,
    // ia_render_command_type_barriers_and_transitions,
    ia_render_command_type_write_timestamps,
    ia_render_command_type_resolve_timestamps,
    ia_render_command_type_begin_label,
    ia_render_command_type_end_label,
    ia_render_command_type_begin_renderpass,
    ia_render_command_type_end_renderpass,
    ia_render_command_type_draw,
    ia_render_command_type_draw_indexed,
    ia_render_command_type_draw_indirect,
    ia_render_command_type_draw_indirect_count,
    ia_render_command_type_draw_indexed_indirect,
    ia_render_command_type_draw_indexed_indirect_count,
    ia_render_command_type_draw_mesh_tasks,
    ia_render_command_type_draw_mesh_tasks_indirect,
    ia_render_command_type_draw_mesh_tasks_indirect_count,
    ia_render_command_type_dispatch,
    ia_render_command_type_dispatch_indirect,
    // TODO WORK GRAPH
    // ia_render_command_type_dispatch_graph,
    // ia_render_command_type_dispatch_graph_indirect,
    // ia_render_command_type_dispatch_graph_indirect_count,
    ia_render_command_type_trace_rays,
    ia_render_command_type_trace_rays_indirect,
} ia_render_command_type;

/** Encodes a stream of high-level render commands. */
typedef struct ia_render_command_stream {
    ia_darray               da;
    ia_render_device_mask   device_mask;    /**< A mask of devices that are expected to run this work. */
    ia_render_queue_mask    queue_mask;     /**< A mask of encountered queue types, for best scheduling. */
} ia_render_command_stream;

/** Core of the command stream encoder, uses the drift allocator. */
IA_HOT_FN IA_API void *IA_CALL 
ia_render_cmd_alloc(
    ia_render_command_stream   *stream,
    i32                         size,
    i32                         align);

/** A header encoding of a high-level render command. */
typedef struct IA_PACKED ia_render_command_header {
    ia_render_command_type  type;           /**< Encodes the payload. */
    ia_render_queue_type    queue_type;     /**< Preferred command queue. */
    i32                     next;           /**< Size in bytes, used to access the next command in the stream. */
} ia_render_command_header;

/** Define an inline render command function for the command stream. */
#define _IA_DECL_RENDER_COMMAND_INLINE_BODY(T, QUEUE, ARGS, HEADER, PAYLOAD_WRITE, PAYLOAD_SIZE) \
    IA_FORCE_INLINE void ia_render_cmd_##T(ia_render_command_stream *stream ARGS) \
    { \
        i32 encoding_size = ia_ssizeof(ia_render_command_##T) PAYLOAD_SIZE; \
        ia_render_command_##T *cmd = ia_reinterpret_cast(ia_render_command_##T *, \
                ia_render_cmd_alloc(stream, encoding_size, ia_salignof(ia_render_command_##T))); \
        HEADER type = ia_render_command_type_##T; \
        HEADER queue_type = QUEUE; \
        HEADER next = encoding_size; \
        PAYLOAD_WRITE \
        stream->queue_mask |= QUEUE; \
    }

/* Define a render command without payload. */
#define _IA_DECL_RENDER_COMMAND_NO_PAYLOAD(T, QUEUE) \
    typedef ia_render_command_header ia_render_command_##T; \
    _IA_DECL_RENDER_COMMAND_INLINE_BODY(T, QUEUE, IA_MAGIC_NOTHING(), cmd->, IA_MAGIC_NOTHING(), IA_MAGIC_NOTHING())

/* Define a render command with a simple payload. */
#define _IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(T, QUEUE, PAYLOAD, NAME) \
    typedef struct IA_PACKED ia_render_command_##T { \
        ia_render_command_header    header; \
        PAYLOAD                     NAME; \
    } ia_render_command_##T; \
    _IA_DECL_RENDER_COMMAND_INLINE_BODY(T, QUEUE, IA_MAGIC_EVAL1(, PAYLOAD NAME), cmd->header., cmd->NAME = NAME; , IA_MAGIC_NOTHING())

/* Define a render command with the default payload. */
#define _IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(T, QUEUE, ARGS, PAYLOAD_WRITE, PAYLOAD_SIZE) \
    typedef struct IA_PACKED ia_render_command_##T { \
        ia_render_command_header        header; \
        ia_render_command_##T##_payload payload; \
    } ia_render_command_##T; \
    _IA_DECL_RENDER_COMMAND_INLINE_BODY(T, QUEUE, IA_MAGIC_EVAL1(, ARGS), cmd->header., PAYLOAD_WRITE, PAYLOAD_SIZE)

/* TODO SHADER OBJECTS */
#if 0
/** ia_render_command_bind_shaders, ia_render_cmd_bind_shaders() */
typedef struct IA_PACKED ia_render_command_bind_shaders_payload {
    // TODO
} ia_render_command_bind_shaders_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(bind_shaders, ia_render_queue_type_compute, 
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())
#endif /* TODO SHADER OBJECTS */

/** ia_render_command_bind_shader_pipeline, ia_render_cmd_bind_shader_pipeline() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(bind_shader_pipeline, ia_render_queue_type_compute, ia_shader_pipeline, pipeline)

/* TODO BVH */
#if 0
/** ia_render_command_build_bvhs, ia_render_cmd_build_bvhs() */
typedef struct IA_PACKED ia_render_command_bvhs_payload {
    // TODO
} ia_render_command_build_bvhs_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(build_bvhs, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())
#endif /* TODO BVH */

/** ia_render_command_fill_buffer, ia_render_cmd_fill_buffer() */
typedef struct IA_PACKED ia_render_command_fill_buffer_payload {
    ia_render_buffer    dst_buffer;
    u64                 dst_offset;
    u64                 size;
    u32                 data;
} ia_render_command_fill_buffer_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(fill_buffer, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    dst_buffer,
            u64                 dst_offset,
            u64                 size,
            u32                 data),
        { /* write */
            cmd->payload.dst_buffer = dst_buffer;
            cmd->payload.dst_offset = dst_offset;
            cmd->payload.size = size;
            cmd->payload.data = data;
        }, IA_MAGIC_NOTHING())

/** Region for copy_buffer payload. */
typedef struct ia_render_buffer_copy_region {
    u64 src_offset, dst_offset, size;
} ia_render_buffer_copy_region;

/** ia_render_command_copy_buffer, ia_render_cmd_copy_buffer() */
typedef struct IA_PACKED ia_render_command_copy_buffer_payload {
    ia_render_buffer                src_buffer;
    ia_render_buffer                dst_buffer;
    u16                             region_count;
} ia_render_command_copy_buffer_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(copy_buffer, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer                    src_buffer,
            ia_render_buffer                    dst_buffer,
            u16                                 region_count,
            ia_render_buffer_copy_region const *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.src_buffer = src_buffer;
            cmd->payload.dst_buffer = dst_buffer;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_copy_buffer)), regions, sizeof(ia_render_buffer_copy_region) * region_count);
        }, + ia_ssizeof(ia_render_buffer_copy_region) * region_count)

/** Region for copy_texture payload. */
typedef struct ia_render_texture_copy_region {
    ia_render_texture_array_slice   src_slice;
    ia_offset3d                     src_offset;
    ia_render_texture_array_slice   dst_slice;
    ia_offset3d                     dst_offset;
    ia_extent3d                     extent;
} ia_render_texture_copy_region;

/** ia_render_command_copy_texture, ia_render_cmd_copy_texture() */
typedef struct IA_PACKED ia_render_command_copy_texture_payload {
    ia_render_texture               src_texture;
    ia_render_texture               dst_texture;
    ia_render_access                src_access;
    ia_render_access                dst_access;
    u16                             region_count;
} ia_render_command_copy_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(copy_texture, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture                       src_texture,
            ia_render_texture                       dst_texture,
            ia_render_access                        src_access,
            ia_render_access                        dst_access,
            u16                                     region_count,
            ia_render_texture_copy_region const    *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.src_texture = src_texture;
            cmd->payload.dst_texture = dst_texture;
            cmd->payload.src_access = src_access;
            cmd->payload.dst_access = dst_access;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_copy_texture)), regions, sizeof(ia_render_texture_copy_region) * region_count);
        }, + ia_ssizeof(ia_render_texture_copy_region) * region_count)

/** Region for copy_buffer_to_texture and copy_texture_to_buffer payloads. */
typedef struct ia_render_buffer_and_texture_copy_region {
    u64                             buffer_offset;
    u32                             buffer_row_length;
    u32                             buffer_texture_height;
    ia_render_texture_array_slice   texture_slice;
    ia_offset3d                     texture_offset;
    ia_extent3d                     texture_extent;
} ia_render_buffer_and_texture_copy_region;

/** ia_render_command_copy_buffer_to_texture, ia_render_cmd_copy_buffer_to_texture() */
typedef struct IA_PACKED ia_render_command_copy_buffer_to_texture_payload {
    ia_render_buffer                src_buffer;
    ia_render_texture               dst_texture;
    ia_render_access                dst_texture_access;
    u16                             region_count;
} ia_render_command_copy_buffer_to_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(copy_buffer_to_texture, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer                                src_buffer,
            ia_render_texture                               dst_texture,
            ia_render_access                                dst_texture_access,
            u16                                             region_count,
            ia_render_buffer_and_texture_copy_region const *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.src_buffer = src_buffer;
            cmd->payload.dst_texture = dst_texture;
            cmd->payload.dst_texture_access = dst_texture_access;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_copy_buffer_to_texture)), regions, sizeof(ia_render_buffer_and_texture_copy_region) * region_count);
        }, + ia_ssizeof(ia_render_buffer_and_texture_copy_region) * region_count)

/** ia_render_command_copy_texture_to_buffer, ia_render_cmd_copy_texture_to_buffer() */
typedef struct IA_PACKED ia_render_command_copy_texture_to_buffer_payload {
    ia_render_buffer                dst_buffer;
    ia_render_texture               src_texture;
    ia_render_access                src_texture_access;
    u16                             region_count;
} ia_render_command_copy_texture_to_buffer_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(copy_texture_to_buffer, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture                               src_texture,
            ia_render_access                                src_texture_access,
            ia_render_buffer                                dst_buffer,
            u16                                             region_count,
            ia_render_buffer_and_texture_copy_region const *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.dst_buffer = dst_buffer;
            cmd->payload.src_texture = src_texture;
            cmd->payload.src_texture_access = src_texture_access;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_copy_texture_to_buffer)), regions, sizeof(ia_render_buffer_and_texture_copy_region) * region_count);
        }, + ia_ssizeof(ia_render_buffer_and_texture_copy_region) * region_count)

/** Region for blit_texture payload. */
typedef struct ia_render_blit_region {
    ia_render_texture_array_slice   src_slice;
    ia_offset3d                     src_offset;
    ia_render_texture_array_slice   dst_slice;
    ia_offset3d                     dst_offset;
} ia_render_blit_region;

/** ia_render_command_blit_texture, ia_render_cmd_blit_texture() */
typedef struct IA_PACKED ia_render_command_blit_texture_payload {
    ia_render_texture               src_texture;
    ia_render_texture               dst_texture;
    ia_render_access                src_access;
    ia_render_access                dst_access;
    ia_filter_mode                  filter_mode;
    u16                             region_count;
} ia_render_command_blit_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(blit_texture, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture               src_texture,
            ia_render_texture               dst_texture,
            ia_render_access                src_access,
            ia_render_access                dst_access,
            ia_filter_mode                  filter_mode,
            u16                             region_count,
            ia_render_blit_region const    *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.src_texture = src_texture;
            cmd->payload.dst_texture = dst_texture;
            cmd->payload.src_access = src_access;
            cmd->payload.dst_access = dst_access;
            cmd->payload.filter_mode = filter_mode;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_blit_texture)), regions, sizeof(ia_render_blit_region) * region_count);
        }, + ia_ssizeof(ia_render_blit_region) * region_count)

/** ia_render_command_clear_texture, ia_render_cmd_clear_texture()  */
typedef struct IA_PACKED ia_render_command_clear_texture_payload {
    ia_render_texture                   dst_texture;
    ia_render_texture_mip_array_slice   dst_slice;
    ia_render_access                    dst_access;
    u8                                  is_clear_depth_stencil;
    ia_clear_value                      clear_value;
} ia_render_command_clear_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(clear_texture, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture                           dst_texture,
            ia_render_texture_mip_array_slice const    *dst_slice,
            ia_render_access                            dst_access,
            bool                                        is_clear_depth_stencil,
            ia_clear_value const                       *clear_value),
        { /* write */
            cmd->payload.dst_texture = dst_texture;
            memcpy(&cmd->payload.dst_slice, dst_slice, sizeof(ia_render_texture_mip_array_slice));
            cmd->payload.dst_access = dst_access;
            cmd->payload.is_clear_depth_stencil = is_clear_depth_stencil;
            memcpy(&cmd->payload.clear_value, clear_value, sizeof(ia_clear_value));
        }, IA_MAGIC_NOTHING())

/** Region for resolve_texture payload. */
typedef struct ia_render_resolve_region {
    ia_render_texture_array_slice   src_slice;
    ia_offset3d                     src_offset;
    ia_render_texture_array_slice   dst_slice;
    ia_offset3d                     dst_offset;
    ia_extent3d                     extent;
} ia_render_resolve_region;

/** ia_render_command_resolve_texture, ia_render_cmd_resolve_texture() */
typedef struct IA_PACKED ia_render_command_resolve_texture_payload {
    ia_render_texture               src_texture;
    ia_render_texture               dst_texture;
    ia_render_access                src_access;
    ia_render_access                dst_access;
    u16                             region_count;
} ia_render_command_resolve_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(resolve_texture, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture               src_texture,
            ia_render_texture               dst_texture,
            ia_render_access                src_access,
            ia_render_access                dst_access,
            u16                             region_count,
            ia_render_resolve_region const *regions),
        { /* write */
            ia_san_assert(region_count > 0 && regions != nullptr, "must not be null");
            cmd->payload.src_texture = src_texture;
            cmd->payload.dst_texture = dst_texture;
            cmd->payload.src_access = src_access;
            cmd->payload.dst_access = dst_access;
            cmd->payload.region_count = region_count;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_resolve_texture)), regions, sizeof(ia_render_resolve_region) * region_count);
        }, + ia_ssizeof(ia_render_resolve_region) * region_count)

/** ia_render_command_destroy_buffer_deferred, ia_render_cmd_destroy_buffer_deferred() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(destroy_buffer_deferred, ia_render_queue_type_any, ia_render_buffer, buffer)

/** ia_render_command_destroy_texture_deferred, ia_render_cmd_destroy_texture_deferred() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(destroy_texture_deferred, ia_render_queue_type_any, ia_render_texture, texture)

/** ia_render_command_destroy_texture_view_deferred, ia_render_cmd_destroy_texture_view_deferred() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(destroy_texture_view_deferred, ia_render_queue_type_any, ia_render_texture_view, texture_view)

/** ia_render_command_destroy_sampler_deferred, ia_render_cmd_destroy_sampler_deferred() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(destroy_sampler_deferred, ia_render_queue_type_any, ia_render_sampler, sampler)

/** ia_render_command_discard_buffer, ia_render_cmd_discard_buffer() */
_IA_DECL_RENDER_COMMAND_SIMPLE_PAYLOAD(discard_buffer, ia_render_queue_type_transfer, ia_render_buffer, buffer)

/** ia_render_command_discard_texture, ia_render_cmd_discard_texture() */
typedef struct IA_PACKED ia_render_command_discard_texture_payload {
    ia_render_texture   texture;
    u64                 subresource_range;
} ia_render_command_discard_texture_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(discard_texture, ia_render_queue_type_transfer,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_texture   texture,
            u64                 subresource_range),
        { /* write */
            cmd->payload.texture = texture;
            cmd->payload.subresource_range = subresource_range;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_root_constants, ia_render_cmd_root_constants() */
typedef struct IA_PACKED ia_render_command_root_constants_payload {
    ia_shader_stage_mask    stage_mask;
    u32                     offset, size;
} ia_render_command_root_constants_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(root_constants, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_shader_stage_mask    stage_mask,
            u32                     offset,
            u32                     size,
            void const             *data),
        { /* write */
            cmd->payload.stage_mask = stage_mask;
            cmd->payload.offset = offset;
            cmd->payload.size = size;
            memcpy(ia_offset_(cmd, ia_ssizeof(ia_render_command_root_constants)), data, size);
        }, + size)


/* TODO EVENTS AND BARRIERS */
#if 0
/** ia_render_command_signal_event, ia_render_cmd_signal_event() */
typedef struct IA_PACKED ia_render_command_signal_event_payload {
    // TODO
} ia_render_command_signal_event_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(signal_event, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())

/** ia_render_command_wait_events, ia_render_cmd_wait_events() */
typedef struct IA_PACKED ia_render_command_wait_events_payload {
    // TODO
} ia_render_command_wait_events_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(wait_events, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())

/** ia_render_command_reset_event, ia_render_cmd_reset_event() */
typedef struct IA_PACKED ia_render_command_reset_event_payload {
    // TODO
} ia_render_command_reset_event_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(reset_event, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())

/** ia_render_command_barriers_and_transitions, ia_render_cmd_barriers_and_transitions() */
typedef struct IA_PACKED ia_render_command_barriers_and_transitions_payload {
    // TODO
} ia_render_command_barriers_and_transitions_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(barriers_and_transitions, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            void *todo),
        { /* write */
            // TODO
        }, IA_MAGIC_NOTHING())
#endif /* TODO EVENTS AND BARRIERS */

/** ia_render_command_write_timestamps, ia_render_cmd_write_timestamps() */
typedef struct IA_PACKED ia_render_command_write_timestamps_payload {
    ia_render_query_pool    query_pool;
    u32                     query_index;
    ia_shader_stage_mask    stage_mask;
} ia_render_command_write_timestamps_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(write_timestamps, ia_render_queue_type_any, 
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_query_pool    query_pool,
            u32                     query_index,
            ia_shader_stage_mask    stage_mask),
        { /* write */
            cmd->payload.query_pool = query_pool;
            cmd->payload.query_index = query_index;
            cmd->payload.stage_mask = stage_mask;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_resolve_timestamps, ia_render_cmd_resolve_timestamps() */
typedef struct IA_PACKED ia_render_command_resolve_timestamps_payload {
    ia_render_query_pool    query_pool;
    u32                     start_index;
    u32                     count;
} ia_render_command_resolve_timestamps_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(resolve_timestamps, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_query_pool    query_pool,
            u32                     start_index,
            u32                     count),
        { /* write */
            cmd->payload.query_pool = query_pool;
            cmd->payload.start_index = start_index;
            cmd->payload.count = count;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_begin_label, ia_render_cmd_begin_label() */
typedef struct IA_PACKED ia_render_command_begin_label_payload {
    u16     label_length;
    f32x4   label_color;
} ia_render_command_begin_label_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(begin_label, ia_render_queue_type_any,
        IA_MAGIC_EVAL1( /* function arguments */
            f32x4       label_color,
            u16         label_length,
            char const *label_name),
        { /* write */
            u8 *label = ia_offset_(cmd, ia_ssizeof(ia_render_command_begin_label));
            cmd->payload.label_length = label_length;
            memcpy(cmd->payload.label_color, label_color, sizeof(f32x4));
            memcpy(label, label_name, label_length);
            /* The label is always null-terminated. */
            label[label_length] = '\0';
        }, + label_length + 1)

/** ia_render_command_end_label, ia_render_cmd_end_label() */
_IA_DECL_RENDER_COMMAND_NO_PAYLOAD(end_label, ia_render_queue_type_any)

/** Defines color, depth or stencil attachments. */
typedef struct ia_renderpass_attachment {
    ia_render_texture_view      texture_view;
    ia_render_texture_view      resolve_texture_view;
    ia_color_value              clear_value;
    ia_render_access            texture_access;
    ia_render_access            resolve_texture_access;
    ia_render_resolve_mode      resolve_mode;
    ia_read_op                  read_op;
    ia_write_op                 write_op;
    u8                          has_resolve;
} ia_renderpass_attachment;

/** ia_render_command_begin_renderpass, ia_render_cmd_begin_renderpass() */
typedef struct IA_PACKED ia_render_command_begin_renderpass_payload {
    u8                          color_attachment_count;
    u8                          has_depth_attachment;
    u8                          has_stencil_attachment;
    ia_rect2d                   render_area;
} ia_render_command_begin_renderpass_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(begin_renderpass, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            u8                               color_attachment_count,
            ia_renderpass_attachment const * restrict color_attachments,
            ia_renderpass_attachment const * restrict depth_attachment,
            ia_renderpass_attachment const * restrict stencil_attachment,
            ia_rect2d const                * render_area),
        { /* write */
            ia_san_assert(color_attachment_count > 0 && color_attachments != nullptr && render_area != nullptr, "must not be null");
             
            u8 *attachments = ia_offset_(cmd, ia_ssizeof(ia_render_command_begin_renderpass));
            isize o = ia_ssizeof(ia_renderpass_attachment) * color_attachment_count;
            cmd->payload.color_attachment_count = color_attachment_count;
            cmd->payload.has_depth_attachment = (depth_attachment != nullptr);
            cmd->payload.has_stencil_attachment = (stencil_attachment != nullptr);
            memcpy(&cmd->payload.render_area, render_area, sizeof(ia_rect2d));
            memcpy(attachments, color_attachments, o);

            if (cmd->payload.has_depth_attachment) {
                memcpy(attachments + o, depth_attachment, sizeof(ia_renderpass_attachment)); 
                o += ia_ssizeof(ia_renderpass_attachment);
            }
            if (cmd->payload.has_stencil_attachment)
                memcpy(attachments + o, stencil_attachment, sizeof(ia_renderpass_attachment)); 
        }, + (ia_ssizeof(ia_renderpass_attachment) * (color_attachment_count + 
                (depth_attachment != nullptr) + (stencil_attachment != nullptr)
        )))

/** ia_render_command_end_renderpass, ia_render_cmd_end_renderpass() */
_IA_DECL_RENDER_COMMAND_NO_PAYLOAD(end_renderpass, ia_render_queue_type_main)

/** ia_render_command_draw, ia_render_cmd_draw() */
typedef struct IA_PACKED ia_render_command_draw_payload {
    u32 vertex_count;
    u32 instance_count; 
    u32 first_vertex; 
    u32 first_instance; 
} ia_render_command_draw_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw, ia_render_queue_type_main, 
        IA_MAGIC_EVAL1( /* function arguments */
            u32 vertex_count, 
            u32 instance_count, 
            u32 first_vertex,
            u32 first_instance),
        { /* write */
            cmd->payload.vertex_count = vertex_count;
            cmd->payload.instance_count = instance_count;
            cmd->payload.first_vertex = first_vertex;
            cmd->payload.first_instance = first_instance;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_indexed, ia_render_cmd_draw_indexed() */
typedef struct IA_PACKED ia_render_command_draw_indexed_payload {
    u32 index_count; 
    u32 instance_count; 
    u32 first_index; 
    i32 vertex_offset;
    u32 first_instance;
} ia_render_command_draw_indexed_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_indexed, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            u32 index_count, 
            u32 instance_count, 
            u32 first_index,
            i32 vertex_offset,
            u32 first_instance),
        { /* write */
            cmd->payload.index_count = index_count;
            cmd->payload.instance_count = instance_count;
            cmd->payload.first_index = first_index;
            cmd->payload.vertex_offset = vertex_offset;
            cmd->payload.first_instance = first_instance;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_mesh_tasks, ia_render_cmd_draw_mesh_tasks() */
typedef struct IA_PACKED ia_render_command_draw_mesh_tasks_payload {
    u32 group_x, group_y, group_z;
} ia_render_command_draw_mesh_tasks_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_mesh_tasks, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            u32 group_x, u32 group_y, u32 group_z),
        { /* write */
            cmd->payload.group_x = group_x;
            cmd->payload.group_y = group_y;
            cmd->payload.group_z = group_z;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_type_draw_indirect, 
 *  ia_render_command_type_draw_indexed_indirect, 
 *  ia_render_command_type_draw_mesh_tasks_indirect */
typedef struct IA_PACKED ia_render_command_draw_indirect_payload {
    ia_render_buffer    indirect_buffer;
    u64                 indirect_buffer_offset;
    u32                 draw_count, draw_stride : 31;
    u32                 is_indexed : 1;
} ia_render_command_draw_indirect_payload;
typedef ia_render_command_draw_indirect_payload ia_render_command_draw_indexed_indirect_payload;
typedef ia_render_command_draw_indirect_payload ia_render_command_draw_mesh_tasks_indirect_payload;

/** ia_render_command_draw_indirect, ia_render_cmd_draw_indirect() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_indirect, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            u32                 draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.draw_count = draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_indexed_indirect, ia_render_cmd_draw_indexed_indirect() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_indexed_indirect, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            u32                 draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.draw_count = draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_mesh_tasks_indirect, ia_render_cmd_draw_mesh_tasks_indirect() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_mesh_tasks_indirect, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            u32                 draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.draw_count = draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_type_draw_indirect_count, 
 *  ia_render_command_type_draw_indexed_indirect_count, 
 *  ia_render_command_type_draw_mesh_tasks_indirect_count */
typedef struct IA_PACKED ia_render_command_draw_indirect_count_payload {
    ia_render_buffer    indirect_buffer;
    u64                 indirect_buffer_offset;
    ia_render_buffer    count_buffer;
    u64                 count_buffer_offset;
    u32                 max_draw_count, draw_stride : 31;
    u32                 is_indexed : 1;
} ia_render_command_draw_indirect_count_payload;
typedef ia_render_command_draw_indirect_count_payload ia_render_command_draw_indexed_indirect_count_payload;
typedef ia_render_command_draw_indirect_count_payload ia_render_command_draw_mesh_tasks_indirect_count_payload;

/** ia_render_command_draw_indirect_count, ia_render_cmd_draw_indirect_count() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_indirect_count, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            ia_render_buffer    count_buffer,
            u64                 count_buffer_offset,
            u32                 max_draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.count_buffer = count_buffer;
            cmd->payload.count_buffer_offset = count_buffer_offset;
            cmd->payload.max_draw_count = max_draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_indexed_indirect_count, ia_render_cmd_draw_indexed_indirect_count() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_indexed_indirect_count, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            ia_render_buffer    count_buffer,
            u64                 count_buffer_offset,
            u32                 max_draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.count_buffer = count_buffer;
            cmd->payload.count_buffer_offset = count_buffer_offset;
            cmd->payload.max_draw_count = max_draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_draw_mesh_tasks_indirect_count, ia_render_cmd_draw_mesh_tasks_indirect_count() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(draw_mesh_tasks_indirect_count, ia_render_queue_type_main,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset,
            ia_render_buffer    count_buffer,
            u64                 count_buffer_offset,
            u32                 max_draw_count,
            u32                 draw_stride,
            bool                is_indexed),
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.count_buffer = count_buffer;
            cmd->payload.count_buffer_offset = count_buffer_offset;
            cmd->payload.max_draw_count = max_draw_count;
            cmd->payload.draw_stride = draw_stride;
            cmd->payload.is_indexed = is_indexed;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_dispatch, ia_render_cmd_dispatch() */
typedef struct IA_PACKED ia_render_command_dispatch_payload {
    u32 group_x, group_y, group_z;
} ia_render_command_dispatch_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(dispatch, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1( /* function arguments */
            u32 group_x, u32 group_y, u32 group_z),
        { /* write */
            cmd->payload.group_x = group_x;
            cmd->payload.group_y = group_y;
            cmd->payload.group_z = group_z;
        }, IA_MAGIC_NOTHING())

/** ia_render_command_dispatch_indirect, ia_render_cmd_dispatch_indirect() */
typedef struct IA_PACKED ia_render_command_dispatch_indirect_payload {
    ia_render_buffer    indirect_buffer;
    u64                 indirect_buffer_offset;
} ia_render_command_dispatch_indirect_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(dispatch_indirect, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer    indirect_buffer,
            u64                 indirect_buffer_offset), 
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
        }, IA_MAGIC_NOTHING())

/* TODO WORK GRAPH */
#if 0
/** Holds a shader enqueue for the work graph, used by dispatch graph commands.
 *  The dispatch payloads are encoded into the command stream. */
typedef struct IA_PACKED ia_render_dispatch_graph_node {
    u32 node_index, payload_count, payload_offset, paylod_stride;
} ia_render_dispatch_graph_node;

/** ia_render_command_dispatch_graph, ia_render_cmd_dispatch_graph() */
typedef struct IA_PACKED ia_render_command_dispatch_graph_payload {
    u32 node_count, node_offset, node_stride;
} ia_render_command_dispatch_graph_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(dispatch_graph, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1(void *), IA_MAGIC_NOTHING(), IA_MAGIC_NOTHING()) // TODO

/** FIXME the api probably needs some fixing, relevant for D3D12 work graphs & VK_AMDX_shader_enqueue
 *  ia_render_command_type_dispatch_graph_indirect, 
 *  ia_render_command_type_dispatch_graph_indirect_count */
typedef struct IA_PACKED ia_render_command_dispatch_graph_indirect_payload {
    u64                                         scratch_size;
    ia_render_command_dispatch_graph_payload    graph; 
} ia_render_command_dispatch_graph_indirect_payload;
typedef ia_render_command_dispatch_graph_indirect_payload ia_render_command_dispatch_graph_indirect_count_payload;

/** ia_render_command_dispatch_graph_indirect, ia_render_cmd_dispatch_graph_indirect() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(dispatch_graph_indirect, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1(void *), IA_MAGIC_NOTHING(), IA_MAGIC_NOTHING()) // TODO

/** ia_render_command_dispatch_graph_indirect_count, ia_render_cmd_dispatch_graph_indirect_count() */
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(dispatch_graph_indirect_count, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1(void *), IA_MAGIC_NOTHING(), IA_MAGIC_NOTHING()) // TODO
#endif /* TODO WORK GRAPH */

/** ia_render_command_trace_rays, ia_render_cmd_trace_rays() */
typedef struct IA_PACKED ia_render_command_trace_rays_payload {
    u32                     width, height, depth;
    u32                     raygen_shader_binding_table_offset;
    u32                     miss_shader_binding_table_offset;
    u32                     hit_shader_binding_table_offset;
    u32                     callable_shader_binding_table_offset;
    ia_shader_binding_table shader_binding_table;
} ia_render_command_trace_rays_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(trace_rays, ia_render_queue_type_compute,
        IA_MAGIC_EVAL1( /* function arguments */
            u32                             width, 
            u32                             height, 
            u32                             depth,
            u32                             raygen_shader_binding_table_offset,
            u32                             miss_shader_binding_table_offset,
            u32                             hit_shader_binding_table_offset,
            u32                             callable_shader_binding_table_offset,
            ia_shader_binding_table const  *shader_binding_table), 
        { /* write */
            cmd->payload.width = width; cmd->payload.height = height; cmd->payload.depth = depth;
            cmd->payload.raygen_shader_binding_table_offset = raygen_shader_binding_table_offset;
            cmd->payload.miss_shader_binding_table_offset = miss_shader_binding_table_offset;
            cmd->payload.hit_shader_binding_table_offset = hit_shader_binding_table_offset;
            cmd->payload.callable_shader_binding_table_offset = callable_shader_binding_table_offset;
            memcpy(&cmd->payload.shader_binding_table, shader_binding_table, sizeof(ia_shader_binding_table));
        }, IA_MAGIC_NOTHING())

/** ia_render_command_trace_rays_indirect, ia_render_cmd_trace_rays_indirect() */
typedef struct IA_PACKED ia_render_command_trace_rays_indirect_payload {
    ia_render_buffer        indirect_buffer;
    u64                     indirect_buffer_offset;
    u32                     raygen_shader_binding_table_offset;
    u32                     miss_shader_binding_table_offset;
    u32                     hit_shader_binding_table_offset;
    u32                     callable_shader_binding_table_offset;
    ia_shader_binding_table shader_binding_table;
} ia_render_command_trace_rays_indirect_payload;
_IA_DECL_RENDER_COMMAND_DEFAULT_PAYLOAD(trace_rays_indirect, ia_render_queue_type_compute, 
        IA_MAGIC_EVAL1( /* function arguments */
            ia_render_buffer                indirect_buffer, 
            u64                             indirect_buffer_offset,
            u32                             raygen_shader_binding_table_offset,
            u32                             miss_shader_binding_table_offset,
            u32                             hit_shader_binding_table_offset,
            u32                             callable_shader_binding_table_offset,
            ia_shader_binding_table const  *shader_binding_table), 
        { /* write */
            cmd->payload.indirect_buffer = indirect_buffer;
            cmd->payload.indirect_buffer_offset = indirect_buffer_offset;
            cmd->payload.raygen_shader_binding_table_offset = raygen_shader_binding_table_offset;
            cmd->payload.miss_shader_binding_table_offset = miss_shader_binding_table_offset;
            cmd->payload.hit_shader_binding_table_offset = hit_shader_binding_table_offset;
            cmd->payload.callable_shader_binding_table_offset = callable_shader_binding_table_offset;
            memcpy(&cmd->payload.shader_binding_table, shader_binding_table, sizeof(ia_shader_binding_table));
        }, IA_MAGIC_NOTHING())

typedef union IA_PACKED ia_render_command {
    ia_render_command_header                            header;
    // TODO SHADER OBJECTS
    // ia_render_command_bind_shaders                      bind_shaders;
    ia_render_command_bind_shader_pipeline              bind_shader_pipeline;
    // TODO BVH
    // ia_render_command_build_bvhs                        build_bvhs;
    ia_render_command_fill_buffer                       fill_buffer;
    ia_render_command_clear_texture                     clear_texture;
    ia_render_command_copy_buffer                       copy_buffer;
    ia_render_command_copy_buffer_to_texture            copy_buffer_to_texture;
    ia_render_command_copy_texture                      copy_texture;
    ia_render_command_copy_texture_to_buffer            copy_texture_to_buffer;
    ia_render_command_blit_texture                      blit_texture;
    ia_render_command_resolve_texture                   resolve_texture;
    ia_render_command_destroy_buffer_deferred           destroy_buffer_deferred;
    ia_render_command_destroy_texture_deferred          destroy_texture_deferred;
    ia_render_command_destroy_texture_view_deferred     destroy_texture_view_deferred;
    ia_render_command_destroy_sampler_deferred          destroy_sampler_deferred;
    ia_render_command_discard_buffer                    discard_buffer;
    ia_render_command_discard_texture                   discard_texture;
    ia_render_command_root_constants                    root_constants;
    // TODO EVENTS AND BARRIERS
    // ia_render_command_signal_event                      signal_event;
    // ia_render_command_wait_events                       wait_events;
    // ia_render_command_reset_event                       reset_event;
    // ia_render_command_barriers_and_transitions          barriers_and_transitions;
    ia_render_command_write_timestamps                  write_timestamps;
    ia_render_command_resolve_timestamps                resolve_timestamps;
    ia_render_command_begin_label                       begin_label;
    ia_render_command_end_label                         end_label;
    ia_render_command_begin_renderpass                  begin_renderpass;
    ia_render_command_end_renderpass                    end_renderpass;
    ia_render_command_draw                              draw;
    ia_render_command_draw_indirect                     draw_indirect;
    ia_render_command_draw_indirect_count               draw_indirect_count;
    ia_render_command_draw_indexed                      draw_indexed;
    ia_render_command_draw_indexed_indirect_count       draw_indexed_indirect_count;
    ia_render_command_draw_mesh_tasks                   draw_mesh_tasks;
    ia_render_command_draw_mesh_tasks_indirect          draw_mesh_tasks_indirect;
    ia_render_command_draw_mesh_tasks_indirect_count    draw_mesh_tasks_indirect_count;
    ia_render_command_dispatch                          dispatch;
    ia_render_command_dispatch_indirect                 dispatch_indirect;
    // TODO WORK GRAPH
    // ia_render_command_dispatch_graph                    dispatch_graph;
    // ia_render_command_dispatch_graph_indirect           dispatch_graph_indirect;
    // ia_render_command_dispatch_graph_indirect_count     dispatch_graph_indirect_count;
    ia_render_command_trace_rays                        trace_rays;
    ia_render_command_trace_rays_indirect               trace_rays_indirect;
} ia_render_command;

#ifdef __cplusplus
}
#endif /* __cplusplus */
