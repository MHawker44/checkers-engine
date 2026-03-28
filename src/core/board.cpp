// board.cpp
#include "core/board.hpp"

#include <cassert>
#include <iostream>

namespace {
Side oppositeSide(Side side) {
    return (side == Side::WHITE) ? Side::BLACK : Side::WHITE;
}
}

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

bool Board::isOccupiedAtIndex(int index, Bitboard pieces) const {
    assert(index >= 0 && index < 32);
    return ((pieces >> index) & Bitboard(1)) == Bitboard(1);
}

bool Board::isFriendlyPieceAtIndex(int index) const {
    assert(index >= 0 && index < 32);
    const Bitboard friendly = (side_to_move == Side::WHITE) ? white_pieces : black_pieces;
    return isOccupiedAtIndex(index, friendly);
}

bool Board::isKingAtIndex(int index) const {
    assert(index >= 0 && index < 32);
    return isOccupiedAtIndex(index, kings);
}

bool Board::isPromotionSquare(int index, Side side) const {
    assert(index >= 0 && index < 32);
    const Bitboard mask = (side == Side::WHITE) ? WHITE_PROMO_MASK : BLACK_PROMO_MASK;
    return isOccupiedAtIndex(index, mask);
}

int Board::getLeftStepIndex(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const bool isEvenRow = ((index / 4) % 2) == 0;
    const int offset = index % 4;

    if (side == Side::WHITE) {
        if (isEvenRow) return index - 4;
        return (offset == 3) ? -1 : index - 3;
    }

    if (isEvenRow) return (offset == 0) ? -1 : index + 3;
    return index + 4;
}

int Board::getRightStepIndex(int index, Side side) const {
    assert(index >= 0 && index < 32);

    const bool isEvenRow = ((index / 4) % 2) == 0;
    const int offset = index % 4;

    if (side == Side::WHITE) {
        if (isEvenRow) return (offset == 0) ? -1 : index - 5;
        return index - 4;
    }

    if (isEvenRow) return index + 4;
    return (offset == 3) ? -1 : index + 5;
}

std::vector<Move> Board::getSimpleMovesFromIndex(int index) const {
    assert(index >= 0 && index < 32);
    std::vector<Move> moves;

    if (!isFriendlyPieceAtIndex(index)) return moves;

    const Side side = side_to_move;
    const bool movingKing = isKingAtIndex(index);
    const Side oppSide = oppositeSide(side);

    auto append_move = [&](int target) {
        if (target >= 0 && target < 32 && !isOccupiedAtIndex(target, occupied())) {
            const bool promotes = !movingKing && isPromotionSquare(target, side);
            moves.push_back(Move{{index, target}, {}, promotes});
        }
    };

    // Standard forward moves
    append_move(getLeftStepIndex(index, side));
    append_move(getRightStepIndex(index, side));

    // King backward moves
    if (movingKing) {
        append_move(getLeftStepIndex(index, oppSide));
        append_move(getRightStepIndex(index, oppSide));
    }

    return moves;
}

