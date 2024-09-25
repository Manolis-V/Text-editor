# Makefile for the C++ text editor project

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Name of the output executable
TARGET = texteditor

# Source files
SRCS = texteditor.cpp

# Object files (generated from source files)
OBJS = $(SRCS:.cpp=.o)

# Libraries
LIBS = -lncurses

# Default target to build the editor
all: $(TARGET)

# Rule to link the object files and create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Rule to run the program
run: $(TARGET)
	./$(TARGET)

# Clean up the build
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (these do not correspond to actual files)
.PHONY: all clean
