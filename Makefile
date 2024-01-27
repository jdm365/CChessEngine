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

all:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
