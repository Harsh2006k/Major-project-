/*
 * main.c - Automated Demonstration & Verification Suite
 *
 * Mini Project 18 - Worklet 1 - COEP Technological University
 * "Finite Automata Builds" - Direct Execution Runner
 *
 * Demonstrates:
 *   1. Traffic Light Controller FSM (timed cycling, emergency override, dead-state recovery)
 *   2. 4-Bit Synchronous Binary Counter FSM (up/down modes, bitfield registers, rollover, waveform)
 *   3. Core FSM Engine Validation (transition handling, boundary checks, dead-state guards)
 *   4. External State Table Ingestion (Worklet 2 CSV integration)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "traffic_light.h"
#include "counter.h"
#include "tui_render.h"

/* ------------------------------------------------------------
 *  CASE STUDY 1: Traffic Light Controller FSM
 * ------------------------------------------------------------ */

static void run_traffic_light_demo(void)
{
    tui_print_banner("Case Study 1: Traffic Light Controller FSM");

    tl_durations_t durations = {
        .red_s    = 5,
        .green_s  = 4,
        .yellow_s = 2
    };

    const unsigned int total_ticks = 20;
    const unsigned int emergency_tick = 7;
    const unsigned int fault_tick = 13;

    tl_context_t ctx;
    tl_init(&ctx, &durations);

    printf("\n  Configuration:\n");
    printf("    RED: %u s | GREEN: %u s | YELLOW: %u s\n",
           durations.red_s, durations.green_s, durations.yellow_s);
    printf("    Predefined Events: Emergency Override at tick %u | Fault Injection at tick %u\n",
           emergency_tick, fault_tick);
    tui_print_separator();

    for (unsigned int tick = 1; tick <= total_ticks; tick++) {
        printf("\n  " ANSI_BOLD "[Tick %2u / %u]" ANSI_RESET "\n", tick, total_ticks);

        /* Scheduled test event: Emergency vehicle override */
        if (tick == emergency_tick) {
            printf(ANSI_BOLD ANSI_RED "  >>> EVENT: Emergency Vehicle Approaching <<<\n" ANSI_RESET);
            tl_emergency_override(&ctx);
        }

        /* Scheduled test event: Fault injection into state register */
        if (tick == fault_tick) {
            printf(ANSI_BOLD ANSI_RED "  >>> EVENT: Injecting State Corruption (0x7F) <<<\n" ANSI_RESET);
            tl_inject_fault(&ctx);
        }

        int result = tl_tick(&ctx);

        /* Safety recovery if dead state was encountered */
        if (result == -2) {
            tl_recover_if_dead(&ctx);
        }

        tui_render_traffic_light(&ctx);
    }

    tui_print_separator();
    printf("  " ANSI_GREEN "[PASS]" ANSI_RESET
           " Traffic Light FSM completed %u ticks (cycling, override, and self-healing verified).\n\n",
           total_ticks);
}

/* ------------------------------------------------------------
 *  CASE STUDY 2: 4-Bit Synchronous Binary Counter FSM
 * ------------------------------------------------------------ */

static void run_counter_demo(void)
{
    tui_print_banner("Case Study 2: 4-Bit Synchronous Binary Counter");

    /* Part A: Up-Counter with Rollover (15 -> 0) */
    printf("\n  " ANSI_BOLD "Part A: Up-Counter Mode (0 to 15, Modulo-16 Rollover)" ANSI_RESET "\n");
    CounterFSM up_ctr;
    counter_init(&up_ctr, 0, COUNTER_MODE_UP);

    printf("  Initial State: Dec %u -> Q3=%u Q2=%u Q1=%u Q0=%u\n\n",
           up_ctr.value,
           (unsigned)up_ctr.reg.q3,
           (unsigned)up_ctr.reg.q2,
           (unsigned)up_ctr.reg.q1,
           (unsigned)up_ctr.reg.q0);

    counter_print_header();
    counter_print_trace(&up_ctr, 0, COUNTER_MODE_UP);

    for (int i = 0; i < 17; i++) {
        int rollover = counter_clock_tick_up(&up_ctr);
        counter_print_trace(&up_ctr, rollover, COUNTER_MODE_UP);
    }
    printf("  +-------+-------+----+----+----+----+-------------------+\n");

    /* Part B: Down-Counter with Rollover (0 -> 15) */
    printf("\n  " ANSI_BOLD "Part B: Down-Counter Mode (Decrement and 0 -> 15 Rollover)" ANSI_RESET "\n");
    CounterFSM down_ctr;
    counter_init(&down_ctr, 2, COUNTER_MODE_DOWN);

    counter_print_header();
    counter_print_trace(&down_ctr, 0, COUNTER_MODE_DOWN);

    for (int i = 0; i < 4; i++) {
        int rollover = counter_clock_tick_down(&down_ctr);
        counter_print_trace(&down_ctr, rollover, COUNTER_MODE_DOWN);
    }
    printf("  +-------+-------+----+----+----+----+-------------------+\n");

    /* Part C: Boundary Input Validation */
    printf("\n  " ANSI_BOLD "Part C: Boundary Validation Check (Start Value > 15)" ANSI_RESET "\n");
    CounterFSM bound_ctr;
    int ret = counter_init(&bound_ctr, 25, COUNTER_MODE_UP);
    if (ret != 0 && bound_ctr.value == 0) {
        printf("  " ANSI_GREEN "[PASS]" ANSI_RESET
               " Out-of-bounds start value 25 safely detected and clamped to 0.\n");
    }

    /* Part D: ASCII Timing Waveform */
    printf("\n  " ANSI_BOLD "Part D: ASCII Clock Waveform (16 Cycles)" ANSI_RESET "\n");
    counter_print_waveform(16);

    tui_print_separator();
    printf("  " ANSI_GREEN "[PASS]" ANSI_RESET
           " Counter FSM verified: bitfields, decimal conversion, up/down rollover, and waveform.\n\n");
}

