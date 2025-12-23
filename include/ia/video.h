#pragma once
/** @file ia/video.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/foundation.h>

/* ia_video_library: ia_video_adapter, ia_video_interface */
IA_DECL_LIBRARY(video);

typedef struct { ia_identifier id; } ia_display;
typedef struct { ia_identifier id; } ia_window;
/* input devices */
typedef struct { ia_identifier id; } ia_keyboard;
typedef struct { ia_identifier id; } ia_mouse;
typedef struct { ia_identifier id; } ia_joystick;
typedef struct { ia_identifier id; } ia_gamepad;

typedef enum ia_mouse_wheel : i8 {
    ia_mouse_wheel_normal = 0,
    ia_mouse_wheel_flipped,
} ia_mouse_wheel;

typedef enum ia_mouse_button : u8 {
    ia_mouse_button_1               = (1u << 0),
    ia_mouse_button_2               = (1u << 1),
    ia_mouse_button_3               = (1u << 2),
    ia_mouse_button_4               = (1u << 3),
    ia_mouse_button_5               = (1u << 4),
    ia_mouse_button_6               = (1u << 5),
    ia_mouse_button_7               = (1u << 6),
    ia_mouse_button_8               = (1u << 7),
    ia_mouse_button_left            = ia_mouse_button_1,
    ia_mouse_button_right           = ia_mouse_button_2,
    ia_mouse_button_middle          = ia_mouse_button_3,
    ia_mouse_button_invalid         = 0,
} ia_mouse_button;
#define ia_mouse_button_last ia_mouse_button_8

typedef enum ia_gamepad_button : u16 {
    ia_gamepad_button_a             = (1u << 0),
    ia_gamepad_button_b             = (1u << 1),
    ia_gamepad_button_x             = (1u << 2),
    ia_gamepad_button_y             = (1u << 3),
    ia_gamepad_button_left_bumper   = (1u << 4),
    ia_gamepad_button_right_bumper  = (1u << 5),
    ia_gamepad_button_back          = (1u << 6),
    ia_gamepad_button_start         = (1u << 7),
    ia_gamepad_button_guide         = (1u << 8),
    ia_gamepad_button_left_thumb    = (1u << 9),
    ia_gamepad_button_right_thumb   = (1u << 10),
    ia_gamepad_button_dpad_up       = (1u << 11),
    ia_gamepad_button_dpad_right    = (1u << 12),
    ia_gamepad_button_dpad_down     = (1u << 13),
    ia_gamepad_button_dpad_left     = (1u << 14),
    ia_gamepad_button_cross         = ia_gamepad_button_a,
    ia_gamepad_button_circle        = ia_gamepad_button_b,
    ia_gamepad_button_square        = ia_gamepad_button_x,
    ia_gamepad_button_triangle      = ia_gamepad_button_y,
    ia_gamepad_button_invalid       = 0,
} ia_gamepad_button;
#define ia_gamepad_button_last ia_gamepad_button_dpad_left

typedef enum ia_gamepad_axis : u8 {
    ia_gamepad_axis_left_x          = (1u << 0),
    ia_gamepad_axis_left_y          = (1u << 1),
    ia_gamepad_axis_right_x         = (1u << 2),
    ia_gamepad_axis_right_y         = (1u << 3),
    ia_gamepad_axis_left_trigger    = (1u << 4),
    ia_gamepad_axis_right_trigger   = (1u << 5),
} ia_gamepad_axis;
#define ia_gamepad_axis_last ia_gamepad_axis_right_trigger

typedef enum ia_gamepad_hat : u8 {
    ia_gamepad_hat_centered         = 0,
    ia_gamepad_hat_up               = (1u << 0),
    ia_gamepad_hat_right            = (1u << 1),
    ia_gamepad_hat_down             = (1u << 2),
    ia_gamepad_hat_left             = (1u << 3),
    ia_gamepad_hat_right_up         = (ia_gamepad_hat_right | ia_gamepad_hat_up),
    ia_gamepad_hat_right_down       = (ia_gamepad_hat_right | ia_gamepad_hat_down),
    ia_gamepad_hat_left_up          = (ia_gamepad_hat_left  | ia_gamepad_hat_up),
    ia_gamepad_hat_left_down        = (ia_gamepad_hat_left  | ia_gamepad_hat_down),
} ia_gamepad_hat;

