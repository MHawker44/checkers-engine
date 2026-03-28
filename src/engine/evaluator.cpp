#include "engine/evaluator.hpp"
#include "core/board.hpp"

/*
Positive is good for Black, Negative is good for White
This breaks chess convention, but black is the "maximizing" player in our implementation, so it makes more sense to have positive values favor black.
*/


int Evaluator::evaluate(const Board& board) const {
    const Bitboard white_pieces = board.getWhitePieces();
    const Bitboard black_pieces = board.getBlackPieces();
    // piece evaluation:
    // count
    const Bitboard white_kings = board.getKings() & white_pieces;
    const Bitboard black_kings = board.getKings() & black_pieces;

    // Material Values
    const int PIECE_VALUE = 100;
    const int KING_BONUS = 50; // Kings get 100 (from counting as pieces already) + 50 = 150 total

    // Positional Values
    const int CENTER_BONUS = 4;
    const int KING_CENTER_BONUS = 1; // Kings get 4 (from counting as pieces already) + 1 = 5 total

    const int pieceCount = __builtin_popcount(black_pieces) * PIECE_VALUE
                         - __builtin_popcount(white_pieces) * PIECE_VALUE;        
    const int kingCount = __builtin_popcount(black_kings) * (KING_BONUS)
                        - __builtin_popcount(white_kings) * (KING_BONUS);
        
    

    const Bitboard home_row_white_mask = 0xF0000000; // Row 7 for white
    const Bitboard home_row_black_mask = 0x0000000F; // Row 0 for black
    const Bitboard white_men_home_mask = white_pieces & ~white_kings & home_row_white_mask;
    const Bitboard black_men_home_mask = black_pieces & ~black_kings & home_row_black_mask;

    const Bitboard central_control_mask = 0x000FF000; // Rows 3 and 4
    
    const Bitboard white_pieces_center_mask = white_pieces & central_control_mask;
    const Bitboard black_pieces_center_mask = black_pieces & central_control_mask;
    const Bitboard white_kings_center_mask = white_kings & central_control_mask;
    const Bitboard black_kings_center_mask = black_kings & central_control_mask;

    int pieceHomeCount =  __builtin_popcount(black_men_home_mask) * 3
                        - __builtin_popcount(white_men_home_mask) * 3;

    int pieceCenterCount = __builtin_popcount(black_pieces_center_mask) * CENTER_BONUS
                         - __builtin_popcount(white_pieces_center_mask) * CENTER_BONUS;

    int kingCenterCount = __builtin_popcount(black_kings_center_mask) * KING_CENTER_BONUS
                        - __builtin_popcount(white_kings_center_mask) * KING_CENTER_BONUS;



    return pieceCount 
        + kingCount
        + pieceHomeCount
        + pieceCenterCount
        + kingCenterCount;
}