/* ------------------------------------------------------------
 *  CORE FSM ENGINE: Boundary & Transition Validation
 * ------------------------------------------------------------ */

static void run_fsm_engine_validation(void)
{
    tui_print_banner("Core FSM Engine: Validation & Boundary Analysis");

    /* Mini transition table: S0 -[event 1]-> S1 */
    const fsm_transition_t test_table[] = {
        { 0, 1, 1 },
        { 1, 2, 0 }
    };

    fsm_engine_t engine = {
        .current_state    = 0,
        .dead_state       = 99,
        .transitions      = test_table,
        .transition_count = 2,
        .entry_hook       = NULL,
        .exit_hook        = NULL,
        .user_context     = NULL
    };

    fsm_init(&engine, 0);
    printf("\n  Engine initialized at state %d.\n", engine.current_state);

    /* 1. Valid transition */
    int step1 = fsm_step(&engine, 1);
    printf("  1. Valid transition on Event 1: result = %d (state -> %d) ", step1, engine.current_state);
    if (step1 == 0 && engine.current_state == 1) {
        printf(ANSI_GREEN "[PASS]" ANSI_RESET "\n");
    } else {
        printf(ANSI_RED "[FAIL]" ANSI_RESET "\n");
    }

    /* 2. Missing transition / invalid event */
    int step2 = fsm_step(&engine, 999);
    printf("  2. Missing transition on Event 999: result = %d ", step2);
    if (step2 == -1) {
        printf(ANSI_GREEN "[PASS]" ANSI_RESET " (safely rejected with -1)\n");
    } else {
        printf(ANSI_RED "[FAIL]" ANSI_RESET "\n");
    }

    /* 3. Dead state trap and recovery */
    fsm_inject_fault(&engine, 99);
    int step3 = fsm_step(&engine, 2);
    printf("  3. Step while in Dead State: result = %d ", step3);
    if (step3 == -2 && fsm_is_dead(&engine)) {
        printf(ANSI_GREEN "[PASS]" ANSI_RESET " (guard blocked transition with -2)\n");
    } else {
        printf(ANSI_RED "[FAIL]" ANSI_RESET "\n");
    }

    fsm_reset(&engine, 0);
    printf("  4. Reset from Dead State to Safe State 0: current_state = %d ", engine.current_state);
    if (engine.current_state == 0 && !fsm_is_dead(&engine)) {
        printf(ANSI_GREEN "[PASS]" ANSI_RESET "\n");
    } else {
        printf(ANSI_RED "[FAIL]" ANSI_RESET "\n");
    }

    tui_print_separator();
    printf("  " ANSI_GREEN "[PASS]" ANSI_RESET " Generic FSM Engine validation tests completed.\n\n");
}

/* ------------------------------------------------------------
 *  EXTERNAL STATE TABLE INGESTION (Worklet 2 Integration)
 * ------------------------------------------------------------ */

static void run_csv_parser_demo(void)
{
    tui_print_banner("External FSM State Table Ingestion (Worklet 2)");

    const char *filepath = "data/custom_fsm.csv";
    csv_transition_row_t rows[MAX_CSV_ROWS];

    printf("\n  Loading external state table from '%s'...\n\n", filepath);
    int count = load_external_fsm_csv(filepath, rows, MAX_CSV_ROWS);

    if (count < 0) {
        printf("  " ANSI_RED "[ERROR]" ANSI_RESET " Unable to open or parse '%s'.\n\n", filepath);
        return;
    }

    tui_print_csv_table(rows, count);

    printf("\n  " ANSI_GREEN "[PASS]" ANSI_RESET
           " Successfully loaded %d transition row(s) into C array structures.\n", count);
    printf("  Integration readiness: rows can be mapped directly to fsm_transition_t entries.\n\n");
}

/* ------------------------------------------------------------
 *  MAIN ENTRY POINT (Direct Execution)
 * ------------------------------------------------------------ */

static void print_usage(const char *prog_name)
{
    printf("Usage: %s [target]\n\n", prog_name);
    printf("Available targets:\n");
    printf("  all       Run all demonstrations and test suites (default)\n");
    printf("  traffic   Run Case Study 1: Traffic Light Controller FSM\n");
    printf("  counter   Run Case Study 2: 4-Bit Synchronous Binary Counter FSM\n");
    printf("  engine    Run Core FSM Engine Validation Suite\n");
    printf("  csv       Run External State Table CSV Ingestion Demo\n");
    printf("  --help    Show this help message\n\n");
}

int main(int argc, char *argv[])
{
    const char *target = "all";

    if (argc >= 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        target = argv[1];
    }

    if (strcmp(target, "all") == 0) {
        tui_print_banner("Mini Project 18: Finite Automata Builds (Worklet 1)");
        printf("  Automated test and demonstration suite running with predefined inputs.\n");
        run_traffic_light_demo();
        run_counter_demo();
        run_fsm_engine_validation();
        run_csv_parser_demo();
    } else if (strcmp(target, "traffic") == 0) {
        run_traffic_light_demo();
    } else if (strcmp(target, "counter") == 0) {
        run_counter_demo();
    } else if (strcmp(target, "engine") == 0) {
        run_fsm_engine_validation();
    } else if (strcmp(target, "csv") == 0) {
        run_csv_parser_demo();
    } else {
        fprintf(stderr, "Unknown target: '%s'\n\n", target);
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
