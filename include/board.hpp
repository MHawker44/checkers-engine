#ifndef BOARD_HPP
#define BOARD_HPP

#include "types.hpp"
#include "move.hpp"

class Board {
public:
    Board();

    Bitboard occupied() const;
    Bitboard empty() const;
    void print() const;

    Bitboard getWhitePieces() const { return white_pieces; }
    Bitboard getBlackPieces() const { return black_pieces; }
    Bitboard getKings() const { return kings; }
    Side getSideToMove() const { return side_to_move; }
    std::vector<CaptureMove> getLegalCaptures() const;
    std::vector<SimpleMove> getLegalSimpleMoves() const;

    bool isFriendlyPieceAtIndex(int index) const;
    bool isOccupiedAtIndex(int index, Bitboard pieces) const;

    int getLeftStepIndex(int index, Side side) const;
    int getRightStepIndex(int index, Side side) const;
    
    Bitboard validSimpleLeftMoveAtIndex(int index) const;
    Bitboard validSimpleRightMoveAtIndex(int index) const;

    CaptureStep getLeftCaptureStep(int index, Side side) const;
    CaptureStep getRightCaptureStep(int index, Side side) const;

    std::vector<SimpleMove> getSimpleMovesFromIndex(int index) const;
    std::vector<SimpleMove> getSimpleMoves() const;

    std::vector<CaptureMove> getSingleCapturesFromIndex(int index) const;
    std::vector<CaptureMove> getSingleCaptures() const;



private:
    Bitboard white_pieces;
    Bitboard black_pieces;
    Bitboard kings;
    Side side_to_move;
};

#endif