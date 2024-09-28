#include <ncurses.h>

int main() {
    initscr();
    start_color();
    
    if (COLORS >= 256) {
        // Print all 256 colors
        for (int i = 0; i < 256; ++i) {
            init_pair(i + 1, COLOR_WHITE, i); // White text, background as the color
            attron(COLOR_PAIR(i + 1));
            mvprintw(i % 24, (i / 24) * 10, "Color %d", i); // Organize them in columns
            attroff(COLOR_PAIR(i + 1));
        }
        refresh();
        getch();  // Wait for user input
    } else {
        printw("256-color mode not supported by your terminal.\n");
        refresh();
        getch();
    }

    endwin();
    return 0;
}
