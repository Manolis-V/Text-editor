#include <string>
#include <ncurses.h>
#include <iostream>

extern void runLexer(const std::string& input, int startRow); // Updated function declaration

int main() {
    initscr();             // Start ncurses mode
    start_color();         // Start color functionality in ncurses
    init_pair(1, COLOR_RED, COLOR_BLACK);      // For keywords like "int", "float"
    init_pair(2, COLOR_GREEN, COLOR_BLACK);    // For strings
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);   // For comments
    init_pair(4, COLOR_BLUE, COLOR_BLACK);     // For numbers
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);  // For other identifiers

    int ch;
    std::string input;
    int startRow = 0;  // Specify the starting row (you can modify it as needed)

    while ((ch = getch()) != '\n') {  // Real-time input handling
        if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
            if (!input.empty()) {
                input.pop_back();
            }
        } else {
            input += ch;  // Append character to input
        }

        clear();  // Clear the screen before redrawing

        // Pass the current input and starting row to the lexer for colorization
        runLexer(input, startRow);

        refresh();  // Refresh the screen to reflect the updates
    }

    getch();
    endwin();  // End ncurses mode
    return 0;
}