std::vector<Move> Board::getSimpleMoves() const {
    std::vector<Move> moves;
    const Bitboard friendly = (side_to_move == Side::WHITE) ? white_pieces : black_pieces;

    for (int index = 0; index < 32; ++index) {
        if (!isOccupiedAtIndex(index, friendly)) continue;

        const std::vector<Move> pieceMoves = getSimpleMovesFromIndex(index);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return moves;
}

void Board::generateCapturesRecursive(
    int currentIndex, 
    bool isKing, 
    Bitboard capturedSoFar, 
    std::vector<int> currentPath, 
    std::vector<int> capturedIndices,
    std::vector<Move>& completedMoves
) const {
    bool furtherCapturesFound = false;
    const Side side = side_to_move;
    const Side oppSide = oppositeSide(side);

    // Ghost Board Logic: 
    // 1. Remove pieces we've already captured in this sequence.
    // 2. Clear the original start square (the moving piece is no longer there).
    // 3. Mark the currentIndex as occupied (the moving piece is here now).
    Bitboard ghostOccupied = occupied() & ~capturedSoFar;
    ghostOccupied &= ~(Bitboard(1) << currentPath.front());
    ghostOccupied |= (Bitboard(1) << currentIndex);

    const Bitboard opponents = (side == Side::WHITE) ? black_pieces : white_pieces;
    const Bitboard validOpponents = opponents & ~capturedSoFar;

    auto checkJump = [&](Side moveDir) {
        // Left Jump Check
        int targetL = getLeftStepIndex(currentIndex, moveDir);
        if (targetL >= 0 && targetL < 32 && ((validOpponents >> targetL) & 1)) {
            int landingL = getLeftStepIndex(targetL, moveDir);
            if (landingL >= 0 && landingL < 32 && !((ghostOccupied >> landingL) & 1)) {
                furtherCapturesFound = true;
                std::vector<int> newPath = currentPath;
                newPath.push_back(landingL);
                
                std::vector<int> newCaptured = capturedIndices;
                newCaptured.push_back(targetL);
                
                Bitboard newCapturedSoFar = capturedSoFar | (Bitboard(1) << targetL);

                if (!isKing && isPromotionSquare(landingL, side)) {
                    completedMoves.push_back(Move{newPath, newCaptured, true});
                } else {
                    generateCapturesRecursive(landingL, isKing, newCapturedSoFar, newPath, newCaptured, completedMoves);
                }
            }
        }

        // Right Jump Check
        int targetR = getRightStepIndex(currentIndex, moveDir);
        if (targetR >= 0 && targetR < 32 && ((validOpponents >> targetR) & 1)) {
            int landingR = getRightStepIndex(targetR, moveDir);
            if (landingR >= 0 && landingR < 32 && !((ghostOccupied >> landingR) & 1)) {
                furtherCapturesFound = true;
                std::vector<int> newPath = currentPath;
                newPath.push_back(landingR);
                
                std::vector<int> newCaptured = capturedIndices;
                newCaptured.push_back(targetR);
                
                Bitboard newCapturedSoFar = capturedSoFar | (Bitboard(1) << targetR);

                if (!isKing && isPromotionSquare(landingR, side)) {
                    completedMoves.push_back(Move{newPath, newCaptured, true});
                } else {
                    generateCapturesRecursive(landingR, isKing, newCapturedSoFar, newPath, newCaptured, completedMoves);
                }
            }
        }
    };

    // Men check forward; Kings check forward and backward
    checkJump(side);
    if (isKing) {
        checkJump(oppSide);
    }

    // Terminal condition: if no valid jumps are found from this node, the sequence is over.
    // Only save the move if we actually made at least one jump.
    if (!furtherCapturesFound && currentPath.size() > 1) {
        completedMoves.push_back(Move{currentPath, capturedIndices, false});
    }
}

std::vector<Move> Board::getCaptureMovesFromIndex(int index) const {
    assert(index >= 0 && index < 32);
    std::vector<Move> moves;

    if (!isFriendlyPieceAtIndex(index)) return moves;

    const bool isKing = isKingAtIndex(index);
    // Kick off the recursion: current path starts with 'index', 0 pieces captured so far.
    generateCapturesRecursive(index, isKing, 0, {index}, {}, moves);

    return moves;
}

std::vector<Move> Board::getCaptureMoves() const {
    std::vector<Move> moves;
    const Bitboard friendly = (side_to_move == Side::WHITE) ? white_pieces : black_pieces;

    for (int index = 0; index < 32; ++index) {
        if (!isOccupiedAtIndex(index, friendly)) continue;

        const std::vector<Move> pieceMoves = getCaptureMovesFromIndex(index);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return moves;
}

std::vector<Move> Board::getMovesFromIndex(int index) const {
    assert(index >= 0 && index < 32);

    // 1. Enforce global mandatory capture rule
    const std::vector<Move> allCaptures = getCaptureMoves();
    if (!allCaptures.empty()) {
        std::vector<Move> filtered;
        for (const Move& move : allCaptures) {
            if (!move.path.empty() && move.path.front() == index) {
                filtered.push_back(move);
            }
        }
        return filtered; // Returns empty if this piece has no captures but others do
    }

    // 2. If no captures exist globally, return simple moves for this piece
    return getSimpleMovesFromIndex(index);
}

std::vector<Move> Board::getLegalMoves() const {
    const std::vector<Move> captures = getCaptureMoves();
    if (!captures.empty()) return captures;

    return getSimpleMoves();
}

uint64_t Board::perft(int depth) const {
    // 1. BASE CASE: 
    // If the depth is 0, we have reached the end of this specific timeline.
    // We are looking at exactly one board state (this one). Return the correct count.
    if (depth == 0) {
        return 1; // We count this leaf node as 1.
    }

    // 2. GENERATE MOVES:
    // Fetch all legal moves for the current board state.
    // (Your getLegalMoves() already perfectly handles the mandatory capture rule).
    const std::vector<Move> legalMoves = getLegalMoves();


    // 3. INITIALIZE COUNTER:
    // Set up a 64-bit unsigned integer to keep a running tally of leaf nodes.
    uint64_t leafNodeCount = 0;

    // 4. THE RECURSIVE LOOP:
    // Iterate through every move in your list of legal moves.
    // For each move:
    for (const Move& move : legalMoves) {
        
        // a. Copy the current state: Create a completely new Board object initialized 
        //    to the current board (*this).
        Board nextBoard = *this; 

        // b. Advance the state: Call makeMove(...) on the *copied* board using the current move.
        nextBoard.makeMove(move);

        // c. Recurse: Call perft(depth - 1) on the *copied* board.
        uint64_t nodesFromThisMove = nextBoard.perft(depth - 1);

        // d. Accumulate: Add the returned value from the recursive call to your counter.
        leafNodeCount += nodesFromThisMove;
    }

    // 5. RETURN:
    // Return the final tally of nodes.
    return leafNodeCount;
}

void Board::makeMove(const Move& move) {
    const int from = move.path.front();
    const int to = move.path.back();

    Bitboard& friendly = (side_to_move == Side::WHITE) ? white_pieces : black_pieces;
    Bitboard& opponent = (side_to_move == Side::WHITE) ? black_pieces : white_pieces;

    friendly &= ~(Bitboard(1) << from);
    friendly |= (Bitboard(1) << to);

    if (isKingAtIndex(from)) {
        kings &= ~(Bitboard(1) << from);
        kings |= (Bitboard(1) << to);
    }

    for (int capturedIndex : move.captured) {
        opponent &= ~(Bitboard(1) << capturedIndex);
        kings &= ~(Bitboard(1) << capturedIndex);
    }

    if (move.promotes) {
        kings |= (Bitboard(1) << to);
    }

    side_to_move = oppositeSide(side_to_move);
}

Board Board::applyMove(const Move& move) const {
    Board boardCopy = *this;
    boardCopy.makeMove(move);
    return boardCopy;
}


void Board::divide(int depth) const {
    std::cout << "--- Divide Perft (Depth " << depth << ") ---\n";
    
    std::vector<Move> moves = getLegalMoves();
    uint64_t totalNodes = 0;

    for (const Move& move : moves) {
        Board nextBoard = *this;
        nextBoard.makeMove(move);
        
        uint64_t nodes = nextBoard.perft(depth - 1);
        totalNodes += nodes;

        // Print the start and end squares of the move, plus its node count
        std::cout << "Move [" << move.path.front() << " -> " << move.path.back() 
                  << "]: " << nodes << " nodes\n";
    }
    
    std::cout << "Total Nodes: " << totalNodes << "\n";
}