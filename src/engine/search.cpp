// search.cpp
#include "engine/search.hpp"
#include "engine/evaluator.hpp"

namespace {
constexpr int INF = 1000000;
constexpr int MIN_EVAL = -INF;
constexpr int MAX_EVAL = INF;
}

Search::Search(const Evaluator& evaluator)
    : evaluator(evaluator) {}

Move Search::findBestMoveAtDepth(const Board& board, int depth) const {
    assert(depth > 0);
    std::vector<Move> moves = board.getLegalMoves();
    assert(!moves.empty()); // Should never call this on a terminal position
    
    Move bestMove = moves.front();
    int bestEval = MIN_EVAL;

    for (const Move& move : moves) {
        Board nextBoard = board.applyMove(move);
        int eval = -negamaxStaticDepth(nextBoard, depth - 1, -MAX_EVAL, -bestEval);
        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}

int Search::negamaxStaticDepth(const Board& board, int depth, int alpha, int beta) const {
    if (depth == 0) {
        int eval = evaluator.evaluate(board);
        return (board.getSideToMove() == Side::BLACK) ? eval : -eval;
    }
    std::vector<Move> moves = board.getLegalMoves();
    if (moves.empty()) {
        return MIN_EVAL;
    }

    int maxEval = MIN_EVAL;
    for (const Move& move : moves) {
        Board nextBoard = board.applyMove(move);
        int eval = -negamaxStaticDepth(nextBoard, depth - 1, -beta, -alpha);
        if (eval > maxEval) {
            maxEval = eval;
        }
        if (maxEval > alpha) {
            alpha = maxEval;
        }
        if (alpha >= beta) {
            break; // Beta cut-off
        }
    }
    return maxEval;
}
