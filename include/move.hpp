#ifndef MOVE_HPP
#define MOVE_HPP
#include <vector>

struct SimpleMove {
    int from;
    int to;
};

struct CaptureMove {
    int from;
    int to;
    int captured;
};

struct CaptureStep {
    int captured;
    int landing;
};

#endif