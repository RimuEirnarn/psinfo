#ifndef COMMON_H
#define COMMON_H

/* Target Windows Vista+ for inet_ntop, GetTickCount64, etc. */
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0600
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── ANSI colour macros ──────────────────────────────────────────────────── */
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
#define DIM(s)    C_DIM s C_RESET

#endif /* COMMON_H */
