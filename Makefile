CXX = g++

CXXFLAGS_DEBUG = -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -g -fsanitize=address,undefined -Iinclude
LDFLAGS_DEBUG = -fsanitize=address,undefined

CXXFLAGS_RELEASE = -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -O3 -DNDEBUG -Iinclude
LDFLAGS_RELEASE =

SRC = \
	src/main.cpp \
	src/core/board.cpp \
	src/core/move.cpp \
	src/app/game.cpp \
	src/engine/evaluator.cpp \
	src/engine/search.cpp \
	src/ui/gui.cpp \
	src/ui/renderer.cpp

APP = build/checkers

debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: LDFLAGS = $(LDFLAGS_DEBUG)
debug: $(APP)

release: CXXFLAGS = $(CXXFLAGS_RELEASE)
release: LDFLAGS = $(LDFLAGS_RELEASE)
release: $(APP)

$(APP): $(SRC)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(APP) $(LDFLAGS)

run: debug
	./$(APP) $(ARGS)

run-release: release
	./$(APP) $(ARGS)

clean:
	rm -rf build