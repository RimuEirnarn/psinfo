#ifndef INFO_SYSTEM_H
#define INFO_SYSTEM_H

/* Prints the banner + current date/time. */
void print_header(void);

/* Prints hostname and current username. */
void print_identity(void);

/* Prints Windows product name and build number from registry. */
void print_os(void);

/* Prints system uptime as days/hours/minutes. */
void print_uptime(void);

#endif /* INFO_SYSTEM_H */
