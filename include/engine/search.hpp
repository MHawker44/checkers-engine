// search.hpp
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <optional>
#include "engine/evaluator.hpp"

struct SearchResult {
    Move bestMove;
    int score;
};

class Search {
public:
    // placeholder
    explicit Search(const Evaluator& evaluator);
    Move findBestMoveAtDepth(const Board& board, int depth) const;

private:
    const Evaluator& evaluator;

    int negamaxStaticDepth(const Board& board, int depth, int alpha, int beta) const;
};

#endif