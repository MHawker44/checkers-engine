#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "app/game.hpp"
#include "engine/evaluator.hpp"
#include "engine/search.hpp"

namespace {

void printUsage(const char* programName) {
    std::cout
        << "Usage:\n"
        << "  " << programName << " print\n"
        << "  " << programName << " perft <depth>\n"
        << "  " << programName << " divide <depth>\n"
        << "  " << programName << " play <plies> <depth>\n";
}

bool parseNonNegativeInt(const char* text, int& value) {
    try {
        const int parsed = std::stoi(text);
        if (parsed < 0) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

int runPrint(const Board& board) {
    std::cout << "Initial position:\n";
    board.print();

    const std::vector<Move> legalMoves = board.getLegalMoves();
    std::cout << "\nSide to move legal moves: " << legalMoves.size() << '\n';

    return 0;
}

int runPerft(const Board& board, int depth) {
    const auto start = std::chrono::steady_clock::now();
    const uint64_t nodes = board.perft(depth);
    const auto end = std::chrono::steady_clock::now();

    const auto elapsedNs =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    const double elapsedSeconds = static_cast<double>(elapsedNs) / 1'000'000'000.0;
    const double nodesPerSecond =
        (elapsedSeconds > 0.0) ? static_cast<double>(nodes) / elapsedSeconds : 0.0;

    std::cout << "=== PERFT RESULT ===\n";
    std::cout << "Depth:           " << depth << '\n';
    std::cout << "Nodes:           " << nodes << '\n';
    std::cout << "Elapsed (sec):   " << elapsedSeconds << '\n';
    std::cout << "Nodes / second:  " << static_cast<uint64_t>(nodesPerSecond) << '\n';

    return 0;
}

int runDivide(const Board& board, int depth) {
    std::cout << "=== DIVIDE RESULT ===\n";
    std::cout << "Depth: " << depth << "\n\n";
    board.divide(depth);
    return 0;
}

int runPlayStaticDepth(int plies, int depth) {
    Board board;

    Evaluator evaluator;
    Search search(evaluator);

    std::cout << "=== Initial Position ===\n";
    board.print();

    for (int ply = 0; ply < plies; ++ply) {
        const std::vector<Move> legalMoves = board.getLegalMoves();
        if (legalMoves.empty()) {
            std::cout << "\n=== Game Over at ply " << ply << " ===\n";
            std::cout << "No legal moves for "
                      << (board.getSideToMove() == Side::BLACK ? "Black" : "White")
                      << '\n';
            return 0;
        }

        std::cout << "\n=== Ply " << ply << " ===\n";
        std::cout << "Side to move: "
                  << (board.getSideToMove() == Side::BLACK ? "Black" : "White")
                  << '\n';

        Move bestMove = search.findBestMoveAtDepth(board, depth);

        std::cout << "Chosen move: "
                  << bestMove.path.front() << " -> " << bestMove.path.back() << '\n';

        board = board.applyMove(bestMove);
        board.print();
    }

    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    Board board;

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string command = argv[1];

    if (command == "print") {
        return runPrint(board);
    }

    if (command == "perft" || command == "divide") {
        if (argc < 3) {
            printUsage(argv[0]);
            return 1;
        }

        int depth = 0;
        if (!parseNonNegativeInt(argv[2], depth)) {
            std::cerr << "Invalid depth: " << argv[2] << '\n';
            return 1;
        }

        if (command == "perft") {
            return runPerft(board, depth);
        }

        return runDivide(board, depth);
    }

    if (command == "play") {
        if (argc < 4) {
            printUsage(argv[0]);
            return 1;
        }

        int plies = 0;
        int depth = 0;

        if (!parseNonNegativeInt(argv[2], plies)) {
            std::cerr << "Invalid plies: " << argv[2] << '\n';
            return 1;
        }

        if (!parseNonNegativeInt(argv[3], depth)) {
            std::cerr << "Invalid depth: " << argv[3] << '\n';
            return 1;
        }

        return runPlayStaticDepth(plies, depth);
    }

    printUsage(argv[0]);
    return 1;
}