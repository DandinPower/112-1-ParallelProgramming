# Makefile

# Compiler
CC = g++

# Compiler flags
CFLAGS = -fopenmp

# Target executable name
TARGET = example

all: $(TARGET)

$(TARGET): example_worksharing.cpp
	$(CC) $(CFLAGS) -o $(TARGET) example_worksharing.cpp

clean:
	rm -f $(TARGET)