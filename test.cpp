#include <ncurses.h>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <dirent.h>
//g++ -std=c++17 -lncurses -o test test.cpp

using namespace std;
bool tabsVisible = true; // To track the visibility of the tabs
// Function to draw the tabs on the left side
void drawTabs(const vector<string>& tabs, int selectedTab) {
    for (size_t i = 0; i < tabs.size(); ++i) {
        if (i == selectedTab) {
            attron(A_REVERSE); // Highlight the selected tab
        }
        mvprintw(i + 1, 1, "%s", tabs[i].c_str()); // Draw each tab
        if (i == selectedTab) {
            attroff(A_REVERSE); // Turn off highlighting
        }
    }
}

// Function to display a message on the right side
void displayMessage(const string& message) {
    
    if (tabsVisible) {
        mvprintw(1, 25, "%s", message.c_str()); // Display message starting at column 25
    } else {
        mvprintw(1, 1, "%s", message.c_str()); // Display message starting at column 1 when tabs are hidden
    }
}

// Function to draw a vertical separator
void drawSeparator(bool sep) {
    if (sep) {
        for (int i = 0; i < LINES; ++i) {
            mvaddch(i, 24, '|'); // Draw a vertical line at column 24
        }
    } else {
        for (int i = 0; i < LINES; ++i) {
            mvaddch(i, 24, ' '); // Clear the separator
        }
    }
}

// Function to display file contents in the right panel
void displayFileContent(const string& filename) {
    clear(); // Clear the screen

    ifstream file(filename);
    if (!file.is_open()) {
        mvprintw(0, 25, "Error opening file: %s", filename.c_str());
        refresh();
        getch(); // Wait for user input
        return; // Exit function
    }

    string line;
    int row = 0;
    while (getline(file, line)) {
        mvprintw(row, 25, "%s", line.c_str()); // Display each line of the file
        row++;
        if (row >= LINES - 1) { // Prevent overflow
            break; // Stop if we reach the bottom of the window
        }
    }

    file.close();
    refresh(); // Refresh to show file contents
}

int main() {
    initscr();                 // Initialize ncurses
    noecho();                  // Do not echo input characters
    keypad(stdscr, TRUE);      // Enable special keys (like arrow keys)
    curs_set(0);               // Hide the cursor for a cleaner look

    vector<string> tabs;       // Vector to hold tab names
    const string directory = "."; // Current directory

    
    // Read all files in the current directory using dirent.h
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(".")) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) {  // Only regular files
                tabs.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }

    if (tabs.empty()) {
        mvprintw(1, 1, "No files found in the directory."); // Message if no files
        refresh();
        getch(); // Wait for a key press
        endwin(); // End ncurses mode
        return 0;
    }

    int selectedTab = 0; // Index of the selected tab
    string message = "Press LEFT/RIGHT to navigate tabs, 'Ctrl + H' to toggle tabs, 'Enter' to open file"; // Instructions message
    

    // Draw the initial screen
    drawTabs(tabs, selectedTab);
    drawSeparator(true);
    refresh(); // Initial refresh

    // Main loop for user input
    int ch;
    while (true) {
        ch = getch(); // Wait for user input
        if (ch == 'q') break; // Exit on 'q'

        // Check for Ctrl + H
        if (ch == 8) { // 8 is the ASCII code for Ctrl + H

            tabsVisible = !tabsVisible; // Toggle visibility
            clear();
            if (tabsVisible) {
                drawTabs(tabs, selectedTab); // Draw tabs if visible
                drawSeparator(true); // Draw separator if tabs are visible
            } else {
                drawSeparator(false); // Clear the separator if tabs are hidden
            }
            //displayMessage(message, tabsVisible); // Redisplay message
            refresh(); // Refresh to show changes
            continue; // Skip the rest of the loop
        }
        // Handle input and update selected tab
        int oldSelectedTab = selectedTab; // Store old selection for comparison
        switch (ch) {
            case KEY_UP:
                if (selectedTab > 0) {
                    selectedTab--;
                    displayFileContent(tabs[selectedTab]);
                }
                break;
            case KEY_DOWN:
                if (selectedTab < tabs.size() - 1) {
                    selectedTab++;
                    displayFileContent(tabs[selectedTab]);
                }
                break;
            case 10: // Enter key
                clear();
                if (tabsVisible) { // Only open file if tabs are visible
                    displayFileContent(tabs[selectedTab]); // Display the selected file
                    getch(); // Wait for user input before returning
                    drawTabs(tabs, selectedTab); // Redraw the tabs
                    drawSeparator(true); // Redraw the separator
                    //displayMessage(message, tabsVisible); // Redisplay message
                }
                break;
            case 27: // Escape key
                clear();
                drawTabs(tabs, selectedTab); // Redraw the tabs
                drawSeparator(true); // Redraw the separator
                //displayMessage(message, tabsVisible); // Redisplay message
                refresh();
                break;
        }
        
        // Only redraw parts that changed if tabs are visible
        if (tabsVisible && oldSelectedTab != selectedTab) {
            drawTabs(tabs, selectedTab); // Update only tabs
            drawSeparator(true); // Redraw the separator
            //displayMessage(message, tabsVisible); // Redisplay message
            refresh(); // Refresh to show changes
        }
    }

    endwin(); // End ncurses mode
    return 0; // Return success
}