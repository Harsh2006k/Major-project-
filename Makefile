# ═══════════════════════════════════════════════════════════
# Makefile – Mini Project 18 · Finite Automata Builds (W1)
# COEP Technological University · CSE Mini Project Leap
# ═══════════════════════════════════════════════════════════

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -Iinc
TARGET   = fsm_sim

SRCDIR   = src
INCDIR   = inc

SOURCES  = $(SRCDIR)/main.c          \
           $(SRCDIR)/fsm_engine.c    \
           $(SRCDIR)/traffic_light.c \
           $(SRCDIR)/counter.c       \
           $(SRCDIR)/tui_render.c

# ─── Targets ────────────────────────────────────────────────

all: $(TARGET)
	@echo "[BUILD] $(TARGET) compiled successfully."

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

ifeq ($(OS),Windows_NT)
    CLEAN_CMD = del /Q /F $(TARGET).exe 2>nul || exit 0
else
    CLEAN_CMD = $(RM) $(TARGET) $(TARGET).exe
endif

clean:
	@$(CLEAN_CMD)

.PHONY: all clean run
