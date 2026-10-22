/*
 * counter.h - 4-Bit Synchronous Binary Counter FSM
 *
 * Models 4 flip-flop registers (Q3, Q2, Q1, Q0) using bit-fields.
 * Supports up-counting, down-counting, rollover handling, and trace display.
 */

#ifndef COUNTER_H
#define COUNTER_H

typedef enum {
    COUNTER_MODE_UP     = 1,
    COUNTER_MODE_DOWN   = 2,
    COUNTER_MODE_MANUAL = 3
} counter_mode_t;

/* 4-bit output flip-flop registers */
typedef struct {
    unsigned int q3 : 1; /* MSB (bit 3) */
    unsigned int q2 : 1; /* bit 2 */
    unsigned int q1 : 1; /* bit 1 */
    unsigned int q0 : 1; /* LSB (bit 0) */
} counter_register_t;

/* Counter state representation */
typedef struct {
    counter_register_t reg;
    unsigned int       cycle;
    unsigned int       value;
    counter_mode_t     mode;
} CounterFSM;

/* Initialize counter; clamps start values > 15 to 0 and returns -1 */
int  counter_init(CounterFSM *ctr, unsigned int start_value, counter_mode_t mode);

/* Clock tick: up-count modulo 16. Returns 1 on rollover (15 -> 0) */
int  counter_clock_tick_up(CounterFSM *ctr);

/* Clock tick: down-count modulo 16. Returns 1 on rollover (0 -> 15) */
int  counter_clock_tick_down(CounterFSM *ctr);

/* Print table header for trace output */
void counter_print_header(void);

/* Print one row of counter register values and status */
void counter_print_trace(const CounterFSM *ctr, int rollover_flag, counter_mode_t mode);

/* Render ASCII timing waveform for clock and output registers */
void counter_print_waveform(int total_cycles);

#endif /* COUNTER_H */
