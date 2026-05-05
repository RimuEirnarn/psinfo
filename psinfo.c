/*
 * psinfo.c — PowerShell boot info companion
 *
 * Displays a system snapshot at shell startup.
 * Compile: gcc psinfo.c -o psinfo.exe -lws2_32 -liphlpapi -lpdh
 * (MinGW/MSYS2 on Windows)
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "pdh.lib")

/* ── ANSI colour helpers ─────────────────────────────────────────────────── */
#define C_RESET   "\033[0m"
#define C_BOLD    "\033[1m"
#define C_DIM     "\033[2m"
#define C_CYAN    "\033[36m"
#define C_BLUE    "\033[34m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_MAGENTA "\033[35m"
#define C_RED     "\033[31m"
#define C_WHITE   "\033[97m"

#define LABEL(s)  C_CYAN C_BOLD s C_RESET
#define VAL(s)    C_WHITE s C_RESET
#define DIM(s)    C_DIM s C_RESET

/* ── helpers ─────────────────────────────────────────────────────────────── */
static void print_separator(void) {
    printf(C_DIM "  ─────────────────────────────────────────" C_RESET "\n");
}

static void format_bytes(unsigned long long bytes, char *buf, size_t len) {
    if (bytes >= (1ULL << 30))
        snprintf(buf, len, "%.2f GB", (double)bytes / (1ULL << 30));
    else if (bytes >= (1ULL << 20))
        snprintf(buf, len, "%.1f MB", (double)bytes / (1ULL << 20));
    else
        snprintf(buf, len, "%llu KB", bytes >> 10);
}

/* ── section printers ────────────────────────────────────────────────────── */

static void print_header(void) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    const char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char *months[] = {"","Jan","Feb","Mar","Apr","May","Jun",
                             "Jul","Aug","Sep","Oct","Nov","Dec"};

    printf("\n");
    printf(C_MAGENTA C_BOLD
           "  ╔══════════════════════════════════════════╗\n"
           "  ║              System Snapshot             ║\n"
           "  ╚══════════════════════════════════════════╝"
           C_RESET "\n\n");

    printf("  " LABEL("Date / Time") "  %s %02d %s %04d  %02d:%02d:%02d\n",
           days[st.wDayOfWeek],
           st.wDay, months[st.wMonth], st.wYear,
           st.wHour, st.wMinute, st.wSecond);
}

static void print_identity(void) {
    char hostname[256] = {0};
    char username[256] = {0};
    DWORD hlen = sizeof(hostname);
    DWORD ulen = sizeof(username);
    GetComputerNameA(hostname, &hlen);
    GetUserNameA(username, &ulen);

    print_separator();
    printf("  " LABEL("Host") "        %s\n", hostname);
    printf("  " LABEL("User") "        %s\n", username);
}

static void print_os(void) {
    /* Read ProductName from registry */
    HKEY hKey;
    char osName[256]  = "Unknown";
    char osBuild[64]  = "";
    DWORD sz;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        sz = sizeof(osName);
        RegQueryValueExA(hKey, "ProductName",  NULL, NULL, (LPBYTE)osName,  &sz);
        sz = sizeof(osBuild);
        RegQueryValueExA(hKey, "CurrentBuild", NULL, NULL, (LPBYTE)osBuild, &sz);
        RegCloseKey(hKey);
    }

    printf("  " LABEL("OS") "          %s  " DIM("(build %s)") "\n",
           osName, osBuild);
}

static void print_uptime(void) {
    ULONGLONG ms = GetTickCount64();
    ULONGLONG s  = ms / 1000;
    unsigned days  = (unsigned)(s / 86400);
    unsigned hours = (unsigned)((s % 86400) / 3600);
    unsigned mins  = (unsigned)((s % 3600) / 60);

    if (days > 0)
        printf("  " LABEL("Uptime") "      %ud %02uh %02um\n", days, hours, mins);
    else
        printf("  " LABEL("Uptime") "      %02uh %02um\n", hours, mins);
}

static void print_cpu(void) {
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

    /* Trim leading spaces */
    char *p = cpuName;
    while (*p == ' ') p++;

    GetSystemInfo(&si);
    print_separator();
    printf("  " LABEL("CPU") "         %s\n", p);
    printf("  " LABEL("Cores") "       %lu logical\n",
           (unsigned long)si.dwNumberOfProcessors);
}

