#pragma once
/** @file ia/base/defer.h
 *  @brief TODO docs
 *
 *  TODO docs
 */
#include <ia/base/types.h>
#include <ia/base/magic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_defer_record {
    void *next_target;
    i64   guard;
} ia_defer_record;

#define IA_DEFER_GUARD_INIT ((i64)0xdefe11defe11defeLL)

#define ia_defer_begin                  \
    ia_defer_record __defer_table = {   \
        nullptr, IA_DEFER_GUARD_INIT,   \
    };                                  \
    void *__defer_return_label = nullptr

#define _IA_DEFER_LABEL(x) \
    IA_MAGIC_GLUE2(__defer_label_, x)

#define _IA_DEFER_NODE(x) \
    IA_MAGIC_GLUE2(__defer_node_, x)

/** The unnecessary extra block after the label is to prevent 
 *  clang-format from wrapping oddly. */
#define ia_defer(...) \
    ia_defer_record _IA_DEFER_NODE(__LINE__) = {0ULL};                          \
    if (IA_LIKELY(_IA_DEFER_NODE(__LINE__).guard != IA_DEFER_GUARD_INIT)) {     \
        _IA_DEFER_NODE(__LINE__).guard       = IA_DEFER_GUARD_INIT;             \
        _IA_DEFER_NODE(__LINE__).next_target = __defer_table.next_target;       \
        __defer_table.next_target            = && _IA_DEFER_LABEL(__LINE__);    \
    }                                                                           \
    if (false) {                                                                \
        _IA_DEFER_LABEL(__LINE__) :                                             \
        {                                                                       \
            _IA_DEFER_NODE(__LINE__).guard = 0ULL;                              \
            __VA_ARGS__                                                         \
            if (_IA_DEFER_NODE(__LINE__).next_target == nullptr) {              \
                goto *(__defer_return_label);                                   \
            }                                                                   \
        }                                                                       \
        goto *(_IA_DEFER_NODE(__LINE__).next_target);                           \
    }

#define ia_defer_return \
    __defer_return_label = &&_IA_DEFER_LABEL(__LINE__); \
    if (__defer_table.next_target) { goto *(_defer_table.next_target); } \
    _IA_DEFER_LABEL(__LINE__) : return    

#define ia_defer_longjmp \
    __defer_return_label = &&_IA_DEFER_LABEL(__LINE__); \
    if (__defer_table.next_target) { goto *(_defer_table.next_target); } \
    _IA_DEFER_LABEL(__LINE__) : longjmp(jump_env, jump_passed_state)

#ifdef __cplusplus
}
#endif /* __cplusplus */
