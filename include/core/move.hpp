// move.hpp
#ifndef MOVE_HPP
#define MOVE_HPP
#include <vector>
#include <cassert>
#include "core/types.hpp"


struct Move {
    std::vector<int> path;
    std::vector<int> captured;
    bool promotes;
};

#endif