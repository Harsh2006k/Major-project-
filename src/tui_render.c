/*
 * tui_render.c - Terminal UI Renderer & CSV Parser
 */

#include "tui_render.h"
#include <stdio.h>
#include <string.h>

void tui_print_banner(const char *title)
{
    printf("\n" ANSI_BOLD ANSI_CYAN
           "  ======================================================\n"
           "   %-50s  \n"
           "  ======================================================\n"
           ANSI_RESET, title);
}

void tui_print_separator(void)
{
    printf(ANSI_CYAN "  ------------------------------------------------------\n" ANSI_RESET);
}

void tui_render_traffic_light(const tl_context_t *ctx)
{
    if (!ctx) return;

    tl_state_t state = tl_get_state(ctx);
    const char *color = ANSI_RESET;
    const char *label = "## ????  ##";
    unsigned int dur = 0;

    switch (state) {
        case TL_RED:
            color = ANSI_RED;
            label = "## STOP  ##";
            dur   = ctx->durations.red_s;
            break;
        case TL_GREEN:
            color = ANSI_GREEN;
            label = "##  GO   ##";
            dur   = ctx->durations.green_s;
            break;
        case TL_YELLOW:
            color = ANSI_YELLOW;
            label = "## SLOW  ##";
            dur   = ctx->durations.yellow_s;
            break;
        case TL_DEAD:
            color = ANSI_BOLD;
            label = "## FAULT ##";
            dur   = 0;
            break;
    }

    unsigned int remaining = (dur > ctx->elapsed) ? (dur - ctx->elapsed) : 0u;

    printf("    +-------------+\n");
    printf("    | %s%s%s |\n", color, label, ANSI_RESET);
    printf("    +-------------+\n");
    printf("    State: %s%-12s%s  Timer: %u/%u s\n",
           color, tl_state_name(state), ANSI_RESET, ctx->elapsed, dur);

    /* Render progress bar for timed states */
    if (dur > 0u) {
        unsigned int bar_width = 20u;
        unsigned int filled = (ctx->elapsed * bar_width) / dur;
        if (filled > bar_width) filled = bar_width;

        printf("    [");
        for (unsigned int i = 0u; i < bar_width; i++) {
            if (i < filled) {
                printf("%s#%s", color, ANSI_RESET);
            } else {
                printf(ANSI_DIM "." ANSI_RESET);
            }
        }
        printf("] %u s remaining\n", remaining);
    }
}

void tui_render_counter_row(const CounterFSM *ctr)
{
    counter_print_trace(ctr, 0, COUNTER_MODE_UP);
}

/* Strip leading and trailing whitespace characters in-place */
static void trim_inplace(char *str)
{
    if (!str) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start == ' ' || *start == '\t') {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    /* Trim trailing whitespace and newlines */
    size_t len = strlen(str);
    while (len > 0 &&
           (str[len - 1] == ' '  || str[len - 1] == '\t' ||
            str[len - 1] == '\r' || str[len - 1] == '\n')) {
        str[--len] = '\0';
    }
}

int load_external_fsm_csv(const char *filepath,
                          csv_transition_row_t *rows,
                          int max_rows)
{
    if (!filepath || !rows || max_rows <= 0) return -1;

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "  [CSV] Error: unable to open file '%s'\n", filepath);
        return -1;
    }

    char line[256];
    int count = 0;
    int header_skipped = 0;

    while (fgets(line, sizeof(line), fp) && count < max_rows) {
        trim_inplace(line);

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') continue;

        /* Skip header row */
        if (!header_skipped) {
            header_skipped = 1;
            continue;
        }

        /* Parse 4 comma-separated values */
        if (sscanf(line, "%31[^,],%31[^,],%31[^,],%31[^\n]",
                   rows[count].current_state,
                   rows[count].input_symbol,
                   rows[count].next_state,
                   rows[count].output_action) == 4) {

            trim_inplace(rows[count].current_state);
            trim_inplace(rows[count].input_symbol);
            trim_inplace(rows[count].next_state);
            trim_inplace(rows[count].output_action);

            count++;
        }
    }

    fclose(fp);
    return count;
}

void tui_print_csv_table(const csv_transition_row_t *rows, int count)
{
    if (!rows || count <= 0) {
        printf("  (No transitions available.)\n");
        return;
    }

    printf("  +--------------+--------------+--------------+------------------+\n");
    printf("  | " ANSI_BOLD "CurrentState" ANSI_RESET
           " | " ANSI_BOLD "InputSymbol " ANSI_RESET
           " | " ANSI_BOLD " NextState  " ANSI_RESET
           " | " ANSI_BOLD " OutputAction   " ANSI_RESET " |\n");
    printf("  +--------------+--------------+--------------+------------------+\n");

    for (int i = 0; i < count; i++) {
        printf("  | " ANSI_CYAN "%-12s" ANSI_RESET
               " | %-12s | " ANSI_GREEN "%-12s" ANSI_RESET
               " | %-16s |\n",
               rows[i].current_state,
               rows[i].input_symbol,
               rows[i].next_state,
               rows[i].output_action);
    }

    printf("  +--------------+--------------+--------------+------------------+\n");
}
