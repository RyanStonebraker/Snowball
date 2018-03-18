CC := clang++
CFLAGS := -std=c++14 -I include -I ../dependencies -Wall
OPTIMALPERF := -O3
DBGFLAGS := -g -O0
SOURCES := $(wildcard src/*.cpp)
TEST_SOURCES := $(wildcard test/*.cpp)
TARGET := bin/ai_run
GUI_TARGET := ../index.js
TEST_TARGET := bin/ai_test
TEMP_OBJECTS := $(wildcard ../comm/currentgame/turn*.txt) $(wildcard ../comm/handshake*.txt)
GENS := $(wildcard ../training/*)

# User Vars
depth = 2
wFile = ""
evolve = "evolve"
generations = 2

build: $(SOURCES)
	$(CC) $(CFLAGS) $(OPTIMALPERF) -o $(TARGET) $(SOURCES)

debug: $(SOURCES)
	$(CC) $(CFLAGS) $(DBGFLAGS) -o $(TARGET) $(SOURCES)

front_run:
	electron $(GUI_TARGET)

run_ai:
	$(TARGET) $(depth) $(wFile)

run:
	electron $(GUI_TARGET) &
	$(TARGET) $(depth) $(wFile)

evolve:
	$(TARGET) $(depth) $(evolve) $(generations)

run_tests:
	$(TEST_TARGET)

tests: $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(filter-out src/backendMain.cpp, $(SOURCES))

.PHONY : clean
clean :
	-rm $(TEMP_OBJECTS)

remove_all_generations :
	-rm -r $(GENS)
