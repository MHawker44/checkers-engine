#include <cassert>
#include "move.hpp"

void run_move_tests() {
    const SimpleMove simple{12, 16};
    assert(simple.from == 12);
    assert(simple.to == 16);

    const CaptureMove capture{21, 14, 17};
    assert(capture.from == 21);
    assert(capture.to == 14);
    assert(capture.captured == 17);

    const CaptureStep step{17, 14};
    assert(step.captured == 17);
    assert(step.landing == 14);
}
