#include <cassert>
#include "move.hpp"

void run_move_tests() {
    Move move{0, 1, 1, 2};
    assert(move.from_row == 0);
    assert(move.to_col == 2);
}