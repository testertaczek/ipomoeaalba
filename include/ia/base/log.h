#pragma once
/** @file ia/base/log.h
 *  @brief Operations for tracing, logging and assertion.
 *
 *  Conventional levels for the job system:
 *     -4  Fatal    (unrecoverable error, application cannot continue safely)
 *     -3  Error    (operation failed)
 *     -2  Warning  (operation succeeded with issues)
 *      0  Trace    (infrequent, high-level events)
 *      1  Debug 1  (coarse debug logging)
 *      2  Debug 2  (detailed debug logging)
 *      3  Debug 3  (very verbose debug logging)
 *      4  Journal  (extremely verbose, diagnostic tracing)
 *
 *  Runtime filtering applies to `ia_log*()` calls
 *  Compile-time filtering applies to `ia_dbg_*()`, `ia_trace()` and `ia_jrnl()`.
 *
 *  The following macros control which logging calls are compiled in:
 *      IA_LOG_0    Disable all tracing and debug logging.
 *      IA_LOG_1    Enable coarse debug logging (ia_dbg_1).
 *      IA_LOG_2    Enable detailed debug logging (ia_dbg_2).
 *      IA_LOG_3    Enable verbose logging (ia_dbg_3, ia_jrnl).
 *
 *  Defaults to IA_LOG_3 for debug builds and IA_LOG_0 for release builds.
 *  Disabled log macros compile to no-ops and generate no code.
 */
#include <ia/base/types.h>
#include <ia/base/sanitize.h>

#if !defined(IA_LOG_0) || defined(IA_LOG_1) || defined(IA_LOG_2) || defined(IA_LOG_3)
    #ifndef IA_NDEBUG
        #define IA_LOG_3 /* enable all tracing, debug mode */
    #else
        #define IA_LOG_0 /* enable infrequent tracing, release mode */
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Aborts the game and returns the given status code back to tty.
 *  TODO RIGHT NOW THIS IS AN UNSAFE OPERATION. */
IA_NORETURN IA_API void IA_CALL
ia_abort_(
    i32         status,
    char const *file,
    i32         line);
/** Macro helper for `ia_abort_()`. */
#define ia_abort(status) ia_abort_(status, __FILE__, __LINE__)

/** Always emits a log message, regardless of the current log level.
 *  Intended for critical diagnostics, init/fini messages, and unconditional output.
 *  Bypasses runtime log filtering. */
IA_API void IA_CALL
ia_print_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    ...) IA_PRINTF(4,5);
/** Macro helper for `ia_print_()`. */
#define ia_print(level, ...) ia_print_(level, __FILE__, __LINE__, __VA_ARGS__)

/** As `ia_print_`, but directly accepts a variable argument list. */
IA_API void IA_CALL
ia_printv_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    va_list     args);
/** Macro helper for `ia_printv_()`. */
#define ia_printv(level, fmt, args) ia_printv_(level, __FILE__, __LINE__, fmt, args)

/** Emits a log message only if `level` is less than or equal to the currently configured log level.
 *  Intended for regular application logging and typical diagnostics. */
IA_API void IA_CALL
ia_log_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    ...) IA_PRINTF(4,5);
/** Macro helper for `ia_log_()`. */
#define ia_log(level, ...) ia_log_(level, __FILE__, __LINE__, __VA_ARGS__)

/** As `ia_log_`, but directly accepts a variable argument list. */
IA_API void IA_CALL
ia_logv_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    va_list     args);
/** Macro helper for `ia_logv_()`. */
#define ia_logv(level, fmt, args) ia_logv_(level, __FILE__, __LINE__, fmt, args)

/** Tracing. Used for logging of infrequent events. */
#define ia_trace_(file, line, ...) ia_log_(0, file, line, __VA_ARGS__)
#define ia_trace(...) ia_trace_(__FILE__, __LINE__, __VA_ARGS__)

/** Warning. Used when an issue occurs, but operation is successful. */
#define ia_warn_(file, line, ...) ia_log_(-2, file, line, __VA_ARGS__)
#define ia_warn(...) ia_warn_(__FILE__, __LINE__, __VA_ARGS__)

