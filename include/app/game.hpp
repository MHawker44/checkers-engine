#ifndef GAME_HPP
#define GAME_HPP

#include "core/board.hpp"

class Game {
public:
    void run();

private:
    Board board_;
};

#endif