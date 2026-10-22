/*
 * counter.c - 4-Bit Synchronous Binary Counter FSM
 */

#include "counter.h"
#include <stdio.h>
#include <string.h>

/* Decode integer value into 4-bit registers */
static void load_registers(CounterFSM *ctr, unsigned int val)
{
    ctr->value  = val & 0x0Fu;
    ctr->reg.q0 = (val >> 0) & 1u;
    ctr->reg.q1 = (val >> 1) & 1u;
    ctr->reg.q2 = (val >> 2) & 1u;
    ctr->reg.q3 = (val >> 3) & 1u;
}

int counter_init(CounterFSM *ctr, unsigned int start_value, counter_mode_t mode)
{
    if (!ctr) return -1;

    memset(ctr, 0, sizeof(*ctr));
    ctr->mode  = mode;
    ctr->cycle = 0;

    int ret = 0;
    if (start_value > 15u) {
        printf("  [COUNTER] Warning: start value %u out of range (0-15). Clamping to 0.\n",
               start_value);
        start_value = 0;
        ret = -1;
    }

    load_registers(ctr, start_value);
    return ret;
}

int counter_clock_tick_up(CounterFSM *ctr)
{
    if (!ctr) return 0;

    ctr->cycle++;
    int rollover = (ctr->value == 15u);
    unsigned int next = (ctr->value + 1u) & 0x0Fu;
    load_registers(ctr, next);
    return rollover;
}

int counter_clock_tick_down(CounterFSM *ctr)
{
    if (!ctr) return 0;

    ctr->cycle++;
    int rollover = (ctr->value == 0u);
    unsigned int next = (ctr->value - 1u) & 0x0Fu;
    load_registers(ctr, next);
    return rollover;
}

void counter_print_header(void)
{
    printf("  +-------+-------+----+----+----+----+-------------------+\n");
    printf("  | Cycle |  Dec  | Q3 | Q2 | Q1 | Q0 |      Status       |\n");
    printf("  +-------+-------+----+----+----+----+-------------------+\n");
}

void counter_print_trace(const CounterFSM *ctr, int rollover_flag, counter_mode_t mode)
{
    if (!ctr) return;

    const char *status = "";
    if (rollover_flag) {
        status = (mode == COUNTER_MODE_DOWN) ? "0->15 ROLLOVER" : "15->0 ROLLOVER";
    } else if (ctr->value == 15u && mode != COUNTER_MODE_DOWN) {
        status = "MAX";
    } else if (ctr->value == 0u && mode == COUNTER_MODE_DOWN && ctr->cycle == 0u) {
        status = "INIT";
    }

    printf("  |  %3u  |  %3u  |  %u |  %u |  %u |  %u | %-17s |\n",
           ctr->cycle,
           ctr->value,
           (unsigned)ctr->reg.q3,
           (unsigned)ctr->reg.q2,
           (unsigned)ctr->reg.q1,
           (unsigned)ctr->reg.q0,
           status);
}

void counter_print_waveform(int total_cycles)
{
    printf("\n  Simulated Clock Waveform (ASCII):\n\n");

    /* Clock pulse signal */
    printf("  CLK : ");
    for (int i = 0; i < total_cycles; i++) {
        printf("_|~");
    }
    printf("_\n");

    /* Register waveforms based on frequency division periods */
    const char *labels[] = { "Q0", "Q1", "Q2", "Q3" };
    int periods[]        = {  1,    2,    4,    8   };

    for (int bit = 0; bit < 4; bit++) {
        int state = 0;
        printf("  %s  : ", labels[bit]);
        for (int i = 0; i <= total_cycles; i++) {
            if (i > 0 && (i % periods[bit]) == 0) {
                state = !state;
            }
            printf("%s", state ? "~~" : "__");
        }
        printf("\n");
    }

    printf("\n  Legend: __ = LOW (0)   ~~ = HIGH (1)   _|~ = rising clock edge\n");
}
