# Rules and State

## State vocabulary

- The board consists of 32 playable squares.
- Each square is identified by a unique index.
- Each square is either empty or occupied by exactly one piece.
- Each piece has exactly one color: white or black.
- Each piece has exactly one rank: man or king.

## Board invariants

- The board has exactly 32 playable squares.
- Every square contains either no piece or exactly one piece.
- No piece exists outside the 32 playable squares.
- Every piece has exactly one color.
- Every piece has exactly one rank.
- No piece is both white and black.
- No piece is both a man and a king.

## Turn and orientation

- Black moves first.
- White forward direction is negative.
- Black forward direction is positive.
- A legal move must move a piece belonging to the side to move.
- After a complete legal move sequence, the side to move flips.

- White's promotion squares are 0 - 3
- Black's promotion squares are 29 - 32

## Move legality rules

- A move can only be made by a player when it is their turn.
- A man may move diagonally forward by one square in a non-capturing move.
- A king may move diagonally by one square in either direction in a non-capturing move.
- A capture jumps over exactly one adjacent opposing piece and lands on an empty square immediately beyond it.
- A simple move is illegal if any capture is available for the side to move.
- After a capture, if another capture is available from the landing square, the sequence must continue.
- A man that ends its move on the promotion rank becomes a king.

## Bit representation

    .  28 . 29 . 30 . 31
    24 . 25 . 26 . 27 .
    . 20 . 21 . 22 . 23
    16 . 17 . 18 . 19 .
    . 12 . 13 . 14 . 15
     8 .  9 . 10 . 11 .
    .  4 .  5 .  6 .  7
     0 .  1 .  2 .  3 .