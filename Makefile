CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -g -fsanitize=address,undefined -Iinclude
LDFLAGS = -fsanitize=address,undefined

SRC = src/main.cpp src/board.cpp src/move.cpp src/game.cpp
TEST_SRC = tests/test_main.cpp tests/test_board.cpp tests/test_moves.cpp src/board.cpp src/move.cpp src/game.cpp

APP = build/checkers
TEST_APP = build/tests

all: $(APP)

$(APP): $(SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(APP) $(LDFLAGS)

test: $(TEST_APP)
	./$(TEST_APP)

$(TEST_APP): $(TEST_SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_APP) $(LDFLAGS)

run: $(APP)
	./$(APP)

clean:
	rm -rf build

re: clean all