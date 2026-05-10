# Architecture

This document maps the main modules in the chess engine and explains how game state, move generation, move application, search, draw detection, position keys and notation parsing fit together.

The most important distinction in the codebase is:

- **Real game moves** go through `play_move()`
- **Search/perft moves** go directly through `make_move()` / `unmake_move()`

This keeps real game history separate from temporary engine search state.

---

## Real Moves vs Search Moves

Real moves and search moves use the same underlying move application system, but they have different responsibilities.

Real game moves must update the board, game metadata and repetition history. Search moves must be reversible and must not modify real game history.

```mermaid
%%{init: {"flowchart": {"curve": "linear", "nodeSpacing": 45, "rankSpacing": 70}} }%%
flowchart TD
    START["Move needs to be applied"]

    START --> CONTEXT{"Move context?"}

    CONTEXT -->|"Real game move"| PLAY["play_move()<br/>game_play.c"]
    PLAY --> REAL_MAKE["make_move()<br/>move_apply.c"]
    REAL_MAKE --> REAL_KEY["create_position_key()<br/>position_key.c"]
    REAL_KEY --> REAL_HISTORY["Push position key to real history<br/>game->position_history"]

    CONTEXT -->|"Search / perft / validation"| MAKE["make_move()<br/>move_apply.c"]
    MAKE --> WORK["Search, perft, or legality check"]
    WORK --> UNMAKE["unmake_move()<br/>move_apply.c"]
    UNMAKE --> CLEAN["Real history unchanged"]

    classDef start fill:#1f2937,stroke:#94a3b8,stroke-width:1.5px,color:#f8fafc;
    classDef decision fill:#78350f,stroke:#fbbf24,stroke-width:1.5px,color:#fffbeb;
    classDef real fill:#14532d,stroke:#86efac,stroke-width:1.5px,color:#f0fdf4;
    classDef search fill:#1e3a8a,stroke:#93c5fd,stroke-width:1.5px,color:#eff6ff;

    class START start;
    class CONTEXT decision;
    class PLAY,REAL_MAKE,REAL_KEY,REAL_HISTORY real;
    class MAKE,WORK,UNMAKE,CLEAN search;
```

`play_move()` is a wrapper for real game moves. It calls `make_move()` and then records the resulting position key in `game->position_history`.

Search, perft and temporary legality checks call `make_move()` / `unmake_move()` directly. They must not call `play_move()` because that would pollute the real game history with positions that were only analysed temporarily.

---

## Search Engine Pipeline

The engine currently uses negamax with alpha-beta pruning. Each candidate move is made on the board, searched recursively, then unmade to restore the original state.

```mermaid
%%{init: {"flowchart": {"curve": "linear", "nodeSpacing": 45, "rankSpacing": 70}} }%%
flowchart TD
    ROOT["engine_find_best_move()<br/>engine.c"]

    ROOT --> GEN["Generate legal moves<br/>move_gen.c"]
    GEN --> LOOP["Loop through candidate moves"]

    LOOP --> MAKE["make_move()<br/>move_apply.c"]
    MAKE --> KEY["Create temporary position key"]
    KEY --> REP{"Threefold in search line?"}

    REP -->|"Yes"| DRAW_SCORE["Score = 0"]
    REP -->|"No"| SEARCH["negamax()<br/>recursive search"]

    SEARCH --> TERMINAL{"Terminal node?"}
    TERMINAL -->|"Depth 0"| EVAL["evaluate_position()<br/>evaluation.c"]
    TERMINAL -->|"Checkmate"| MATE["Mate score"]
    TERMINAL -->|"Stalemate / draw"| DRAW["Draw score = 0"]
    TERMINAL -->|"Continue"| CHILDREN["Generate child moves"]

    CHILDREN --> MAKE_CHILD["make_move() child"]
    MAKE_CHILD --> CHILD_KEY["Push temporary key"]
    CHILD_KEY --> SEARCH

    EVAL --> UNMAKE["unmake_move()"]
    MATE --> UNMAKE
    DRAW --> UNMAKE
    DRAW_SCORE --> UNMAKE

    UNMAKE --> BEST["Update best move<br/>alpha-beta pruning"]

    classDef engine fill:#7c2d12,stroke:#fdba74,stroke-width:1.5px,color:#fff7ed;
    classDef move fill:#1e3a8a,stroke:#93c5fd,stroke-width:1.5px,color:#eff6ff;
    classDef decision fill:#78350f,stroke:#fbbf24,stroke-width:1.5px,color:#fffbeb;
    classDef eval fill:#14532d,stroke:#86efac,stroke-width:1.5px,color:#f0fdf4;
    classDef terminal fill:#581c87,stroke:#d8b4fe,stroke-width:1.5px,color:#faf5ff;

    class ROOT,SEARCH,BEST engine;
    class GEN,LOOP,MAKE,KEY,CHILDREN,MAKE_CHILD,CHILD_KEY,UNMAKE move;
    class REP,TERMINAL decision;
    class EVAL eval;
    class DRAW_SCORE,MATE,DRAW terminal;
```

The engine evaluates each node from the perspective of the side to move, then negates scores when returning to the previous ply.

Search uses a temporary position-key history for the current analysed line. This allows the engine to detect repeated positions during search without modifying the real game history.

---

## Position Keys and Repetition

A `PositionKey` identifies a chess position for repetition detection and repetition-aware search.

It represents:

- Piece placement
- Side to move
- Castling rights
- En passant target

It intentionally does not represent:

- Halfmove clock
- Fullmove number
- Move text

