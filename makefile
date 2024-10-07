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

###############################################################
# # Makefile for the C++ text editor project
# DOESNT WORK 
# # Compiler and compiler flags
# CXX = g++
# CXXFLAGS = -Wall -std=c++17

# # Flex lexer generator
# LEX = flex

# # Name of the output executable
# TARGET = texteditor

# # Source files (add the .l file as a source)
# SRCS = test.cpp syntax_highlighter.cpp

# # Object files (generated from source files)
# OBJS = $(SRCS:.cpp=.o)

# # Libraries
# LIBS = -lncurses

# # Flex source (the .l file)
# LEXER_SRC = syntax_highlighter.l

# # Generated C++ file from Flex
# LEXER_CPP = syntax_highlighter.cpp

# # Default target to build the editor
# all: $(TARGET)

# # Rule to link the object files and create the executable
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# # Rule to compile .cpp files into .o files
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $<

# # Rule to generate the C++ lexer file from the .l Flex file
# $(LEXER_CPP): $(LEXER_SRC)
# 	$(LEX) --c++ -o $(LEXER_CPP) $(LEXER_SRC)

# # Include the lexer source in the build process
# $(OBJS): $(LEXER_CPP)

# # Rule to run the program
# run: $(TARGET)
# 	./$(TARGET)

# # Clean up the build
# clean:
# 	rm -f $(OBJS) $(TARGET) $(LEXER_CPP)

# # Phony targets (these do not correspond to actual files)
# .PHONY: all clean run
#################################################################################

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
OBJ = syntax_highlighter.o test.o

# Build the final executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Compile the lexer from the Flex file
syntax_highlighter.o: $(FLEX_SRC)
	$(FLEX) -o $(FLEX_OUTPUT) $(FLEX_SRC)
	$(CXX) $(CXXFLAGS) -c $(FLEX_OUTPUT)

# Compile the test program
test.o: test.cpp
	$(CXX) $(CXXFLAGS) -c test.cpp

# Rule to run the program
run: $(TARGET)
	./$(TARGET)

# Clean the build
clean:
	rm -f $(OBJ) $(TARGET) $(FLEX_OUTPUT)
