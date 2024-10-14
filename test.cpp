// #include <ncurses.h>
// #include <string.h>

// void highlight_selection(const char *text, int start, int end);
// void delete_selection(char *text, int start, int end);

// int main() {
//     initscr();
//     noecho();
//     keypad(stdscr, TRUE);
//     curs_set(1);
    
//     // Sample content to edit
//     char text[1024] = "This is a sample text. You can mark and delete this text.";
//     int len = strlen(text);

//     int cursor_pos = 0;  // Current cursor position
//     int mark_start = -1; // Start of marked text (-1 means no selection)
//     int mark_end = -1;   // End of marked text

//     int ch;
//     while ((ch = getch()) != 'q') {  // Press 'q' to quit
//         switch (ch) {
//             case KEY_LEFT:
//                 if (cursor_pos > 0) cursor_pos--;
//                 mark_start = mark_end = -1;  // Reset selection
//                 break;
//             case KEY_RIGHT:
//                 if (cursor_pos < len) cursor_pos++;
//                 mark_start = mark_end = -1;  // Reset selection
//                 break;
//             case 393:  // Ctrl + Left Arrow (mark text to the left)
//                 if (mark_start == -1) {
//                     mark_start = cursor_pos;
//                 }
//                 if (cursor_pos > 0) cursor_pos--;
//                 mark_end = cursor_pos;
//                 break;
//             case 402:  // Ctrl + Right Arrow (mark text to the right)
//                 if (mark_start == -1) {
//                     mark_start = cursor_pos;
//                 }
//                 if (cursor_pos < len) cursor_pos++;
//                 mark_end = cursor_pos;
//                 break;
//             case KEY_BACKSPACE:  // Backspace/Delete
//                 if (mark_start != -1 && mark_end != -1) {
//                     // If text is marked, delete the marked selection
//                     delete_selection(text, mark_start, mark_end);
//                     cursor_pos = mark_start;
//                     len = strlen(text);
//                     mark_start = mark_end = -1;  // Reset selection
//                 }
//                 break;
//         }

//         // Clear the screen and display updated text
//         clear();
//         mvprintw(0, 0, text);

//         // // Highlight the selected text if any
//         if (mark_start != -1 && mark_end != -1) {
//             highlight_selection(text, mark_start, mark_end);
//         }

//         // Move the cursor to the current position
//         move(0, cursor_pos);
//         refresh();
//     }

//     endwin();
//     return 0;
// }

// void highlight_selection(const char *text, int start, int end) {
//     // Highlight the text between start and end    
//     attron(A_REVERSE);  // Highlight selected text with reverse video
//     if (start < end){
//         for (int i = start; i <= end-1; i++) {
//             mvprintw(0, i, "%c", text[i]);
//         }
//     } else {
//         for (int i = start - 1; i >= end; i--) {
//             mvprintw(0, i, "%c", text[i]);
//         }
//     }
//     attroff(A_REVERSE);
// }

// void delete_selection(char *text, int start, int end) {
//     // Remove the text between start and end by shifting the text
//     memmove(&text[start], &text[end], strlen(text) - end - 1);
// }

#include <ncurses.h>
#include <iostream>
// Macro to help with Ctrl key combinations
#define CTRL(x) ((x) & 0x1f) // Ctrl + x is x with the high bit cleared


int main() {
    initscr();            // Start ncurses mode
    keypad(stdscr, TRUE); // Enable function and arrow keys
    noecho();             // Don't echo pressed keys

    int ch;

    printw("Press Ctrl + Right Arrow or Ctrl + Left Arrow to test.\nPress 'q' to quit.\n");

    while ((ch = getch()) != 'q') { // Exit on 'q'
        // Check for Ctrl + Right Arrow
        if (ch == CTRL('f')) { // Typically Ctrl + Right Arrow is interpreted as Ctrl + 'f'
            printw("Ctrl + Right Arrow detected!\n");
        }
        // Check for Ctrl + Left Arrow
        else if (ch == CTRL('b')) { // Typically Ctrl + Left Arrow is interpreted as Ctrl + 'b'
            printw("Ctrl + Left Arrow detected!\n");
        } else {
            printw("Key pressed: %d\n", ch); // Print the ASCII value of other keys
        }
        refresh(); // Refresh the window to show changes
    }

    endwin(); // End ncurses mode
    return 0;
}
