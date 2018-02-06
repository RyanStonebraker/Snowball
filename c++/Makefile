CC := clang++
CFLAGS := -std=c++14 -I include -I dependencies -g -Wall -O3
SOURCES := $(wildcard src/*.cpp)
TEST_SOURCES := $(wildcard test/*.cpp)
TARGET := bin/ai_run
GUI_TARGET := ../index.js
TEST_TARGET := bin/ai_test
TEMP_OBJECTS := $(wildcard ../comm/currentgame/turn*.txt) $(wildcard ../comm/handshake*.txt)

ai_run: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

front_run:
	electron $(GUI_TARGET)

build_run: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) && $(RUN)

run_ai:
	$(TARGET)

run :
	electron $(GUI_TARGET) &
	$(TARGET)

ai_test: $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(filter-out src/backendMain.cpp, $(SOURCES))

.PHONY : clean
clean :
	-rm $(TEMP_OBJECTS)
