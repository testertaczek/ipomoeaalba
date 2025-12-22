#pragma once
/// @file ia/modules/video.h
/// @brief TODO docs
///
/// TODO docs
#include <ia/foundation.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum ia_mouse_wheel : i8 {
    ia_mouse_wheel_normal = 0,
    ia_mouse_wheel_flipped,
} ia_mouse_wheel;

typedef enum ia_mouse_button : u16 {
    ia_mouse_button_1       = 0x0001,
    ia_mouse_button_2       = 0x0002,
    ia_mouse_button_3       = 0x0004,
    ia_mouse_button_4       = 0x0008,
    ia_mouse_button_5       = 0x0010,
    ia_mouse_button_6       = 0x0020,
    ia_mouse_button_7       = 0x0040,
    ia_mouse_button_8       = 0x0080,
    ia_mouse_button_left    = ia_mouse_button_1,
    ia_mouse_button_right   = ia_mouse_button_2,
    ia_mouse_button_middle  = ia_mouse_button_3,
    ia_mouse_button_invalid = 0,
} ia_mouse_button;
#define ia_mouse_button_last ia_mouse_button_8

#ifdef __cplusplus
}
#endif /* __cplusplus */
