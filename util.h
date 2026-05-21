#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Format a byte count into a human-readable string (KB/MB/GB). */
void format_bytes(unsigned long long bytes, char *buf, size_t len);

/* Print a horizontal section separator. */
void print_separator(void);

#endif /* UTIL_H */
