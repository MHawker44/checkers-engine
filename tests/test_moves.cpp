#include <cassert>
#include <vector>

#include "core/move.hpp"

void run_move_tests() {
    const Move simple{{12, 16}, {}, false};
    assert(simple.path.size() == 2);
    assert(simple.path.front() == 12);
    assert(simple.path.back() == 16);
    assert(simple.captured.empty());
    assert(!simple.promotes);

    const Move capture{{21, 14, 7}, {17, 10}, true};
    assert(capture.path.size() == 3);
    assert(capture.path.front() == 21);
    assert(capture.path.back() == 7);
    assert(capture.captured.size() == 2);
    assert(capture.captured[0] == 17);
    assert(capture.captured[1] == 10);
    assert(capture.promotes);

    assert(capture.captured.size() == capture.path.size() - 1);
}
