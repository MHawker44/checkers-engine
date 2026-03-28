#include <cassert>
#include <vector>

#include "core/board.hpp"

namespace {

bool containsMove(const std::vector<Move>& moves, int from, int to) {
    for (const Move& move : moves) {
        if (move.path.size() == 2 && move.path.front() == from && move.path.back() == to) {
            return true;
        }
    }
    return false;
}

} // namespace

void run_board_tests() {
    const Board board;

    assert(board.getWhitePieces() == 0xFFF00000u);
    assert(board.getBlackPieces() == 0x00000FFFu);
    assert(board.getKings() == 0u);
    assert(board.getSideToMove() == Side::BLACK);

    assert(board.occupied() == (board.getWhitePieces() | board.getBlackPieces()));
    assert(board.empty() == (~board.occupied() & VALID_MASK));

    const std::vector<Move> legalMoves = board.getLegalMoves();
    assert(legalMoves.size() == 7);
    assert(containsMove(legalMoves, 8, 12));
    assert(containsMove(legalMoves, 9, 12));
    assert(containsMove(legalMoves, 9, 13));
    assert(containsMove(legalMoves, 10, 13));
    assert(containsMove(legalMoves, 10, 14));
    assert(containsMove(legalMoves, 11, 14));
    assert(containsMove(legalMoves, 11, 15));

    const std::vector<Move> movesFromNine = board.getMovesFromIndex(9);
    assert(movesFromNine.size() == 2);
    assert(containsMove(movesFromNine, 9, 12));
    assert(containsMove(movesFromNine, 9, 13));

    assert(board.getMovesFromIndex(20).empty());

    const Move simpleMove{{8, 12}, {}, false};
    const Board nextBoard = board.applyMove(simpleMove);

    assert(
        nextBoard.getBlackPieces() ==
        ((board.getBlackPieces() & ~(Bitboard(1) << 8)) | (Bitboard(1) << 12))
    );
    assert(nextBoard.getWhitePieces() == board.getWhitePieces());
    assert(nextBoard.getKings() == 0u);
    assert(nextBoard.getSideToMove() == Side::WHITE);

    assert(board.perft(0) == 1);
    assert(board.perft(1) == legalMoves.size());
}
