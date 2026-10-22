/*
 * traffic_light.h - Traffic Light Controller FSM
 *
 * States: RED -> GREEN -> YELLOW -> RED
 * Features: Emergency vehicle override, fault injection, and dead-state recovery.
 */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "fsm_engine.h"

typedef enum {
    TL_RED    = 0,
    TL_GREEN  = 1,
    TL_YELLOW = 2,
    TL_DEAD   = 0x7F   /* Illegal/fault state sentinel */
} tl_state_t;

typedef struct {
    unsigned int red_s;
    unsigned int green_s;
    unsigned int yellow_s;
} tl_durations_t;

typedef struct {
    fsm_engine_t   engine;
    tl_durations_t durations;
    unsigned int   elapsed;
    int            emergency_pending;
} tl_context_t;

enum {
    TL_EVENT_NONE          = 0,
    TL_EVENT_TIMER_EXPIRED = 1,
    TL_EVENT_EMERGENCY     = 2,
    TL_EVENT_FAULT_INJECT  = 3
};

/* Initialize traffic light FSM with state durations; starts at RED */
void tl_init(tl_context_t *ctx, const tl_durations_t *durations);

/* Advance FSM by one clock tick. Returns 0 on normal step, -2 if in dead state */
int tl_tick(tl_context_t *ctx);

/* Queue an emergency vehicle override (forces immediate GREEN on next tick) */
void tl_emergency_override(tl_context_t *ctx);

/* Inject fault to test error handling (sets state to 0x7F) */
void tl_inject_fault(tl_context_t *ctx);

/* Recover from dead state back to safe RED state. Returns 1 if recovered, else 0 */
int tl_recover_if_dead(tl_context_t *ctx);

/* Return human-readable name of state */
const char *tl_state_name(tl_state_t state);

/* Return current state */
tl_state_t tl_get_state(const tl_context_t *ctx);

#endif /* TRAFFIC_LIGHT_H */
