# Mini Project 18 – Finite Automata Builds (Worklet 1)
### COEP Technological University · CSE "Mini Project Leap" Competition

---

## Project Overview

A production-grade, modular C99 implementation of two core Finite State Machine (FSM) case studies, complete with a generic FSM engine, ANSI TUI dashboard, and external state table ingestion for cross-group (Worklet 2) integration.

---

## Repository Structure

```
Mini_Project_18_Worklet1/
├── inc/
│   ├── fsm_engine.h       # Generic FSM engine: transition table, hooks, fault recovery
│   ├── traffic_light.h    # Traffic Light FSM: states, durations, emergency override
│   ├── counter.h          # 4-Bit Counter FSM: bit-field registers, clock tick, waveform
│   └── tui_render.h       # ANSI TUI: colors, dashboard, CSV parser declarations
├── src/
│   ├── fsm_engine.c       # Engine: transition eval, dead-state detection, safe reset
│   ├── traffic_light.c    # Traffic Light: timer logic, override, fault injection
│   ├── counter.c          # Counter: bitwise register masking, trace, ASCII waveform
│   ├── tui_render.c       # TUI renderer + CSV loader
│   └── main.c             # Interactive CLI menu driver
├── data/
│   └── custom_fsm.csv     # Sample Worklet 2 state transition table (CSV format)
├── Makefile               # Build automation
└── README.md              # This file
```

---

## Case Studies

### Case Study 1 – Traffic Light Controller FSM

| State  | Duration | Transitions To |
|--------|----------|----------------|
| RED    | 5 s      | GREEN          |
| GREEN  | 4 s      | YELLOW         |
| YELLOW | 2 s      | RED            |

**Special Features:**
- **Emergency Vehicle Override** – forces immediate GREEN from any state
- **Fault Injection** – corrupts state to `0x7F` (dead state sentinel)
- **Dead-State Recovery** – Safety engine detects `0x7F`, logs fault, resets to RED

**State Diagram:**
```
    ┌─────────────────────────────────────────────────┐
    │                                                 │
    ▼    5 ticks        4 ticks        2 ticks        │
  [RED] ────────► [GREEN] ────────► [YELLOW] ─────────┘

  Emergency: ANY STATE ──────────────────► [GREEN]
  Fault:     ANY STATE ──── 0x7F ───────► DEAD STATE
  Recovery:  DEAD STATE ──── Safety ───► [RED]
```

---

### Case Study 2 – 4-Bit Synchronous Binary Counter FSM

- Models **Q3 Q2 Q1 Q0** using C bit-fields (1-bit each)
- Clock-edge triggered increment using bitwise `& 0x0F` for modulo-16 rollover
- Register decomposition: `q0 = (val >> 0) & 1`, `q1 = (val >> 1) & 1`, etc.
- Outputs: truth trace table + ASCII clock waveform

**Truth Table (partial):**
```
  Cycle |  Dec  | Q3 | Q2 | Q1 | Q0 | Status
  ------|-------|----|----|----|----|---------
    0   |   0   |  0 |  0 |  0 |  0 |
    1   |   1   |  0 |  0 |  0 |  1 |
   ...  |  ...  | .. | .. | .. | .. |
   15   |  15   |  1 |  1 |  1 |  1 | MAX
   16   |   0   |  0 |  0 |  0 |  0 | ROLLOVER
```

---

### External State Table (Worklet 2 Integration)

CSV format expected from Group 2:

```csv
CurrentState,InputSymbol,NextState,OutputAction
S0,a,S1,Emit_X
S1,b,S0,Emit_W
...
```

The `load_external_fsm_csv()` function parses this into `csv_transition_row_t[]` arrays which can be directly fed into the generic `fsm_engine_t` transition table.

---

## Compilation & Build Instructions

### Prerequisites
- GCC (MinGW on Windows, or native GCC on Linux/macOS)
- Make (`mingw32-make` on Windows, `make` on Linux/macOS)

### Build

```bash
# Build the executable
make

# Or build manually with full flags:
gcc -Wall -Wextra -std=c99 -Iinc -o fsm_sim src/main.c src/fsm_engine.c src/traffic_light.c src/counter.c src/tui_render.c
```

### Run

```bash
# Linux / macOS
./fsm_sim

# Windows (PowerShell / CMD)
.\fsm_sim.exe
```

### Clean Build Artifacts

```bash
make clean
```

---

## Execution & Direct Test Runner

Run without arguments to execute the complete automated demonstration and verification suite, or pass a target argument:

```bash
# Run all demonstrations and test suites (default)
.\fsm_sim.exe
# or
.\fsm_sim.exe all

# Run specific modules:
.\fsm_sim.exe traffic   # Case Study 1: Traffic Light Controller FSM
.\fsm_sim.exe counter   # Case Study 2: 4-Bit Synchronous Binary Counter FSM
.\fsm_sim.exe engine    # Core FSM Engine Validation Suite
.\fsm_sim.exe csv       # External State Table CSV Ingestion Demo
.\fsm_sim.exe --help    # Command-line usage summary
```

**Demonstration Modules:**
- **Traffic Light Controller (`traffic`)** – Runs 20-tick automated cycle:
  - Normal cycling: RED (5s) -> GREEN (4s) -> YELLOW (2s)
  - Emergency vehicle override at Tick 7: forces immediate GREEN
  - Fault injection (0x7F) at Tick 13: self-healing dead-state safety recovery resets to RED
- **4-Bit Binary Counter (`counter`)** – Up-counter (modulo-16 rollover 15->0), Down-counter (0->15 rollover), Boundary validation (>15 clamped), and ASCII waveform generator
- **Core FSM Engine (`engine`)** – Transition validation, missing event rejection (-1), and dead-state trap/recovery (-2)
- **External State Table (`csv`)** – Parses `data/custom_fsm.csv` into C transition structures for Worklet 2 integration

---

## Technical Architecture

| Component         | Design Pattern                                        |
|-------------------|-------------------------------------------------------|
| `fsm_engine`      | Table-driven transitions, function pointer hooks      |
| `traffic_light`   | Embedded engine composition, timer context            |
| `counter`         | Bit-field registers, bitwise masking (`& 0x0F`)      |
| `tui_render`      | ANSI escape codes, `sscanf`-based CSV parser         |

---

## Compilation Flags

```
-Wall -Wextra -std=c99 -Iinc
```

Compiles with **zero warnings** under GCC.

---

## Authors

**Group 18 · Worklet 1** – COEP Technological University  
CSE Department · Mini Project Leap Competition