typedef struct ia_gamepad_state {
    u8  buttons[15];
    f32 axes[6];
} ia_gamepad_state;

typedef ia_flags ia_window_flags;
typedef enum ia_window_flag_bits : ia_window_flags {
    ia_window_flag_none             = 0u,
    ia_window_flag_visible          = (1u << 0),
    ia_window_flag_should_close     = (1u << 1),
    ia_window_flag_fullscreen       = (1u << 2),
    ia_window_flag_maximized        = (1u << 3),
    ia_window_flag_iconified        = (1u << 4),
    ia_window_flag_resizable        = (1u << 5),
    ia_window_flag_decorated        = (1u << 6),
    ia_window_flag_input_focus      = (1u << 7),
    ia_window_flag_mouse_focus      = (1u << 8),
    ia_window_flag_mouse_captured   = (1u << 9),
    ia_window_flag_mouse_relative   = (1u << 10),
    ia_window_flag_mouse_grabbed    = (1u << 11),
    ia_window_flag_keyboard_grabbed = (1u << 12),
    ia_window_flag_cursor_hovered   = (1u << 13),
    ia_window_flag_modal            = (1u << 14),
    ia_window_flag_occluded         = (1u << 15),
    ia_window_flag_transparent      = (1u << 16),
} ia_window_flag_bits;

typedef struct ia_window_info {
    i32             width, height;
    i32             min_width, min_height;
    i32             max_width, max_height;
    i32             numer, denom;
    ia_window_flags hints;
    char const     *title;
} ia_window_info;

typedef ia_flags ia_display_flags;
typedef enum ia_display_flag_bits : ia_display_flags {
    ia_display_flag_none            = 0u,
    ia_display_flag_primary         = (1u << 0),
    ia_display_flag_hdr             = (1u << 1),
    ia_display_flag_internal        = (1u << 2),
    ia_display_flag_external        = (1u << 3),
} ia_display_flag_bits;

typedef struct ia_display_info {
    ia_display          handle;
    i32                 width_px, height_px;
    i32                 origin_x, origin_y;
    u32                 refresh_mhz;
    /** Will be a valid window handle if it runs this display in fullscreen mode, 0 otherwise. */
    ia_window           fullscreen;
    ia_display_flags    flags;
    f32                 dpi_x, dpi_y;
    char const         *name;
} ia_display_info;

/** Polls OS events. */
typedef void (IA_CALL *ia_video_poll_fn)(ia_video_adapter *adapter);
#define IA_VIDEO_POLL_FN(X) \
    void IA_CALL _ia_##X##_video_poll(ia_video_adapter *adapter)

/** Returns display information prepared by the last call to `ia_video_poll_fn`.
 *  The first poll is always done during backend initialization. Output is allocated via drifter. */
typedef ia_result (IA_CALL *ia_video_enumerate_displays_fn)(
    ia_video_adapter   *adapter, 
    i32                *out_display_count, 
    ia_display_info   **out_display_infos);
#define IA_VIDEO_ENUMERATE_DISPLAYS(X) \
    ia_result IA_CALL _ia_##X##_video_enumerate_displays( \
        ia_video_adapter   *adapter, \
        i32                *out_display_count, \
        ia_display_info    *out_display_infos)

/** TODO docs. */
typedef ia_result (IA_CALL *ia_video_create_window_fn)(
    ia_video_adapter       *adapter,
    ia_window_info const   *info,
    ia_window              *out_window);
#define IA_VIDEO_CREATE_WINDOW_FN(X) \
    ia_result IA_CALL _ia_##X##_video_create_window( \
        ia_video_adapter       *adapter, \
        ia_window_info const   *info, \
        ia_window              *out_window)

/** TODO docs. */
typedef void (IA_CALL *ia_video_destroy_window_fn)(
    ia_video_adapter   *adapter,
    ia_window           window);
#define  IA_VIDEO_DESTROY_WINDOW_FN(X) \
    void IA_CALL _ia_##X##_video_destroy_window( \
        ia_video_adapter   *adapter, \
        ia_window           window)

/** TODO docs. */
typedef ia_result (IA_CALL *ia_video_set_fullscreen_fn)(
    ia_video_adapter   *adapter,
    ia_window           window,
    ia_display          display);
#define IA_VIDEO_SET_FULLSCREEN_FN(X) \
    ia_result IA_CALL _ia_##X##_video_set_fullscreen( \
        ia_video_adapter   *adapter, \
        ia_window           window, \
        ia_display          display)

