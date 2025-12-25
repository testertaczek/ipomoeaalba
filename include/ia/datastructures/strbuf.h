#pragma once
/** @file ia/datastructures/strbuf.h
 *  @brief Utility for constructing strings.
 * 
 *  These are meant to be used with the usual C string and memory APIs.
 *  Given that the length of the buffer is known, it's often better to 
 *  use the standard mem* functions than the str* ones (e.g. memchr vs strchr).
 */
#include <ia/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The string buffer structure. */
typedef struct ia_strbuf {
    char   *v; /**< A byte array of atleast `len + 1` bytes allocated. */
    i32     len, alloc;
} ia_strbuf;

static constexpr ia_strbuf ia_strbuf_init = { .v = nullptr, .len = 0, .alloc = 0 };

/** Append the buffer using maximum of N bytes from a (preferrably null-terminated) cstr. */
IA_API void IA_CALL 
ia_strbuf_appendstrn(
    ia_strbuf  *buf, 
    char const *str, 
    i32         n);

#ifdef __cplusplus
}
#endif /* __cplusplus */
