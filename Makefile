## Makefile
PROJECT_NAME = $(shell basename "$(realpath ./)")

CXX = clang
CXXFLAGS = -Wall -g -O3
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

clean:
	rm -f $(TARGET)
