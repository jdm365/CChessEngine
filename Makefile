## Makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

## CXX = clang
CXX = gcc
CXXFLAGS = -std=c2x
CXXFLAGS += -Wall -g -O3 -ffast-math
CXXFLAGS += -mpopcnt -march=native -mtune=native
CXXFLAGS += -Wno-format
OS = $(shell uname -s)
ifeq ($(OS), Darwin)
	LDFLAGS = -Xpreprocessor -fopenmp -lomp
else
	LDFLAGS = -fopenmp
endif

SRCS = src/*.c
INCLUDES = -I./src/*.h
TARGET = bin/release/$(PROJECT_NAME)
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

NNUE_SRCS     = src/nnue/*.c
NNUE_INCLUDES = -I./src/nnue/*.h
SRCS     += $(NNUE_SRCS)
INCLUDES += $(NNUE_INCLUDES)


all:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(LIBS) -o $(TARGET)

run: all
	./$(TARGET)

sanitize:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(LIBS) -fsanitize=address -o $(TARGET)
	./$(TARGET)

profile:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(LIBS) -pg -o $(TARGET)
	./$(TARGET)
	gprof $(TARGET) gmon.out > analysis.txt


clean:
	rm -f ./bin./release/*
