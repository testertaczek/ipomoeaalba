#include <ia/base/system.h>
#include <ia/base/log.h>

#ifdef IA_PLATFORM_WINDOWS

void ia_cpuinfo(
    i32 *out_thread_count, 
    i32 *out_core_count, 
    i32 *out_package_count)
{
    // TODO
}

ia_hugepage_sizes ia_hugetlbinfo(usize *out_total_ram)
{
    // TODO
}

u64 ia_rtc_counter(void)
{
    // TODO
}

u64 ia_rtc_frequency(void)
{
    // TODO
}

void *ia_module_open(char const *libname)
{
    // TODO
}

void ia_module_close(void *lib)
{
    // TODO
}

void *ia_get_proc_address(void *lib, char const *procname)
{
    // TODO
}

ia_thread_id ia_thread_id_current(void)
{
    // TODO
}

void ia_thread_create(
    ia_thread_id   *out_thread, 
    usize           stacksize,
    void           *stackaddr,
    void           *(*proc)(void *), 
    void           *argument)
{
    // TODO
}

void ia_thread_join(ia_thread_id thread)
{
    // TODO
}

void ia_thread_affinity(
    i32                 cpu_count,
    i32                 thread_count, 
    ia_thread_id const *threads)
{
    // TODO
}

i32 ia_dump_stack_trace(ia_strbuf *buf)
{
    // TODO
}

void *ia_mmap(void)
{
    // TODO
}

void ia_munmap(
    void *mapped, 
    usize page_aligned)
{
    // TODO
}
#endif /* IA_PLATFORM_WINDOWS */
