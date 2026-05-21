#include "common.h"
#include "util.h"
#include "info_disk.h"

void print_disks(void) {
    print_separator();
    printf("  " LABEL("Disks") "\n");

    char drives[512] = {0};
    GetLogicalDriveStringsA(sizeof(drives) - 1, drives);

    for (char *drv = drives; *drv; drv += strlen(drv) + 1) {
        UINT type = GetDriveTypeA(drv);
        if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE) continue;

        ULARGE_INTEGER free_bytes, total_bytes, total_free;
        if (!GetDiskFreeSpaceExA(drv, &free_bytes, &total_bytes, &total_free))
            continue;

        char total_s[32], free_s[32];
        format_bytes(total_bytes.QuadPart, total_s, sizeof(total_s));
        format_bytes(free_bytes.QuadPart,  free_s,  sizeof(free_s));

        int pct_used = (int)(100.0
                       * (total_bytes.QuadPart - free_bytes.QuadPart)
                       / total_bytes.QuadPart);

        const char *colour = (pct_used < 70) ? C_GREEN
                           : (pct_used < 90) ? C_YELLOW : C_RED;

        printf("    %s%-3s" C_RESET "  %s free / %s total  %s%d%% used" C_RESET "\n",
               C_BLUE C_BOLD, drv, free_s, total_s, colour, pct_used);
    }
}
