# Makefile

# Compiler
CC = g++

# Compiler flags
CFLAGS = -fopenmp

# Target executable name
TARGET = example

all: $(TARGET)

$(TARGET): example_synch.cpp
	$(CC) $(CFLAGS) -o $(TARGET) example_synch.cpp

clean:
	rm -f $(TARGET)