/*
 * fsm_engine.h - Generic Finite State Machine Framework
 *
 * Mini Project 18 - Worklet 1 - COEP Technological University
 *
 * Transition-table-driven FSM engine supporting state/event IDs,
 * entry/exit action hooks, fault injection, and dead-state recovery.
 */

#ifndef FSM_ENGINE_H
#define FSM_ENGINE_H

#include <stddef.h>

/* State and event identifiers (cast from user enums) */
typedef int fsm_state_t;
typedef int fsm_event_t;

/* Action callback receiving user context */
typedef void (*fsm_action_fn)(void *context);

/* Single transition rule: (current_state, event) -> next_state */
typedef struct {
    fsm_state_t current_state;
    fsm_event_t event;
    fsm_state_t next_state;
} fsm_transition_t;

/* FSM Engine instance */
typedef struct {
    fsm_state_t             current_state;
    fsm_state_t             dead_state;
    const fsm_transition_t *transitions;
    size_t                  transition_count;
    fsm_action_fn           entry_hook;
    fsm_action_fn           exit_hook;
    void                   *user_context;
} fsm_engine_t;

/* Initialize engine to start_state and trigger entry_hook */
void fsm_init(fsm_engine_t *engine, fsm_state_t start_state);

/*
 * Process an event against the transition table.
 * Returns:  0 on success
 *          -1 if no matching transition was found
 *          -2 if currently in dead state (recovery required)
 */
int fsm_step(fsm_engine_t *engine, fsm_event_t event);

/* Forcibly corrupt engine state for fault testing */
void fsm_inject_fault(fsm_engine_t *engine, fsm_state_t illegal_state);

/* Check if engine is currently in dead state */
int fsm_is_dead(const fsm_engine_t *engine);

/* Reset engine to a safe state and trigger entry_hook */
void fsm_reset(fsm_engine_t *engine, fsm_state_t safe_state);

#endif /* FSM_ENGINE_H */
