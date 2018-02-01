CC := clang++
CFLAGS := -std=c++14 -I include -g -Wall
SOURCES := $(wildcard src/*.cpp)
TEST_SOURCES := $(wildcard test/*.cpp)
TARGET := bin/ai_run
TEST_TARGET := bin/ai_test

ai_run: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

ai_test: $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(filter-out src/backendMain.cpp, $(SOURCES))


