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