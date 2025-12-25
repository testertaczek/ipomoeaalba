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
typedef enum ia_render_access : u8 {
    ia_render_access_none = 0,
    ia_render_access_shader_read,
    ia_render_access_shader_write,
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
