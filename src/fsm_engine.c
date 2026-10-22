/*
 * fsm_engine.c - Generic FSM Engine Implementation
 */

#include "fsm_engine.h"
#include <stdio.h>

void fsm_init(fsm_engine_t *engine, fsm_state_t start_state)
{
    if (!engine) return;

    engine->current_state = start_state;
    if (engine->entry_hook) {
        engine->entry_hook(engine->user_context);
    }
}

int fsm_step(fsm_engine_t *engine, fsm_event_t event)
{
    if (!engine) return -1;

    /* Prevent stepping if engine is trapped in dead state */
    if (engine->current_state == engine->dead_state) {
        printf("  [FSM-ENGINE] Fault: engine in dead state 0x%X (recovery required)\n",
               engine->dead_state);
        return -2;
    }

    /* Evaluate transitions */
    for (size_t i = 0; i < engine->transition_count; ++i) {
        const fsm_transition_t *t = &engine->transitions[i];

        if (t->current_state == engine->current_state && t->event == event) {
            if (engine->exit_hook) {
                engine->exit_hook(engine->user_context);
            }

            engine->current_state = t->next_state;

            if (engine->entry_hook) {
                engine->entry_hook(engine->user_context);
            }

            return 0;
        }
    }

    return -1; /* No matching transition */
}

void fsm_inject_fault(fsm_engine_t *engine, fsm_state_t illegal_state)
{
    if (!engine) return;

    printf("  [FSM-ENGINE] Fault injected: state changed from %d to 0x%X\n",
           engine->current_state, illegal_state);
    engine->current_state = illegal_state;
}

int fsm_is_dead(const fsm_engine_t *engine)
{
    if (!engine) return 1;
    return engine->current_state == engine->dead_state;
}

void fsm_reset(fsm_engine_t *engine, fsm_state_t safe_state)
{
    if (!engine) return;

    printf("  [FSM-ENGINE] Reset: 0x%X -> safe state %d\n",
           engine->current_state, safe_state);
    engine->current_state = safe_state;

    if (engine->entry_hook) {
        engine->entry_hook(engine->user_context);
    }
}
