# Move Generation

## Dependency

This document assumes the board orientation and delta table defined in:

- `docs/invariants.md`

Do not duplicate or redefine orientation rules here.

## Layering

### Geometry helpers

- `getLeftStepIndex(index, side)`
- `getRightStepIndex(index, side)`

These use:

- side orientation
- row parity
- offset edge conditions
- delta table from `docs/invariants.md`

### Source-level generation

- `getSimpleMovesFromIndex(index)`
- `getCaptureMovesFromIndex(index)`

These build complete `Move` objects for one source square.

### Side-level generation

- `getSimpleMoves()`
- `getCaptureMoves()`

These iterate over all pieces for `side_to_move`.

### Multi-jump capture generation

- `generateCapturesRecursive(...)`

This extends a partial capture path until no further legal jump exists.

For capture sequences:

- `Move::path` stores the visited landing squares in order, including source and final destination
- `Move::captured` stores captured square indices in jump order
- promotion ends the move immediately

### Legal move generation

- if any capture exists:
  - simple moves are illegal
  - legal moves = captures
- otherwise:
  - legal moves = simple moves