/** Error. Used when an issue occurs, and operation fails. */
#define ia_error_(file, line, ...) ia_log_(-3, file, line, __VA_ARGS__)
#define ia_error(...) ia_error_(__FILE__, __LINE__, __VA_ARGS__)

/** Fatal. Used when an issue occurs, and the application cannot continue. */
#define ia_fatal_(file, line, ...) ia_log_(-4, file, line, __VA_ARGS__)
#define ia_fatal(...) ia_fatal_(__FILE__, __LINE__, __VA_ARGS__)

#ifndef IA_LOG_0
    #ifdef IA_LOG_3
        #define ia_jrnl(...) ia_log(4, __VA_ARGS__)
        #define ia_dbg_3(...) ia_log(3, __VA_ARGS__)
        #ifndef IA_LOG_2
            #define IA_LOG_2
        #endif
    #else
        #define ia_jrnl(...)
        #define ia_dbg_3(...)
    #endif /* IA_LOG_3 */

    #ifdef IA_LOG_2
        #define ia_dbg_2(...) ia_log(2, __VA_ARGS__)
        #ifndef IA_LOG_1
            #define IA_LOG_1
        #endif
    #else
        #define ia_dbg_2(...)
    #endif /* IA_LOG_2 */

    #ifdef IA_LOG_1
        #define ia_dbg_1(...) ia_log(1, __VA_ARGS__)
    #else
        #define ia_dbg_1(...)
    #endif /* IA_LOG_1 */
#else
#undef ia_trace
#define ia_trace(...)
#define ia_dbg_1(...)
#define ia_dbg_2(...)
#define ia_dbg_3(...)
#define ia_jrnl(...)
#endif /* IA_LOG_0 */

/** Controls how to interpret a failed assertion. */
typedef enum ia_assert_status : i8 {
    ia_assert_status_continue = 0,
    ia_assert_status_trap = -1,
    ia_assert_status_abort = -2,
} ia_assert_status;

/** Logs a failed assertion. Returns an innate code for how to handle the runtime. */
IA_API ia_assert_status IA_CALL
ia_assert_log_(
    char const *condition,
    char const *file,
    i32         line,
    char const *fmt,
    ...) IA_PRINTF(4,5);

#if defined(IA_NDEBUG) && !defined(IA_KEEP_ASSERT)
#define ia_assert(condition, ...) ((void)0)
#else
#define ia_assert(condition, ...) do {                              \
        if (IA_UNLIKELY(!(condition))) {                            \
            char const *file = __FILE__;                            \
            i32 line = __LINE__;                                    \
            ia_assert_status assert_status = ia_assert_log_(        \
                    #condition, file, line, __VA_ARGS__);           \
            if (assert_status == ia_assert_status_trap) {           \
                ia_debugtrap();                                     \
            } else if (assert_status == ia_assert_status_abort) {   \
                ia_abort_(-1, file, line);                          \
            }                                                       \
        }                                                           \
    } while (0)
#endif /* ia_assert */

/** Debug assert. Is only valid in debug mode (ignores IA_KEEP_ASSERT). */
#ifdef IA_DEBUG
    #define ia_dbg_assert(condition, ...) ia_assert(condition, __VA_ARGS__)
#else
    #define ia_dbg_assert(condition, ...) ((void)0)
#endif

/** Sanitize assert. Is only valid in sanitized mode (ignores IA_KEEP_ASSERT). */
#ifdef IA_SANITIZE
    #define ia_san_assert(condition, ...) ia_assert(condition, __VA_ARGS__)
#else
    #define ia_san_assert(condition, ...) ((void)0)
#endif

#if 0
/** TODO docs */
IA_API i32 IA_CALL
ia_log_set_level(i32 val);

/** TODO docs */
IA_API i32 IA_CALL
ia_log_get_level(void);

/** TODO docs */
IA_API bool IA_CALL
ia_log_set_color_output(bool val);

/** TODO docs */
IA_API bool IA_CALL
ia_log_set_fiber_output(bool val);

/** TODO docs */
IA_API bool IA_CALL
ia_log_set_thread_output(bool val);

/** TODO docs */
IA_API bool IA_CALL
ia_log_set_timestamp_output(bool val);
#endif // TODO

#ifdef __cplusplus
}
#endif /* __cplusplus */
