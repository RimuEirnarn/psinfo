#include "common.h"
#include "util.h"
#include "info_memory.h"

void print_memory(void) {
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);

    char used_s[32], total_s[32];
    unsigned long long used = ms.ullTotalPhys - ms.ullAvailPhys;
    format_bytes(used,            used_s,  sizeof(used_s));
    format_bytes(ms.ullTotalPhys, total_s, sizeof(total_s));

    int pct    = (int)(100.0 * used / ms.ullTotalPhys);
    int filled = pct / 5;

    char bar[21] = {0};
    for (int i = 0; i < 20; i++)
        bar[i] = (i < filled) ? '#' : '-';

    const char *colour = (pct < 60) ? C_GREEN : (pct < 85) ? C_YELLOW : C_RED;

    print_separator();
    printf("  " LABEL("RAM") "          %s / %s  %s[%s]" C_RESET " %d%%\n",
           used_s, total_s, colour, bar, pct);
}
