#include "common.h"
#include "util.h"

void format_bytes(unsigned long long bytes, char *buf, size_t len) {
    if (bytes >= (1ULL << 30))
        snprintf(buf, len, "%.2f GB", (double)bytes / (1ULL << 30));
    else if (bytes >= (1ULL << 20))
        snprintf(buf, len, "%.1f MB", (double)bytes / (1ULL << 20));
    else
        snprintf(buf, len, "%llu KB", bytes >> 10);
}

void print_separator(void) {
    printf(C_DIM "  ─────────────────────────────────────────" C_RESET "\n");
}
