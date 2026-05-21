#include "common.h"
#include "util.h"
#include "info_cpu.h"

void print_cpu(void) {
    HKEY hKey;
    char cpuName[256] = "Unknown";
    DWORD sz = sizeof(cpuName);
    SYSTEM_INFO si;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL,
                         (LPBYTE)cpuName, &sz);
        RegCloseKey(hKey);
    }

    /* Trim leading spaces that some OEMs embed */
    char *p = cpuName;
    while (*p == ' ') p++;

    GetSystemInfo(&si);
    print_separator();
    printf("  " LABEL("CPU") "          %s\n", p);
    printf("  " LABEL("Cores") "        %lu logical\n",
           (unsigned long)si.dwNumberOfProcessors);
}
