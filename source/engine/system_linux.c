#include <ia/base/system.h>
#include <ia/base/defer.h>
#include <ia/base/log.h>

#ifdef IA_PLATFORM_LINUX
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void ia_cpuinfo(
    i32 *out_thread_count, 
    i32 *out_core_count, 
    i32 *out_package_count)
{
    ia_defer_begin;
    static i32 thread_count, core_count, package_count = 0;
    ia_defer({
        if(out_thread_count)  *out_thread_count  = thread_count;
        if(out_core_count)    *out_core_count    = core_count;
        if(out_package_count) *out_package_count = package_count;
    });
    if (package_count != 0)
        ia_defer_return; /* query only once per runtime */
    thread_count = 1;
    core_count = 1;
    package_count = 1;

    i32 fd, len, pos = 0, end;
    char buf[4096];
    char num[101];
    i32 numsize = sizeof(num) - 1;
    char const *err = "Failed parsing /proc/cpuinfo.";

    fd = open("/proc/cpuinfo", O_RDONLY);
    if (fd == -1) {
        ia_error("%s", err);
        core_count = ia_max(1, sysconf(_SC_NPROCESSORS_CONF));
        thread_count = 2 * core_count;
        ia_defer_return;
    }
    len = read(fd, buf, 4096);
    close(fd);

    while (pos < len) {
        if (!strncmp(buf + pos, "cpu cores", 9)) {
            pos = strchr(buf + pos, ':') - buf + 2;
            end = strchr(buf + pos, '\n') - buf;
            
            if (pos < len && end < len) {
                strncpy(num, buf + pos, numsize);
                num[numsize - 1] = 0; /* [-Wstringop-truncation] */
                ia_assert((end - pos) > 0 && (end - pos) < numsize, "CPU num buffer too small.");
                num[end - pos] = '\0';

                i32 processor = atoi(num);
                if ((processor) > core_count)
                    core_count = processor;
            } else {
                ia_error("%s", err);
                break;
            }
        } else if (strncmp(buf + pos, "siblings", 8)) {
            pos = strchr(buf + pos, ':') - buf + 2;
            end = strchr(buf + pos, '\n') - buf;

            if (pos < len && end < len) {
                strncpy(num, buf + pos, numsize);
                num[numsize - 1] = 0; /* [-Wstringop-truncation] */
                ia_assert((end - pos) > 0 && (end - pos) < numsize, "CPU num buffer too small.");
                num[end - pos] = '\0';

                i32 core_id = atoi(num);
                if ((core_id) > thread_count)
                    thread_count = core_id;
            } else {
                ia_error("%s", err);
                break;
            }
        }
        pos = strchr(buf + pos, '\n') - buf + 1;
    }
    ia_defer_return;
}

ia_hugepage_sizes ia_hugetlbinfo(usize *out_total_ram)
{
    static usize total_ram = 0;
    static ia_hugepage_sizes sizes = 0;

    if (sizes || total_ram) {
        if (out_total_ram) *out_total_ram = total_ram;
        return sizes;
    }
    DIR *dir;
    struct dirent *entry;
    isize count, page;

    /* total ram */
    count = sysconf(_SC_PHYS_PAGES);
    if (count == -1)
        ia_error("sysconf _SC_PHYS_PAGES failed.");

    page = sysconf(_SC_PAGE_SIZE);
    if (page == -1)
        ia_error("sysconf _SC_PAGE_SIZE failed.");

    total_ram = ia_max(4096, page * count);
    sizes |= ia_hugepage_size_from_bytes((usize)ia_max(4096, page));
    IA_ASSUME(sizes != ia_hugepage_size_none);

    /* hugetlb */
    dir = opendir("/sys/kernel/mm/hugepages/");
    if (dir) {
        while ((entry = readdir(dir))) {
            char *name = entry->d_name;
            char *end;

            if (strncmp(name, "hugepages-", 10) != 0)
                continue;
            name += 10;

            count = strtol(name, &end, 10);
            if (*end == 'k' && *(end + 1) == 'B')
                sizes |= ia_hugepage_size_from_bytes((usize)(count << 10));
        }
        closedir(dir);
    }
    if (out_total_ram) *out_total_ram = total_ram;
    return sizes;
}

#endif /* IA_PLATFORM_LINUX */
