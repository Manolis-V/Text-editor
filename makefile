# # Makefile for the C++ text editor project
# # THIS WORKS
# # Compiler and compiler flags
# CXX = g++
# CXXFLAGS = -Wall -std=c++17

# # Name of the output executable
# TARGET = texteditor

# # Source files
# SRCS = texteditor.cpp
# #SRCS = test.cpp
# # Object files (generated from source files)
# OBJS = $(SRCS:.cpp=.o)

# # Libraries
# LIBS = -lncurses

# # Default target to build the editor
# all: $(TARGET)

# # Rule to link the object files and create the executable
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# # Rule to compile .cpp files into .o files
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $<

# # Rule to run the program
# run: $(TARGET)
# 	./$(TARGET)

# # Clean up the build
# clean:
# 	rm -f $(OBJS) $(TARGET)

# # Phony targets (these do not correspond to actual files)
# .PHONY: all clean


# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Flex and Bison tools
FLEX = flex
FLEX_SRC = syntax_highlighter.l
FLEX_OUTPUT = syntax_highlighter.cpp

# Executable name
TARGET = syntax_highlighter

# ncurses library for color highlighting
LIBS = -lncurses

# Object files
OBJ = syntax_highlighter.o texteditor.o

# Build the final executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Compile the lexer from the Flex file
syntax_highlighter.o: $(FLEX_SRC)
	$(FLEX) -o $(FLEX_OUTPUT) $(FLEX_SRC)
	$(CXX) $(CXXFLAGS) -c $(FLEX_OUTPUT)

# Compile the test program
texteditor.o: texteditor.cpp
	$(CXX) $(CXXFLAGS) -c texteditor.cpp

# Rule to run the program
run: $(TARGET)
	./$(TARGET)

# Clean the build
clean:
	rm -f $(OBJ) $(TARGET) $(FLEX_OUTPUT)