## Makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

CXX = clang
## CXX = gcc
CXXFLAGS = -Wall -g -O3 -ffast-math -march=native
CXXFLAGS += -mpopcnt
CXXFLAGS += -Wno-format
OS = $(shell uname -s)
ifeq ($(OS), Darwin)
	LDFLAGS = -Xpreprocessor -fopenmp -lomp
else
	LDFLAGS = -fopenmp
endif

SRCS = src/*.c
INCLUDES = -Isrc/*.h
TARGET = bin/release/$(PROJECT_NAME)
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

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