/** Atomically reads the window flags and returns their current state. */
typedef ia_window_flags (IA_CALL *ia_video_get_window_flags_fn)(
    ia_video_adapter   *adapter, 
    ia_window           window);
#define IA_VIDEO_GET_WINDOW_FLAGS_FN(X) \
    ia_flags IA_CALL _ia_##X##_video_get_window_flags( \
        ia_video_adapter   *adapter, \
        ia_window           window)

/** Outputs the native display handle: wl_display *, HMONITOR, etc. Headless returns nullptr without error. */
typedef ia_result (IA_CALL *ia_video_get_native_display_handle_fn)(
    ia_video_adapter   *adapter, 
    ia_display          display, 
    void               *out_handle);
#define IA_VIDEO_GET_NATIVE_DISPLAY_HANDLE_FN(X) \
    ia_result IA_CALL _ia_##X##_video_get_native_display_handle( \
        ia_video_adapter   *adapter, \
        ia_display          display, \
        void               *out_handle)

/** Outputs the native window handle: wl_surface *, HWND, etc. Headless returns nullptr without error. */
typedef ia_result (IA_CALL *ia_video_get_native_window_handle_fn)(
    ia_video_adapter   *adapter, 
    ia_window           window, 
    void               *out_handle);
#define IA_VIDEO_GET_NATIVE_WINDOW_HANDLE_FN(X) \
    ia_result IA_CALL _ia_##X##_video_get_native_window_handle( \
        ia_video_adapter   *adapter, \
        ia_window           window, \
        void               *out_handle)

struct ia_video_interface {
    ia_interface_header                     header;
    ia_video_poll_fn                        poll;
    ia_video_enumerate_displays_fn          enumerate_displays;
    ia_video_create_window_fn               create_window;
    ia_video_destroy_window_fn              destroy_window;
    ia_video_set_fullscreen_fn              set_fullscreen;
    ia_video_get_window_flags_fn            get_window_flags;
    ia_video_get_native_display_handle_fn   get_native_display_handle;
    ia_video_get_native_window_handle_fn    get_native_window_handle;
};

enum : i32 {
    ia_video_adapter_headless = 0,
    ia_video_adapter_wayland,
    ia_video_adapter_xcb,
    ia_video_adapter_win32,
    ia_video_adapter_android,
    ia_video_adapter_drm_kms,
    ia_video_adapter_emscripten,
    ia_video_adapter_cocoa,
    ia_video_adapter_uikit,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IA_VIDEO_WAYLAND
IA_API IA_DECL_LIBRARY_IMPL(video, wayland);
#endif /* IA_VIDEO_WAYLAND */
#ifdef IA_VIDEO_XCB
IA_API IA_DECL_LIBRARY_IMPL(video, xcb);
#endif /* IA_VIDEO_XCB */
#ifdef IA_VIDEO_WIN32
IA_API IA_DECL_LIBRARY_IMPL(video, win32);
#endif /* IA_VIDEO_WIN32 */
#ifdef IA_VIDEO_ANDROID
IA_API IA_DECL_LIBRARY_IMPL(video, android);
#endif /* IA_VIDEO_ANDROID */
#ifdef IA_VIDEO_DRM_KMS
IA_API IA_DECL_LIBRARY_IMPL(video, drm_kms);
#endif /* IA_VIDEO_DRM_KMS */
#ifdef IA_VIDEO_EMSCRIPTEN
IA_API IA_DECL_LIBRARY_IMPL(video, emscripten);
#endif /* IA_VIDEO_EMSCRIPTEN */
#ifdef IA_VIDEO_COCOA
IA_API IA_DECL_LIBRARY_IMPL(video, cocoa);
#endif /* IA_VIDEO_COCOA */
#ifdef IA_VIDEO_UIKIT
IA_API IA_DECL_LIBRARY_IMPL(video, uikit);
#endif /* IA_VIDEO_UIKIT */

/** The headless display backend is always available. */
IA_API IA_DECL_LIBRARY_IMPL(video, headless);

/** Picks the first working backend in an order optimal for the host platform.
 *  The `headless` backend is ignored, and if any other backend fails, must be called explicitly. */
IA_API IA_DECL_LIBRARY_IMPL(video, auto);

#ifdef __cplusplus
}
#endif /* __cplusplus */
