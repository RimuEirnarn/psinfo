#include "common.h"
#include "info_system.h"

void print_header(void) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    const char *days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char *months[] = {"","Jan","Feb","Mar","Apr","May","Jun",
                             "Jul","Aug","Sep","Oct","Nov","Dec"};

    printf("\n");
    printf(C_MAGENTA C_BOLD
           "  ╔══════════════════════════════════════════╗\n"
           "  ║        Luna/Rimu  ·  System Snapshot     ║\n"
           "  ╚══════════════════════════════════════════╝"
           C_RESET "\n\n");

    printf("  " LABEL("Date / Time") "  %s %02d %s %04d  %02d:%02d:%02d\n",
           days[st.wDayOfWeek],
           st.wDay, months[st.wMonth], st.wYear,
           st.wHour, st.wMinute, st.wSecond);
}

void print_identity(void) {
    char hostname[256] = {0};
    char username[256] = {0};
    DWORD hlen = sizeof(hostname);
    DWORD ulen = sizeof(username);
    GetComputerNameA(hostname, &hlen);
    GetUserNameA(username, &ulen);

    printf("  " LABEL("Host") "         %s\n", hostname);
    printf("  " LABEL("User") "         %s\n", username);
}

void print_os(void) {
    HKEY hKey;
    char osName[256] = "Unknown";
    char osBuild[64] = "";
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

    printf("  " LABEL("OS") "           %s  " DIM("(build %s)") "\n",
           osName, osBuild);
}

void print_uptime(void) {
    ULONGLONG ms   = GetTickCount64();
    ULONGLONG s    = ms / 1000;
    unsigned days  = (unsigned)(s / 86400);
    unsigned hours = (unsigned)((s % 86400) / 3600);
    unsigned mins  = (unsigned)((s % 3600) / 60);

    if (days > 0)
        printf("  " LABEL("Uptime") "       %ud %02uh %02um\n", days, hours, mins);
    else
        printf("  " LABEL("Uptime") "       %02uh %02um\n", hours, mins);
}
