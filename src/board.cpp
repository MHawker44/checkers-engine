#include "board.hpp"
#include "types.hpp"

#include <cassert>
#include <iostream>

Board::Board()
    : white_pieces(0xFFF00000),
      black_pieces(0x00000FFF),
      kings(0),
      side_to_move(Side::BLACK) {}

Bitboard Board::occupied() const {
    return white_pieces | black_pieces;
}

Bitboard Board::empty() const {
    return ~occupied() & VALID_MASK;
}

void Board::print() const {
    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            if (row % 2 != col % 2) {
                std::cout << ". ";
                continue;
            }

            const int index = row * 4 + col / 2;

            if (isOccupiedAtIndex(index, white_pieces)) {
                std::cout << (isOccupiedAtIndex(index, kings) ? "W " : "w ");
            } else if (isOccupiedAtIndex(index, black_pieces)) {
                std::cout << (isOccupiedAtIndex(index, kings) ? "B " : "b ");
            } else {
                std::cout << "_ ";
            }
        }
        std::cout << '\n';
    }
}

bool Board::isFriendlyPieceAtIndex(int index) const {
    assert(index >= 0 && index < 32);

    const Bitboard friendly =
        (side_to_move == Side::WHITE) ? white_pieces : black_pieces;

    return isOccupiedAtIndex(index, friendly);
}

bool Board::isOccupiedAtIndex(int index, Bitboard b) const {
    assert(index >= 0 && index < 32);
    return ((b >> index) & Bitboard(1)) == Bitboard(1);
}

int Board::getLeftStepIndex(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const bool isEvenRow = ((index / 4) % 2) == 0;
    const int offset = index % 4;

    if (side == Side::WHITE) {
        if (isEvenRow) {
            return index - 4;
        }
        return (offset == 3) ? -1 : index - 3;
    }

    if (isEvenRow) {
        return (offset == 0) ? -1 : index + 3;
    }
    return index + 4;
}

int Board::getRightStepIndex(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const bool isEvenRow = ((index / 4) % 2) == 0;
    const int offset = index % 4;

    if (side == Side::WHITE) {
        if (isEvenRow) {
            return (offset == 0) ? -1 : index - 5;
        }
        return index - 4;
    }

    if (isEvenRow) {
        return index + 4;
    }
    return (offset == 3) ? -1 : index + 5;
}

Bitboard Board::validSimpleLeftMoveAtIndex(int index) const {
    assert(index >= 0 && index < 32);

    const Side side = getSideToMove();
    const Bitboard friendly =
        (side == Side::WHITE) ? white_pieces : black_pieces;

    if (!isOccupiedAtIndex(index, friendly)) {
        return 0;
    }

    const int target = getLeftStepIndex(index, side);
    if (target < 0 || target >= 32) {
        return 0;
    }

    return (Bitboard(1) << target) & empty();
}

Bitboard Board::validSimpleRightMoveAtIndex(int index) const {
    assert(index >= 0 && index < 32);

    const Side side = getSideToMove();
    const Bitboard friendly =
        (side == Side::WHITE) ? white_pieces : black_pieces;

    if (!isOccupiedAtIndex(index, friendly)) {
        return 0;
    }

    const int target = getRightStepIndex(index, side);
    if (target < 0 || target >= 32) {
        return 0;
    }

    return (Bitboard(1) << target) & empty();
}

CaptureStep Board::getLeftCaptureStep(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const int target = getLeftStepIndex(index, side);
    if (target < 0 || target >= 32) {
        return {-1, -1};
    }

    const Bitboard opponents =
        (side == Side::WHITE) ? black_pieces : white_pieces;

    if (!isOccupiedAtIndex(target, opponents)) {
        return {-1, -1};
    }

    const int landing = getLeftStepIndex(target, side);
    if (landing < 0 || landing >= 32) {
        return {-1, -1};
    }

    if (isOccupiedAtIndex(landing, occupied())) {
        return {-1, -1};
    }

    return {target, landing};
}

CaptureStep Board::getRightCaptureStep(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const int target = getRightStepIndex(index, side);
    if (target < 0 || target >= 32) {
        return {-1, -1};
    }

    const Bitboard opponents =
        (side == Side::WHITE) ? black_pieces : white_pieces;

    if (!isOccupiedAtIndex(target, opponents)) {
        return {-1, -1};
    }

    const int landing = getRightStepIndex(target, side);
    if (landing < 0 || landing >= 32) {
        return {-1, -1};
    }

    if (isOccupiedAtIndex(landing, occupied())) {
        return {-1, -1};
    }

    return {target, landing};
}

std::vector<SimpleMove> Board::getSimpleMovesFromIndex(int index) const {
    assert(index >= 0 && index < 32);

    std::vector<SimpleMove> moves;

    const Bitboard left = validSimpleLeftMoveAtIndex(index);
    const Bitboard right = validSimpleRightMoveAtIndex(index);

    if (left != 0) {
        const int toIndex = __builtin_ctz(left);
        moves.push_back(SimpleMove{index, toIndex});
    }

    if (right != 0) {
        const int toIndex = __builtin_ctz(right);
        moves.push_back(SimpleMove{index, toIndex});
    }

    return moves;
}

std::vector<SimpleMove> Board::getSimpleMoves() const {
    std::vector<SimpleMove> moves;

    const Bitboard friendly =
        (side_to_move == Side::WHITE) ? white_pieces : black_pieces;

    for (int index = 0; index < 32; ++index) {
        if (!isOccupiedAtIndex(index, friendly)) {
            continue;
        }

        const std::vector<SimpleMove> pieceMoves = getSimpleMovesFromIndex(index);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return moves;
}

std::vector<CaptureMove> Board::getSingleCapturesFromIndex(int index) const {
    assert(index >= 0 && index < 32);

    std::vector<CaptureMove> moves;

    const Side side = getSideToMove();
    const Bitboard friendly =
        (side == Side::WHITE) ? white_pieces : black_pieces;

    if (!isOccupiedAtIndex(index, friendly)) {
        return moves;
    }

    const CaptureStep left = getLeftCaptureStep(index, side);
    if (left.landing != -1) {
        moves.push_back(CaptureMove{index, left.landing, left.captured});
    }

    const CaptureStep right = getRightCaptureStep(index, side);
    if (right.landing != -1) {
        moves.push_back(CaptureMove{index, right.landing, right.captured});
    }

    return moves;
}

std::vector<CaptureMove> Board::getSingleCaptures() const {
    std::vector<CaptureMove> moves;

    const Bitboard friendly =
        (side_to_move == Side::WHITE) ? white_pieces : black_pieces;

    for (int index = 0; index < 32; ++index) {
        if (!isOccupiedAtIndex(index, friendly)) {
            continue;
        }

        const std::vector<CaptureMove> pieceMoves = getSingleCapturesFromIndex(index);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return moves;
}

// Mandatory-capture legality gate.
std::vector<CaptureMove> Board::getLegalCaptures() const {
    return getSingleCaptures();
}

std::vector<SimpleMove> Board::getLegalSimpleMoves() const {
    if (!getSingleCaptures().empty()) {
        return {};
    }
    return getSimpleMoves();
}