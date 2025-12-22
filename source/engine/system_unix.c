#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ia/base/system.h>
#include <ia/base/log.h>

/* TODO check this from CMake */
#define IA_HAS_CLOCK_GETTIME 1

#ifdef IA_PLATFORM_UNIX
#include <dlfcn.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <pthread.h>

#ifdef IA_HAS_CLOCK_GETTIME
    #include <time.h>
#endif
#ifdef IA_PLATFORM_APPLE
    #include <mach/mach_time.h>
#else /* linux */
    #include <sched.h>
#endif /* IA_PLATFORM_APPLE */

static bool g_checked_monotonic = false;
static bool g_has_monotonic = false;

static void check_monotonic(void)
{
#ifdef IA_HAS_CLOCK_GETTIME
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        g_has_monotonic = true;
#elifdef IA_PLATFORM_APPLE
    if (mach_timebase_info(&mach_base_info) == 0)
        g_has_monotonic = true;
#endif
    g_checked_monotonic = true;
}

u64 ia_rtc_counter(void)
{
    u64 ticks = 0;

    if (IA_UNLIKELY(!g_checked_monotonic))
        check_monotonic();

    if (g_has_monotonic) {
#ifdef IA_HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= IA_NS_PER_SECOND;
        ticks += now.tv_nsec;
#elifdef IA_PLATFORM_APPLE
        ticks = mach_absolute_time();
#else
        IA_UNREACHABLE;
#endif
    } else {
        struct timeval now;
        gettimeofday(&now, nullptr);
        ticks = now.tv_sec;
        ticks *= IA_US_PER_SECOND;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 ia_rtc_frequency(void)
{
    if (IA_UNLIKELY(!g_checked_monotonic))
        check_monotonic();

    if (IA_LIKELY(g_has_monotonic)) {
#ifdef IA_HAS_CLOCK_GETTIME
        return IA_NS_PER_SECOND;
#elifdef IA_PLATFORM_APPLE
        u64 freq = mach_base_info.denom;
        freq *= IA_NS_PER_SECOND;
        freq /= mach_base_info.numer;
        return freq;
#else
        IA_UNREACHABLE;
#endif
    }
    return IA_US_PER_SECOND;
}

void *ia_module_open(char const *libname)
{
    void *lib = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
#ifndef IA_NDEBUG
    if (!lib) ia_error("dlopen `%s` failed: %s.", libname, dlerror());
#endif
    return lib;
}

void ia_module_close(void *lib)
{
    dlclose(lib);
}

void *ia_get_proc_address(void *lib, char const *procname)
{
#ifndef IA_NDEBUG
    char const *err;
    void *addr = dlsym(lib, procname);
    if ((err = dlerror()) != nullptr)
        ia_error("dlsym `%s` failed: %s.", procname, err);
    return addr;
#else
    return dlsym(lib, procname);
#endif
}

ia_thread_id ia_thread_id_current(void)
{
    return (ia_thread_id){ (u64)pthread_self() };
}

void ia_thread_create(
    ia_thread_id   *out_thread, 
    usize           stacksize,
    void           *stackaddr,
    void           *(*proc)(void *), 
    void           *argument)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        ia_fatal("Creating a joinable thread attribute with `pthread_attr_setdetachstate` failed.");
        ia_debugtrap();
    }
    if (pthread_attr_setstack(&attr, stackaddr, stacksize) != 0) {
        ia_fatal("Creating a stack thread attribute with `pthread_attr_setstack` failed.");
        ia_debugtrap();
    }
    if (pthread_create((pthread_t *)out_thread, &attr, proc, argument) != 0) {
        ia_fatal("Creating a thread with `pthread_create` failed.");
        ia_debugtrap();
    }
    pthread_attr_destroy(&attr);
}

void ia_thread_join(ia_thread_id thread)
{
    IA_ASSUME(!pthread_equal((pthread_t)thread.handle, pthread_self() && "thread can't join itself"));
    if (pthread_join((pthread_t)thread.handle, nullptr) != 0)
        ia_error("Joining a thread with `pthread_join` (no cancel) failed.");
}

void ia_thread_affinity(
    i32                 cpu_count,
    i32                 thread_count, 
    ia_thread_id const *threads)
{
#ifdef IA_PLATFORM_LINUX
    if (cpu_count <= 0 || thread_count <= 0)
        return;

    for (i32 i = 0; i < thread_count; i++) {
        cpu_set_t set;
        CPU_ZERO(&set);

        int cpu = i % cpu_count;
        CPU_SET(cpu, &set);

        pthread_t thread = (pthread_t)threads[i].handle;
        /* ignore failures */
        (void)pthread_setaffinity_np(thread, sizeof(cpu_set_t), &set);
    }
#else
    /* I don't know any way to set CPU affinity for non-linux platforms */
    (void)cpu_count; (void)thread_count; (void)threads;
#endif
}

#if (defined(IA_HAS_EXECINFO) || IA_HAS_INCLUDE("execinfo.h")) && !defined(IA_NDEBUG)
#include <execinfo.h>
#include <stdio.h>

static constexpr int STACK_TRACE_BUF_SIZE = 100;

i32 ia_dump_stack_trace(ia_strbuf *buf)
{
    i32 nptrs, len = buf->len;
    void *buffer[STACK_TRACE_BUF_SIZE];
    char **strings;

    if (buf->alloc <= 0) 
        ia_debugtrap();

    nptrs = backtrace(buffer, STACK_TRACE_BUF_SIZE);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == nullptr)
        return 0;

    buf->len += snprintf(buf->v + buf->len, buf->alloc - buf->len, "\n");
    for (i32 j = 1; j < nptrs && buf->len < buf->alloc; j++) 
        buf->len += snprintf(buf->v + buf->len, buf->alloc - buf->len, "%s\n", strings[j]);
    buf->len += snprintf(buf->v + buf->len, buf->alloc - buf->len, "\n");

    free(strings);
    return buf->len - len;;
}
#else
i32 ia_dump_stack_trace(ia_strbuf *buf)
{
    (void)buf;
    return 0;
}
#endif /* IA_HAS_EXECINFO */

void *ia_mmap(void)
{
    return nullptr;
}

void ia_munmap(
    void *mapped, 
    usize page_aligned)
{
    i32 res = munmap(mapped, page_aligned);
    if (res != 0) { ia_error("Failed munmap with status %d.", res); }
}
#endif /* IA_PLATFORM_UNIX */
