
# Board Model

## Purpose

This document defines the board and move representation used by the engine.

It is the source of truth for:

* square indexing
* board orientation
* side direction
* row parity
* playable-square mapping
* move structure

All move-generation and state-transition code must agree with this document.

---

## 1. Representation

The engine uses a 32-square playable-square representation.

Only dark squares are represented.

Each playable square is assigned an index from `0` to `31`.

The board is stored using 32-bit bitboards.

### Core bitboards

* `white_pieces`
* `black_pieces`
* `kings`

### Turn state

* `side_to_move`

---

## 2. Printed board orientation

The board is printed from top row to bottom row.

* top visible row = row 7
* bottom visible row = row 0

Board columns go from left to right:

* leftmost column = col 0
* rightmost column = col 7

So visible board coordinates are:

* rows: `0..7`
* cols: `0..7`

---

## 3. Bit-index mapping

```
row 7: .  28 . 29 . 30 . 31
row 6: 24 . 25 . 26 . 27 .
row 5: .  20 . 21 . 22 . 23
row 4: 16 . 17 . 18 . 19 .
row 3: .  12 . 13 . 14 . 15
row 2:  8 .  9 . 10 . 11 .
row 1: .   4 .  5 .  6 .  7
row 0:  0 .  1 .  2 .  3 .
```

### Consequence

* low indices are at the bottom of the board
* high indices are at the top of the board

So:

* upward movement ⇒ increasing index
* downward movement ⇒ decreasing index

---

## 4. Side orientation

* `BLACK` starts at the bottom
* `WHITE` starts at the top

### Forward direction

* black forward = toward greater index
* white forward = toward smaller index

So:

* black moves upward
* white moves downward

---

## 5. Player-relative left and right

Left and right are defined relative to the moving side’s forward direction.

### BLACK

Facing upward:

* left  = board up-left
* right = board up-right

### WHITE

Facing downward:

* left  = board down-right
* right = board down-left

---

## 6. Row and offset formulas

For index `i`:

* `row = i / 4`
* `offset = i % 4`

Where:

* `row` ∈ `[0..7]`
* `offset` ∈ `[0..3]`

### Offset meaning

* `offset == 0` = leftmost playable square
* `offset == 3` = rightmost playable square

---

## 7. Row parity

Playable rows alternate shape.

### Even rows (`0,2,4,6`)

Playable columns:

* `0,2,4,6`

### Odd rows (`1,3,5,7`)

Playable columns:

* `1,3,5,7`

### Playability rule

A square `(row, col)` is playable iff:

```
row % 2 == col % 2
```

---

## 8. Mapping visible coordinates → bit index

For playable squares:

```
index = row * 4 + col / 2
```

Examples:

* `(0,0) → 0`
* `(0,2) → 1`
* `(1,1) → 4`
* `(7,1) → 28`

---

## 9. Derived bitboards

### Occupied

```
occupied = white_pieces | black_pieces
```

### Empty

```
empty = ~occupied & VALID_MASK
```

### Men

```
white_men = white_pieces & ~kings
black_men = black_pieces & ~kings
```

---

## 10. Board-state invariants

These must always hold:

* `white_pieces & black_pieces == 0`
* `kings ⊆ occupied`
* `occupied & ~VALID_MASK == 0`
* `empty & ~VALID_MASK == 0`
* `occupied & empty == 0`
* `side_to_move ∈ {WHITE, BLACK}`

---

## 11. Move representation

A move represents one complete turn.

```
Move:
- path: ordered list of squares
- captured: list of captured squares
- promotes: boolean
```

### Semantics

* `path.front()` = source square
* `path.back()` = final destination
* simple move ⇒ `path.size() == 2`
* capture move ⇒ `captured.size() > 0`

### Capture invariant

For capture sequences:

```
captured.size() == path.size() - 1
```

### Promotion rule

* promotion occurs when reaching final row
* promotion ends the move immediately

---

## 12. Initial position

* black occupies bits `0..11`
* white occupies bits `20..31`
* kings = `0`
* side_to_move = `BLACK`

---

## 13. Directional delta table

### BLACK (forward = +)

#### Even row

* left  = `+3` (invalid if `offset == 0`)
* right = `+4`

#### Odd row

* left  = `+4`
* right = `+5` (invalid if `offset == 3`)

---

### WHITE (forward = -)

#### Even row

* left  = `-4`
* right = `-5` (invalid if `offset == 0`)

#### Odd row

* left  = `-3` (invalid if `offset == 3`)
* right = `-4`

---

## 14. Implementation rule

Do not rely on shifts alone.

A move is valid only if it satisfies:

* correct side
* correct row parity
* correct offset constraints

---

## 15. Debugging rule

When in doubt:

1. draw the board
2. locate the source index
3. trace destination manually
4. compare with delta table

---

## 16. Normative examples

These must match implementation.

### Black from 0 (row 0, offset 0)

* left: invalid
* right: 4

### Black from 1

* left: 4
* right: 5

### Black from 7 (row 1, offset 3)

* left: 11
* right: invalid

### White from 13

* left: 10
* right: 9

### White from 12

* left: 9
* right: 8

### White from 31

* left: invalid
* right: 27

---

# Summary

This document now cleanly separates:

* geometry
* representation
* invariants
* move structure

It is now safe to treat as the **single source of truth** for the engine.

---

