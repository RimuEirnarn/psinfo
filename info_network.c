#include "common.h"
#include "util.h"
#include "info_network.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

void print_network(void) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    print_separator();
    printf("  " LABEL("Network") "\n");

    ULONG bufLen = 15000;
    IP_ADAPTER_ADDRESSES *addrs = NULL;
    ULONG ret;

    /* Retry with growing buffer if needed */
    for (int attempts = 0; attempts < 3; attempts++) {
        addrs = (IP_ADAPTER_ADDRESSES *)malloc(bufLen);
        if (!addrs) break;
        ret = GetAdaptersAddresses(
                AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                GAA_FLAG_SKIP_DNS_SERVER,
                NULL, addrs, &bufLen);
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

        char fname[256] = {0};
        WideCharToMultiByte(CP_ACP, 0, a->FriendlyName, -1,
                            fname, sizeof(fname) - 1, NULL, NULL);

        int printed_header = 0;

        for (IP_ADAPTER_UNICAST_ADDRESS *ua = a->FirstUnicastAddress;
             ua; ua = ua->Next)
        {
            SOCKADDR *sa = ua->Address.lpSockaddr;
            char ipstr[INET6_ADDRSTRLEN] = {0};

            if (sa->sa_family == AF_INET) {
                inet_ntop(AF_INET,
                          &((struct sockaddr_in *)sa)->sin_addr,
                          ipstr, sizeof(ipstr));
            } else if (sa->sa_family == AF_INET6) {
                struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)sa;
                /* Skip link-local (fe80::) */
                if (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr)) continue;
                inet_ntop(AF_INET6, &s6->sin6_addr, ipstr, sizeof(ipstr));
            } else {
                continue;
            }

            if (!printed_header) {
                printf("    " C_BLUE C_BOLD "%-32s" C_RESET "\n", fname);
                printed_header = 1;
            }
            printf("      " C_WHITE "%-40s" C_RESET "\n", ipstr);
        }
    }

    free(addrs);
    WSACleanup();
}