This matches the information needed to decide whether two positions are the same for threefold repetition.

```mermaid
%%{init: {"flowchart": {"curve": "linear", "nodeSpacing": 45, "rankSpacing": 70}} }%%
flowchart TD
    GS["GameState"]

    GS --> BOARD["Piece placement"]
    GS --> SIDE["Side to move"]
    GS --> CASTLE["Castling rights"]
    GS --> EP["En passant target"]

    BOARD --> KEY["PositionKey<br/>position_key.c"]
    SIDE --> KEY
    CASTLE --> KEY
    EP --> KEY

    KEY --> REAL["Real game history<br/>game->position_history"]
    KEY --> SEARCH["Temporary search history<br/>search_history[]"]

    REAL --> DRAW_RULE["Actual threefold detection<br/>draw_rules.c"]
    REAL --> ENGINE_REP["Engine repetition check"]
    SEARCH --> ENGINE_REP

    DRAW_RULE --> GAME_DRAW["Game result: draw"]
    ENGINE_REP --> SEARCH_SCORE["Search result: score 0"]

    classDef state fill:#0f172a,stroke:#60a5fa,stroke-width:1.5px,color:#eff6ff;
    classDef key fill:#78350f,stroke:#fbbf24,stroke-width:1.5px,color:#fffbeb;
    classDef history fill:#14532d,stroke:#86efac,stroke-width:1.5px,color:#f0fdf4;
    classDef result fill:#581c87,stroke:#d8b4fe,stroke-width:1.5px,color:#faf5ff;

    class GS,BOARD,SIDE,CASTLE,EP state;
    class KEY key;
    class REAL,SEARCH history;
    class DRAW_RULE,ENGINE_REP,GAME_DRAW,SEARCH_SCORE result;
```

There are two kinds of position history:

| History | Stored in | Purpose |
|---|---|---|
| Real game history | `game->position_history` | Detect threefold repetition in the actual played game |
| Search line history | `search_history[]` inside `engine.c` | Detect repetition inside temporary engine analysis |

The search history only applies to the current branch being analysed. It is not stored permanently.

---

## Ownership Model

The board owns the pieces stored in its grid. Moves and squares are value types. Search uses `UndoInfo` to restore captured pieces and previous game metadata after temporary moves.

```mermaid
%%{init: {"flowchart": {"curve": "linear", "nodeSpacing": 45, "rankSpacing": 70}} }%%
flowchart TD
    GAME["GameState"]
    BOARD["Board"]
    PIECES["Piece pointers<br/>stored in board grid"]
    MOVE["Move<br/>value type"]
    SQUARE["Square<br/>value type"]
    UNDO["UndoInfo<br/>temporary restore data"]

    GAME --> BOARD
    BOARD --> PIECES

    MOVE --> APPLY["make_move()"]
    SQUARE --> MOVE
    APPLY --> BOARD
    APPLY --> UNDO

    UNDO --> UNMAKE["unmake_move()"]
    UNMAKE --> BOARD

    BOARD --> DESTROY["destroy_board()"]
    DESTROY --> FREE["Destroys remaining pieces"]

    classDef owner fill:#0f172a,stroke:#60a5fa,stroke-width:1.5px,color:#eff6ff;
    classDef value fill:#3f3f46,stroke:#d4d4d8,stroke-width:1.5px,color:#fafafa;
    classDef action fill:#064e3b,stroke:#6ee7b7,stroke-width:1.5px,color:#ecfdf5;
    classDef danger fill:#7f1d1d,stroke:#fca5a5,stroke-width:1.5px,color:#fef2f2;

    class GAME,BOARD,PIECES owner;
    class MOVE,SQUARE,UNDO value;
    class APPLY,UNMAKE action;
    class DESTROY,FREE danger;
```

`make_move()` moves existing `Piece *` pointers between board squares. It does not allocate a new piece for normal moves. Captured pieces are stored in `UndoInfo` so `unmake_move()` can restore them during search.

When the board is destroyed, it destroys any pieces still present in the grid.

---

## Module Layers

| Layer | Modules | Responsibility |
|---|---|---|
| Application | `main.c` | CLI loop, human input, engine turn handling |
| Input / Notation | `fen_parser.c`, `san_parser.c`, `san.c`, `san_resolve.c` | Load FEN positions and convert SAN input into concrete moves |
| Game State & History | `game_state.c`, `game_play.c`, `position_key.c` | Store game metadata, apply real moves and track repetition history |
| Chess Rules | `move_gen.c`, `move_apply.c`, `check_rules.c`, `draw_rules.c` | Generate legal moves, make/unmake moves, detect check/checkmate/stalemate and draws |
| Engine | `engine.c`, `evaluation.c` | Search legal moves using negamax/alpha-beta and evaluate positions |
| Core Data | `board.c`, `piece.c`, `move.c`, `square.c` | Define and manage the core chess data structures |
| Debug / Utilities | `debug_print.c`, `linked_list.c`, `queue.c` | Debug output and generic helper structures |

---

## Future Direction

The current architecture is intentionally simple and pointer-based. Future engine improvements such as move ordering, iterative deepening, quiescence search, Zobrist hashing, transposition tables and bitboards can be added without changing the high-level module boundaries.

For example:

- Move ordering can extend `engine.c` or become a separate `move_ordering.c` module.
- Zobrist hashing can replace or accelerate `position_key.c`.
- Transposition tables can sit beside the engine layer and use position keys.
- UCI support can be added as a new application-layer entry point beside `main.c`.