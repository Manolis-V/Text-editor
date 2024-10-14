#include <ncurses.h>
#include <string.h>

void highlight_selection(const char *text, int start, int end);
void delete_selection(char *text, int start, int end);

int main() {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);
    
    // Sample content to edit
    char text[1024] = "This is a sample text. You can mark and delete this text.";
    int len = strlen(text);

    int cursor_pos = 0;  // Current cursor position
    int mark_start = -1; // Start of marked text (-1 means no selection)
    int mark_end = -1;   // End of marked text

    int ch;
    while ((ch = getch()) != 'q') {  // Press 'q' to quit
        switch (ch) {
            case KEY_LEFT:
                if (cursor_pos > 0) cursor_pos--;
                mark_start = mark_end = -1;  // Reset selection
                break;
            case KEY_RIGHT:
                if (cursor_pos < len) cursor_pos++;
                mark_start = mark_end = -1;  // Reset selection
                break;
            case 393:  // Ctrl + Left Arrow (mark text to the left)
                if (mark_start == -1) {
                    mark_start = cursor_pos;
                }
                if (cursor_pos > 0) cursor_pos--;
                mark_end = cursor_pos;
                break;
            case 402:  // Ctrl + Right Arrow (mark text to the right)
                if (mark_start == -1) {
                    mark_start = cursor_pos;
                }
                if (cursor_pos < len) cursor_pos++;
                mark_end = cursor_pos;
                break;
            case KEY_BACKSPACE:  // Backspace/Delete
                if (mark_start != -1 && mark_end != -1) {
                    // If text is marked, delete the marked selection
                    delete_selection(text, mark_start, mark_end);
                    cursor_pos = mark_start;
                    len = strlen(text);
                    mark_start = mark_end = -1;  // Reset selection
                }
                break;
        }

        // Clear the screen and display updated text
        clear();
        mvprintw(0, 0, text);

        // // Highlight the selected text if any
        if (mark_start != -1 && mark_end != -1) {
            highlight_selection(text, mark_start, mark_end);
        }

        // Move the cursor to the current position
        move(0, cursor_pos);
        refresh();
    }

    endwin();
    return 0;
}

void highlight_selection(const char *text, int start, int end) {
    // Highlight the text between start and end    
    attron(A_REVERSE);  // Highlight selected text with reverse video
    if (start < end){
        for (int i = start; i <= end-1; i++) {
            mvprintw(0, i, "%c", text[i]);
        }
    } else {
        for (int i = start - 1; i >= end; i--) {
            mvprintw(0, i, "%c", text[i]);
        }
    }
    attroff(A_REVERSE);
}

void delete_selection(char *text, int start, int end) {
    // Remove the text between start and end by shifting the text
    memmove(&text[start], &text[end], strlen(text) - end - 1);
}