#include <ia/render.h>
#include <ia/render_commands.h>

void *ia_render_cmd_alloc(
    ia_render_command_stream   *stream,
    i32                         size,
    i32                         align)
{
    i32 offset = ia_align(ia_darray_alloc(&stream->da), align);
    if (offset + size > ia_darray_alloc(&stream->da))
        ia_darray_resize_as_bytes(&stream->da, offset + size, 1, ia_drift_allocator);
    stream->da.len = offset + size;
    return ia_offset_(&stream->da.v, offset);
}
