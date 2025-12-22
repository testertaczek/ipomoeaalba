#include <ia/foundation.h>

void ia_abort_(
    i32         status,
    char const *file,
    i32         line)
{
    // TODO
    exit(status);
}

void ia_print_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt, 
    ...)
{
    // TODO
}

void ia_printv_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    va_list     args)
{
    // TODO
}

void ia_log_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    ...)
{
    // TODO
}
void ia_logv_(
    i32         level,
    char const *file,
    i32         line,
    char const *fmt,
    va_list     args)
{
    // TODO
}

ia_assert_status ia_assert_log_(
    char const *condition,
    char const *file,
    i32         line,
    char const *fmt,
    ...) 
{
    // TODO
    return ia_assert_status_abort;
}

i32 ia_worker_thread_index(void)
{
    // TODO
    return 0;
}

ia_work_chain ia_submit_work(
    i32                     work_count,
    ia_work_details const  *work)
{
    // TODO
    return nullptr;
}

void ia_yield(ia_work_chain chain)
{
    // TODO
}

i32 ia_foundation_main(
    ia_foundation_main_fn   main_fn,
    void                   *main_data,
    ia_foundation          *foundation)
{
    // TODO
    return 0;
}
