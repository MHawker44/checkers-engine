#include <cassert>
#include <vector>

#include "board.hpp"

namespace {

bool containsSimpleMove(const std::vector<SimpleMove>& moves, int from, int to) {
    for (const SimpleMove& move : moves) {
        if (move.from == from && move.to == to) {
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

    assert(board.isOccupiedAtIndex(0, board.getBlackPieces()));
    assert(board.isOccupiedAtIndex(31, board.getWhitePieces()));
    assert(!board.isOccupiedAtIndex(12, board.occupied()));

    assert(board.isFriendlyPieceAtIndex(0));
    assert(board.isFriendlyPieceAtIndex(11));
    assert(!board.isFriendlyPieceAtIndex(20));

    assert(board.getLeftStepIndex(8, Side::BLACK) == -1);
    assert(board.getRightStepIndex(8, Side::BLACK) == 12);
    assert(board.getLeftStepIndex(9, Side::BLACK) == 12);
    assert(board.getRightStepIndex(9, Side::BLACK) == 13);
    assert(board.getLeftStepIndex(22, Side::WHITE) == 19);
    assert(board.getRightStepIndex(22, Side::WHITE) == 18);

    assert(board.validSimpleLeftMoveAtIndex(8) == 0u);
    assert(board.validSimpleRightMoveAtIndex(8) == (Bitboard(1) << 12));
    assert(board.validSimpleLeftMoveAtIndex(9) == (Bitboard(1) << 12));
    assert(board.validSimpleRightMoveAtIndex(9) == (Bitboard(1) << 13));
    assert(board.validSimpleRightMoveAtIndex(0) == 0u);

    const CaptureStep leftStep = board.getLeftCaptureStep(9, Side::BLACK);
    assert(leftStep.captured == -1);
    assert(leftStep.landing == -1);

    const CaptureStep rightStep = board.getRightCaptureStep(9, Side::BLACK);
    assert(rightStep.captured == -1);
    assert(rightStep.landing == -1);

    const std::vector<SimpleMove> pieceMoves = board.getSimpleMovesFromIndex(9);
    assert(pieceMoves.size() == 2);
    assert(containsSimpleMove(pieceMoves, 9, 12));
    assert(containsSimpleMove(pieceMoves, 9, 13));

    const std::vector<SimpleMove> allSimpleMoves = board.getSimpleMoves();
    assert(allSimpleMoves.size() == 7);
    assert(containsSimpleMove(allSimpleMoves, 8, 12));
    assert(containsSimpleMove(allSimpleMoves, 9, 12));
    assert(containsSimpleMove(allSimpleMoves, 9, 13));
    assert(containsSimpleMove(allSimpleMoves, 10, 13));
    assert(containsSimpleMove(allSimpleMoves, 10, 14));
    assert(containsSimpleMove(allSimpleMoves, 11, 14));
    assert(containsSimpleMove(allSimpleMoves, 11, 15));

    assert(board.getSingleCapturesFromIndex(9).empty());
    assert(board.getSingleCaptures().empty());
    assert(board.getLegalCaptures().empty());

    const std::vector<SimpleMove> legalSimpleMoves = board.getLegalSimpleMoves();
    assert(legalSimpleMoves.size() == allSimpleMoves.size());
    assert(containsSimpleMove(legalSimpleMoves, 8, 12));
    assert(containsSimpleMove(legalSimpleMoves, 11, 15));
}
