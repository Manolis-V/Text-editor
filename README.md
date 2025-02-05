# Vim-like Text Editor

## Overview

This is a Text Editor using Flex (Lex), C++, and Ncurses. It reads text input, applies syntax highlighting based on predefined patterns, and displays it in a terminal-based user interface. The project supports real-time text navigation, selection, and editing, with an intuitive interface for handling syntax coloring and user interactions.

## Features

:white_check_mark: **Syntax Highlighting**: Recognizes keywords, comments, numbers, and function names, displaying them in different colors.

:white_check_mark: **Ncurses-Based Interface**: Provides an interactive text editor-like environment with cursor movement and text rendering.

:white_check_mark: **Text Selection**: Allows users to select text using ```Shift + Arrow Keys``` and delete selected content.

:white_check_mark: **Shortcuts**: Supports keyboard shortcuts such as ```Ctrl + Arrow Keys``` for extended text navigation.

## Installation & Compilation

### Prerequisites

Ensure you have the following installed on your system:
- ```g++``` (C++17 or later)
- ```Flex``` (Fast Lexical Analyzer)
- ```Ncurses``` (for terminal UI rendering)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/your-repository/syntax-highlighter.git
cd syntax-highlighter
```
2. Compile and run the project using the provided Makefile:
```bash
make run
```

## Usage

- **Typing & Syntax Highlighting**: Start typing in the terminal, and the text will be automatically highlighted.
- **Navigation**: Use the arrow keys to move the cursor.
- **Selecting Text**:
  1. Hold ```Shift + Arrow Keys``` to select text.
  2. Press ```Ctrl + Arrow Keys``` for faster navigation.
- **Deleting Selected Text**: Press Backspace to remove the selection.
- **Save Changes**: Press ```:w```.
- **Save and Exit**: Press ```:wq```.
- **Back to Menu**: Press ```:m```.
- **Exit the Program**: Press ```:q```.

## File Structure

```bash
📂 syntax-highlighter/
│── 📜 README.md            # Project documentation
│── 📜 makefile             # Build automation script
│── 📜 syntax_highlighter.l # Flex lexer definitions
│── 📜 syntax_highlighter.cpp # Main C++ program
│── 📜 syntax_highlighter.h   # Header file
│── 📜 test.cpp             # Example usage file
```

## Customization

You can modify the syntax rules in ```syntax_highlighter.l``` to recognize additional programming constructs. To extend functionality:
- Edit the Flex file to define new token patterns.
- Modify ```apply_color(int color)``` in ```syntax_highlighter.cpp``` to adjust color schemes.
- Recompile the project using ```make clean && make run```.
