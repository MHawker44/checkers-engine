#include <iostream>

void run_board_tests();
void run_move_tests();

int main() {
    run_board_tests();
    run_move_tests();
    std::cout << "All tests completed.\n";
    return 0;
}