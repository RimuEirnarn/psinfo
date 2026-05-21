# ── psinfo Makefile ───────────────────────────────────────────────────────────
# Targets Windows Vista+ (for inet_ntop, GetTickCount64, etc.)
# Requires: MinGW-w64 / MSYS2 mingw-w64-x86_64-gcc
#
# Usage:
#   make          — build psinfo.exe
#   make clean    — remove build artefacts
#   make rebuild  — clean then build
# ─────────────────────────────────────────────────────────────────────────────

CC      := gcc
TARGET  := psinfo.exe
OBJDIR  := obj
PREFIX  := $(if $(RIMU_PREFIX),$(RIMU_PREFIX),$(PREFIX))

CFLAGS  := -std=c11 -Wall -Wextra -O2 -D_WIN32_WINNT=0x0600
LDFLAGS := -lws2_32 -liphlpapi -ldxgi

# ── source / object lists ─────────────────────────────────────────────────────
SRCS := main.c \
        util.c \
        info_system.c \
        info_cpu.c \
        info_memory.c \
        info_disk.c \
        info_network.c \
        info_gpu.c

OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# ── rules ─────────────────────────────────────────────────────────────────────
.PHONY: all clean rebuild, check-prefix

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

install: psinfo.exe
	cp psinfo.exe $(PREFIX)\usr\bin\psinfo.exe

check-prefix:
	printf "$(PREFIX)\n"

rebuild: clean all
