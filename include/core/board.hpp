// board.hpp
#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <cstdint>

#include "core/types.hpp"
#include "core/move.hpp"

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

    // Public move-generation API
    std::vector<Move> getMovesFromIndex(int index) const;
    std::vector<Move> getLegalMoves() const;
    Board applyMove(const Move& move) const;

    uint64_t perft(int depth) const;
    void divide(int depth) const;
    

private:
    Bitboard white_pieces;
    Bitboard black_pieces;
    Bitboard kings;
    Side side_to_move;

    // Occupancy / identity helpers
    bool isOccupiedAtIndex(int index, Bitboard pieces) const;
    bool isFriendlyPieceAtIndex(int index) const;
    bool isKingAtIndex(int index) const;
    bool isPromotionSquare(int index, Side side) const;

    // Geometry helpers
    int getLeftStepIndex(int index, Side side) const;
    int getRightStepIndex(int index, Side side) const;

    // Internal move construction helpers (Simple)
    std::vector<Move> getSimpleMovesFromIndex(int index) const;
    std::vector<Move> getSimpleMoves() const;

    // Internal move construction helpers (Captures - Rule Complete)
    std::vector<Move> getCaptureMovesFromIndex(int index) const;
    std::vector<Move> getCaptureMoves() const;

    // The recursive engine for multi-jumps
    void generateCapturesRecursive(
        int currentIndex, 
        bool isKing, 
        Bitboard capturedSoFar, 
        std::vector<int> currentPath, 
        std::vector<int> capturedIndices,
        std::vector<Move>& completedMoves
    ) const;

    void makeMove(const Move& move);
};

#endif