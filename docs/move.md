# Move Generation

## Dependency

This document assumes the board orientation and delta table defined in:

- `docs/board_model.md`

Do not duplicate or redefine orientation rules here.

## Layering

### Primitive directional queries

- `validSimpleLeftMoveAtIndex(index)`
- `validSimpleRightMoveAtIndex(index)`
- `validCaptureLeftMoveAtIndex(index)`
- `validCaptureRightMoveAtIndex(index)`

These use:

- side_to_move
- row parity
- offset edge conditions
- delta table from `board_model.md`

### Source-level generation

- `getSimpleMovesFromIndex(index)`
- `getCaptureMovesFromIndex(index)`

These convert primitive destination results into `Move` objects.

### Side-level generation

- `getSimpleMoves()`
- `getCaptureMoves()`

These iterate over all pieces for `side_to_move`.

### Legal move generation

- if any capture exists:
  - simple moves are illegal
  - legal moves = captures
- otherwise:
  - legal moves = simple moves