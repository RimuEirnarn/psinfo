#include "common.h"
#include "util.h"
#include "info_system.h"
#include "info_cpu.h"
#include "info_memory.h"
#include "info_disk.h"
#include "info_network.h"
#include "info_gpu.h"

int main(void) {
    /* Enable ANSI escape codes in the Windows console (Windows 10+) */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode  = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    print_header();

    print_separator();
    print_identity();
    print_os();
    print_uptime();

    print_cpu();
    print_memory();
    print_disks();
    print_gpu();
    print_network();

    printf("\n");
    return 0;
}
