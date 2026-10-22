/*
 * traffic_light.c - Traffic Light Controller FSM Implementation
 */

#include "traffic_light.h"
#include <stdio.h>
#include <string.h>

/* Transition table for traffic light states */
static const fsm_transition_t tl_table[] = {
    /* Normal cyclic transitions */
    { TL_RED,    TL_EVENT_TIMER_EXPIRED, TL_GREEN  },
    { TL_GREEN,  TL_EVENT_TIMER_EXPIRED, TL_YELLOW },
    { TL_YELLOW, TL_EVENT_TIMER_EXPIRED, TL_RED    },

    /* Emergency override: immediate transition to GREEN from any active state */
    { TL_RED,    TL_EVENT_EMERGENCY,     TL_GREEN  },
    { TL_GREEN,  TL_EVENT_EMERGENCY,     TL_GREEN  },
    { TL_YELLOW, TL_EVENT_EMERGENCY,     TL_GREEN  },
};

#define TL_TABLE_LEN (sizeof(tl_table) / sizeof(tl_table[0]))

/* Reset state elapsed timer upon entering a new state */
static void tl_entry_hook(void *context)
{
    tl_context_t *ctx = (tl_context_t *)context;
    ctx->elapsed = 0;
}

/* Get configured duration for current state */
static unsigned int tl_get_duration(const tl_context_t *ctx)
{
    switch ((tl_state_t)ctx->engine.current_state) {
        case TL_RED:    return ctx->durations.red_s;
        case TL_GREEN:  return ctx->durations.green_s;
        case TL_YELLOW: return ctx->durations.yellow_s;
        case TL_DEAD:   return 0;
    }
    return 0;
}

void tl_init(tl_context_t *ctx, const tl_durations_t *durations)
{
    if (!ctx || !durations) return;

    memset(ctx, 0, sizeof(*ctx));
    ctx->durations = *durations;

    /* Configure embedded FSM engine */
    ctx->engine.transitions      = tl_table;
    ctx->engine.transition_count = TL_TABLE_LEN;
    ctx->engine.dead_state       = TL_DEAD;
    ctx->engine.entry_hook       = tl_entry_hook;
    ctx->engine.exit_hook        = NULL;
    ctx->engine.user_context     = ctx;

    fsm_init(&ctx->engine, TL_RED);
}

int tl_tick(tl_context_t *ctx)
{
    if (!ctx) return -1;

    /* Guard against stepping while dead state is unrecovered */
    if (fsm_is_dead(&ctx->engine)) {
        return -2;
    }

    /* Process pending emergency override with priority */
    if (ctx->emergency_pending) {
        ctx->emergency_pending = 0;
        printf("  [TRAFFIC] Emergency override active -> switching to GREEN\n");
        return fsm_step(&ctx->engine, TL_EVENT_EMERGENCY);
    }

    ctx->elapsed++;

    /* Check if current state duration has expired */
    unsigned int duration = tl_get_duration(ctx);
    if (duration > 0 && ctx->elapsed >= duration) {
        return fsm_step(&ctx->engine, TL_EVENT_TIMER_EXPIRED);
    }

    return 0;
}

void tl_emergency_override(tl_context_t *ctx)
{
    if (!ctx) return;
    ctx->emergency_pending = 1;
    printf("  [TRAFFIC] Emergency vehicle override requested\n");
}

void tl_inject_fault(tl_context_t *ctx)
{
    if (!ctx) return;
    printf("  [TRAFFIC] Corrupting state to illegal value (0x7F)...\n");
    fsm_inject_fault(&ctx->engine, TL_DEAD);
}

int tl_recover_if_dead(tl_context_t *ctx)
{
    if (!ctx) return -1;

    if (fsm_is_dead(&ctx->engine)) {
        printf("  [TRAFFIC] \033[1;31m*** DEAD-STATE DETECTED (0x%X) ***\033[0m\n",
               ctx->engine.current_state);
        printf("  [TRAFFIC] \033[1;32mSafety engine: restoring safe state RED\033[0m\n");
        fsm_reset(&ctx->engine, TL_RED);
        return 1;
    }
    return 0;
}

const char *tl_state_name(tl_state_t state)
{
    switch (state) {
        case TL_RED:    return "RED";
        case TL_GREEN:  return "GREEN";
        case TL_YELLOW: return "YELLOW";
        case TL_DEAD:   return "DEAD(0x7F)";
    }
    return "UNKNOWN";
}

tl_state_t tl_get_state(const tl_context_t *ctx)
{
    if (!ctx) return TL_DEAD;
    return (tl_state_t)ctx->engine.current_state;
}
