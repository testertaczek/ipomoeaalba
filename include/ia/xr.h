#pragma once
/** @file ia/xr.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/foundation.h>

struct ia_video_adapter;
struct ia_window;

/* ia_xr_library: ia_xr_adapter, ia_xr_interface */
IA_DECL_LIBRARY(xr);

typedef struct { ia_identifier id; } ia_xr_session;
typedef struct { ia_identifier id; } ia_xr_space;
typedef struct { ia_identifier id; } ia_xr_view;
typedef struct { ia_identifier id; } ia_xr_input;
typedef struct { ia_identifier id; } ia_xr_swapchain;

typedef enum ia_xr_input_device_type : i8 {
    ia_xr_input_device_type_hmd = 0,
    ia_xr_input_device_type_controller,
    ia_xr_input_device_type_tracker,
} ia_xr_input_device_type;

typedef struct ia_xr_input_device_info {
    ia_xr_input_device_type type;
    i8                      handedness;
    u32                     supported_inputs;
    char const             *name;
} ia_xr_input_device_info;

typedef struct ia_xr_frame_info {
    u64                     predicted_display_time_ns;
    u64                     frame_id;
    i32                     view_count;
} ia_xr_frame_info;

typedef struct ia_xr_view_info {
    ia_xr_swapchain         swapchain;
    i32                     image_index;
    f32m4x4                 view;
    f32m4x4                 projection;
} ia_xr_view_info;

/** TODO docs */
typedef ia_result (IA_CALL *ia_xr_create_session_fn)(
    ia_xr_adapter              *adapter,
    struct ia_video_adapter    *video_adapter, /* optional, may be null */
    struct ia_window            window, /* optional, may be 0 */
    ia_xr_session              *out_session);
#define IA_XR_CREATE_SESSION_FN(X) \
    ia_result IA_CALL _ia_##X##_xr_create_session( \
        ia_xr_adapter              *adapter, \
        struct ia_video_adapter    *video_adapter, \
        struct ia_window            window, \
        ia_xr_session              *out_session)

/** TODO docs */
typedef void (IA_CALL *ia_xr_destroy_session_fn)(
    ia_xr_adapter  *adapter,
    ia_xr_session   session);
#define IA_XR_DESTROY_SESSION_FN(X) \
    void IA_CALL _ia_##X##_xr_destroy_session( \
        ia_xr_adapter  *adapter, \
        ia_xr_session   session)

/** TODO docs */
typedef ia_result (IA_CALL *ia_xr_wait_frame_fn)(
    ia_xr_adapter      *adapter,
    ia_xr_session       session,
    ia_xr_frame_info   *out_frame_info);
#define IA_XR_WAIT_FRAME_FN(X) \
    ia_result IA_CALL _ia_##X##_xr_wait_frame( \
        ia_xr_adapter      *adapter, \
        ia_xr_session       session, \
        ia_xr_frame_info   *out_frame_info)

/** TODO docs */
typedef ia_result (IA_CALL *ia_xr_begin_frame_fn)(
    ia_xr_adapter          *adapter,
    ia_xr_session           session,
    ia_xr_frame_info const *frame_info,
    i32                    *inout_view_count,
    ia_xr_view_info        *out_views);
#define IA_XR_BEGIN_FRAME_FN(X) \
    ia_result IA_CALL _ia_##X##_xr_begin_frame( \
        ia_xr_adapter          *adapter, \
        ia_xr_session           session, \
        ia_xr_frame_info const *frame_info, \
        i32                    *inout_view_count, \
        ia_xr_view_info        *out_views)

/** TODO docs */
typedef ia_result (IA_CALL *ia_xr_end_frame_fn)(
    ia_xr_adapter          *adapter,
    i32                     view_count,
    ia_xr_view_info const  *views);
#define IA_XR_END_FRAME_FN(X) \
    ia_result IA_CALL _ia_##X##_xr_end_frame( \
        ia_xr_adapter          *adapter, \
        i32                     view_count, \
        ia_xr_view_info const  *views)

struct ia_xr_interface {
    ia_interface_header         header;
    ia_xr_create_session_fn     create_session;
    ia_xr_destroy_session_fn    destroy_session;
    ia_xr_wait_frame_fn         wait_frame;
    ia_xr_begin_frame_fn        begin_frame;
    ia_xr_end_frame_fn          end_frame;
};

enum : i32 {
    ia_xr_adapter_headless = 0,
    ia_xr_adapter_openxr,
    ia_xr_adapter_webxr,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IA_XR_OPENXR
IA_API IA_DECL_LIBRARY_IMPL(xr, openxr);
#endif /* IA_XR_OPENXR */
#ifdef IA_XR_WEBXR
IA_API IA_DECL_LIBRARY_IMPL(xr, webxr);
#endif /* IA_XR_WEBXR */

/** The headless XR backend is always available. */
IA_API IA_DECL_LIBRARY_IMPL(xr, headless);

/** Picks the first working backend in an order optimal for the host platform.
 *  The `headless` backend is ignored, and if any other backend fails, must be called explicitly. */
IA_API IA_DECL_LIBRARY_IMPL(xr, auto);

#ifdef __cplusplus
}
#endif /* __cplusplus */
