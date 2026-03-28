# Board Model

Note: canonical board-state and move-semantics documentation now lives in `docs/invariants.md`.
This file is kept as a geometry-oriented reference.

## Purpose

This document defines the board geometry used by the engine.

It is a reference for:

- square indexing
- board orientation
- side direction
- row parity
- playable-square mapping

The full source of truth for engine invariants and move representation is `docs/invariants.md`.

---

## 1. Representation

The engine uses a 32-square playable-square representation.

Only dark squares are represented.

Each playable square is assigned an index from 0 to 31.

The board is stored using 32-bit bitboards.

### Core bitboards

- `white_pieces`
- `black_pieces`
- `kings`

### Turn state

- `side_to_move`

---

## 2. Printed board orientation

The board is printed from top row to bottom row.

- top visible row = row 7
- bottom visible row = row 0

Board columns go from left to right:

- leftmost column = col 0
- rightmost column = col 7

So visible board coordinates are:

- rows: `0..7`
- cols: `0..7`

with `(0,0)` at the bottom-left when reasoning geometrically, but printing is usually done from row 7 down to row 0.

---

## 3. Bit-index mapping

The playable-square mapping is:

    row 7: .  28 . 29 . 30 . 31
    row 6: 24 . 25 . 26 . 27 .
    row 5: .  20 . 21 . 22 . 23
    row 4: 16 . 17 . 18 . 19 .
    row 3: .  12 . 13 . 14 . 15
    row 2:  8 .  9 . 10 . 11 .
    row 1: .   4 .  5 .  6 .  7
    row 0:  0 .  1 .  2 .  3 .

This means:

- low indices are at the bottom of the board
- high indices are at the top of the board

### Consequence

- moving upward on the board means moving to a greater index
- moving downward on the board means moving to a smaller index

---

## 4. Side orientation

The engine uses the following convention:

- `BLACK` starts at the bottom of the board
- `WHITE` starts at the top of the board

### Forward direction

- black forward = toward greater index
- white forward = toward smaller index

So:

- black moves upward
- white moves downward

This convention must be used consistently in all move-generation code.

---

## 5. Player-relative left and right

Left and right are defined relative to the moving side's forward direction.

This is critical.

### For BLACK

Black faces upward.

So:

- black-left = board up-left
- black-right = board up-right

### For WHITE

White faces downward.

So:

- white-left = board down-right
- white-right = board down-left

This is the main source of confusion if not stated explicitly.

---

## 6. Row and offset formulas

Each playable row contains 4 playable squares.

For an index `i`:

- `row = i / 4`
- `offset = i % 4`

Where:

- `row` is the playable-row number from 0 to 7
- `offset` is the square position inside that playable row

### Offset meaning

Within a playable row:

- `offset == 0` = leftmost playable square in that row
- `offset == 3` = rightmost playable square in that row

---

## 7. Row parity

Playable rows alternate shape.

### Even playable rows

Rows:

- 0
- 2
- 4
- 6

These correspond to visible rows where playable squares are at:

- cols `0, 2, 4, 6`

### Odd playable rows

Rows:

- 1
- 3
- 5
- 7

These correspond to visible rows where playable squares are at:

- cols `1, 3, 5, 7`

### Formula

A visible square `(row, col)` is playable iff:

- `row % 2 == col % 2`

---

## 8. Mapping between visible board coordinates and bit index

If `(row, col)` is playable, then its bit index is:

- `index = row * 4 + col / 2`

Examples:

- `(0,0) -> 0`
- `(0,2) -> 1`
- `(1,1) -> 4`
- `(7,1) -> 28`

This formula is used by `print()` and any board-coordinate debugging logic.

---

## 9. Derived bitboards

### Occupied

- `occupied = white_pieces | black_pieces`

### Empty

- `empty = ~occupied & VALID_MASK`

### Men

- `white_men = white_pieces & ~kings`
- `black_men = black_pieces & ~kings`

---

## 10. Invariants

These must be true for every valid board state.

- `white_pieces & black_pieces == 0`
- `kings` is a subset of `occupied`
- `occupied` is a subset of `VALID_MASK`
- `empty` is a subset of `VALID_MASK`
- `occupied & empty == 0`
- every represented piece is on a playable square
- `side_to_move` is exactly one of:
  - `WHITE`
  - `BLACK`

---

## 11. Initial position

Initial state for this engine:

- black occupies bits `0..11`
- white occupies bits `20..31`
- kings = `0`
- side_to_move = `BLACK`

This matches the orientation rules above:

- black starts at the bottom and moves upward
- white starts at the top and moves downward

---

## 12. Directional delta table

This table is the move-generation ground truth.

### BLACK (forward = +)

#### Even row
- left  = `+3`   invalid if `offset == 0`
- right = `+4`

#### Odd row
- left  = `+4`
- right = `+5`   invalid if `offset == 3`

### WHITE (forward = -)

#### Even row
- left  = `-4`
- right = `-5`   invalid if `offset == 0`

#### Odd row
- left  = `-3`   invalid if `offset == 3`
- right = `-4`

All simple-move and capture code must derive from this table.

---

## 13. Important implementation rule

Do not infer geometry from a raw shift alone.

A shift is only valid if it agrees with:

- side
- row parity
- offset edge conditions

This is necessary because some invalid directional moves can still land on another in-range bit.

---

## 14. Debugging rule

If any directional move logic seems uncertain:

1. draw the board
2. locate the source index
3. trace the destination visually
4. compare against the delta table
5. only then write code