static void print_memory(void) {
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);

    char used_s[32], total_s[32];
    unsigned long long used = ms.ullTotalPhys - ms.ullAvailPhys;
    format_bytes(used,          used_s,  sizeof(used_s));
    format_bytes(ms.ullTotalPhys, total_s, sizeof(total_s));

    int pct = (int)(100.0 * used / ms.ullTotalPhys);
    /* Mini bar */
    char bar[21] = {0};
    int filled = pct / 5;
    for (int i = 0; i < 20; i++)
        bar[i] = (i < filled) ? '#' : '-';

    const char *colour = (pct < 60) ? C_GREEN : (pct < 85) ? C_YELLOW : C_RED;

    print_separator();
    printf("  " LABEL("RAM") "         %s / %s  %s[%s]" C_RESET " %d%%\n",
           used_s, total_s, colour, bar, pct);
}

static void print_disks(void) {
    print_separator();
    printf("  " LABEL("Disks") "\n");

    char drives[512] = {0};
    GetLogicalDriveStringsA(sizeof(drives)-1, drives);

    for (char *drv = drives; *drv; drv += strlen(drv)+1) {
        UINT type = GetDriveTypeA(drv);
        if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE) continue;

        ULARGE_INTEGER free_bytes, total_bytes, total_free;
        if (!GetDiskFreeSpaceExA(drv, &free_bytes, &total_bytes, &total_free))
            continue;

        char total_s[32], free_s[32];
        format_bytes(total_bytes.QuadPart, total_s, sizeof(total_s));
        format_bytes(free_bytes.QuadPart,  free_s,  sizeof(free_s));
        int pct_used = (int)(100.0 * (total_bytes.QuadPart - free_bytes.QuadPart)
                             / total_bytes.QuadPart);

        const char *colour = (pct_used < 70) ? C_GREEN
                           : (pct_used < 90) ? C_YELLOW : C_RED;
        printf("    %s%-3s" C_RESET "  %s free / %s total  %s%d%% used" C_RESET "\n",
               C_BLUE C_BOLD, drv, free_s, total_s, colour, pct_used);
    }
}

static void print_network(void) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    ULONG bufLen = 15000;
    IP_ADAPTER_ADDRESSES *addrs = NULL;
    ULONG ret;

    print_separator();
    printf("  " LABEL("Network") "\n");

    /* Retry loop in case buffer is too small */
    for (int attempts = 0; attempts < 3; attempts++) {
        addrs = (IP_ADAPTER_ADDRESSES *)malloc(bufLen);
        if (!addrs) break;
        ret = GetAdaptersAddresses(AF_UNSPEC,
              GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
              GAA_FLAG_SKIP_DNS_SERVER, NULL, addrs, &bufLen);
        if (ret == ERROR_SUCCESS) break;
        free(addrs); addrs = NULL;
        if (ret != ERROR_BUFFER_OVERFLOW) break;
    }

    if (!addrs) {
        printf("    (unavailable)\n");
        WSACleanup();
        return;
    }

    for (IP_ADAPTER_ADDRESSES *a = addrs; a; a = a->Next) {
        if (a->OperStatus != IfOperStatusUp) continue;
        if (a->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;

        /* Convert friendly name (wide) to narrow */
        char fname[256] = {0};
        WideCharToMultiByte(CP_ACP, 0, a->FriendlyName, -1,
                            fname, sizeof(fname)-1, NULL, NULL);

        int printed_header = 0;

        for (IP_ADAPTER_UNICAST_ADDRESS *ua = a->FirstUnicastAddress; ua; ua = ua->Next) {
            SOCKADDR *sa = ua->Address.lpSockaddr;
            char ipstr[INET6_ADDRSTRLEN] = {0};

            if (sa->sa_family == AF_INET) {
                inet_ntop(AF_INET,
                    &((struct sockaddr_in*)sa)->sin_addr,
                    ipstr, sizeof(ipstr));
            } else if (sa->sa_family == AF_INET6) {
                struct sockaddr_in6 *s6 = (struct sockaddr_in6*)sa;
                /* Skip link-local */
                if (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr)) continue;
                inet_ntop(AF_INET6, &s6->sin6_addr, ipstr, sizeof(ipstr));
            } else {
                continue;
            }

            if (!printed_header) {
                printf("    " C_BLUE C_BOLD "%-32s" C_RESET "\n", fname);
                printed_header = 1;
            }
            printf("      %s%-40s" C_RESET "\n", C_WHITE, ipstr);
        }
    }

    free(addrs);
    WSACleanup();
}

/* ── entry point ─────────────────────────────────────────────────────────── */
int main(void) {
    /* Enable ANSI escape codes in Windows console */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    print_header();
    print_identity();
    print_os();
    print_uptime();
    print_cpu();
    print_memory();
    print_disks();
    print_network();

    printf("\n");
    return 0;
}
