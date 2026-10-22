/*
 * tui_render.h - Terminal UI Renderer & CSV Parser
 *
 * Provides visual rendering helpers and CSV transition table ingestion
 * for Worklet 2 integration.
 */

#ifndef TUI_RENDER_H
#define TUI_RENDER_H

#include "traffic_light.h"
#include "counter.h"

/* ANSI formatting codes */
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_DIM     "\033[2m"
#define ANSI_RESET   "\033[0m"

#define MAX_CSV_ROWS    64
#define MAX_FIELD_LEN   32

/* Transition row structure matching Worklet 2 CSV table format */
typedef struct {
    char current_state[MAX_FIELD_LEN];
    char input_symbol[MAX_FIELD_LEN];
    char next_state[MAX_FIELD_LEN];
    char output_action[MAX_FIELD_LEN];
} csv_transition_row_t;

/* UI banner and separator helpers */
void tui_print_banner(const char *title);
void tui_print_separator(void);

/* Render traffic light signal, state, and timer progress bar */
void tui_render_traffic_light(const tl_context_t *ctx);

/* Render one counter trace row */
void tui_render_counter_row(const CounterFSM *ctr);

/*
 * Load external transition table from CSV file.
 * Returns number of loaded rows, or -1 on failure.
 */
int  load_external_fsm_csv(const char *filepath,
                           csv_transition_row_t *rows,
                           int max_rows);

/* Print tabular view of loaded CSV transition rows */
void tui_print_csv_table(const csv_transition_row_t *rows, int count);

#endif /* TUI_RENDER_H */
