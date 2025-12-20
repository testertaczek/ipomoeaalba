#pragma once
/// @file ia/modules/video.h
/// @brief TODO docs
///
/// TODO docs
#ifdef IA_MODULE_VIDEO
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum ia_mouse_wheel : i8 {
    IA_MOUSE_WHEEL_NORMAL = 0,
    IA_MOUSE_WHEEL_FLIPPED,
} ia_mouse_wheel;

typedef enum ia_mouse_button : u16 {
    IA_MOUSE_BUTTON_1       = 0x0001,
    IA_MOUSE_BUTTON_2       = 0x0002,
    IA_MOUSE_BUTTON_3       = 0x0004,
    IA_MOUSE_BUTTON_4       = 0x0008,
    IA_MOUSE_BUTTON_5       = 0x0010,
    IA_MOUSE_BUTTON_6       = 0x0020,
    IA_MOUSE_BUTTON_7       = 0x0040,
    IA_MOUSE_BUTTON_8       = 0x0080,
    IA_MOUSE_BUTTON_LEFT    = IA_MOUSE_BUTTON_1,
    IA_MOUSE_BUTTON_RIGHT   = IA_MOUSE_BUTTON_2,
    IA_MOUSE_BUTTON_MIDDLE  = IA_MOUSE_BUTTON_3,
    IA_MOUSE_BUTTON_INVALID = 0,
} ia_mouse_button;
#define IA_MOUSE_BUTTON_LAST IA_MOUSE_BUTTON_8

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* IA_MODULE_VIDEO */
