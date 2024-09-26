#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <dirent.h> // For directory operations on UNIX systems

using namespace std;

class Editor {
public:
    Editor() {
        initscr();
        start_color(); // Initialize color support
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Line number color
        init_pair(2, COLOR_GREEN, COLOR_BLACK); // Status message color
        raw();                  
        keypad(stdscr, TRUE);    
        noecho();                
        curs_set(1);             
        getmaxyx(stdscr, screenHeight, screenWidth); 
        statusMessage = "";
        inCommandMode = false;

        // Initialize cursor position variables
        previousCursorY = 0;
        previousCursorX = 0;
    }

    ~Editor() {
        endwin();
    }

    void run() {
        showFileMenu(); // Show file menu at the start

        while (true) {
            if (!inCommandMode) {
                displayText(); // Initial display call
                int ch = getch();

                if (ch == ':') {
                    enterCommandMode();
                } else if (ch == KEY_UP) {
                    moveCursorUp();
                } else if (ch == KEY_DOWN) {
                    moveCursorDown();
                } else if (ch == KEY_LEFT) {
                    moveCursorLeft();
                } else if (ch == KEY_RIGHT) {
                    moveCursorRight();
                } else if (ch == KEY_BACKSPACE || ch == 127) {
                    backspace();
                } else if (ch == '\n') {
                    insertNewline();
                } else {
                    insertChar(ch);
                }
            } else {
                handleCommandMode();
            }
        }
    }

    void loadFile(const string& filename) {
        ifstream file(filename);
        if (!file) return; // If the file can't be opened, do nothing
        
        text.clear(); // Clear previous text
        string line;
        while (getline(file, line)) {
            text.push_back(line); // Load lines from the file into the text vector
        }
        file.close();

        // Reset cursor position and previous cursor tracking
        cursorY = 0; // Start at the top of the file
        cursorX = 0; // Start at the beginning of the first line
        previousCursorY = 0; // Reset previous cursor position
        previousCursorX = 0;

        // Clear the screen and display the loaded text
        clear();
        displayAllText(); // Call displayText to show the content of the loaded file
    }

    void saveFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            showStatusMessage("Error saving file.");
            return;
        }
        for (const auto& line : text) {
            file << line << '\n';
        }
        file.close();
        showStatusMessage("File saved as " + filename);
    }

private:
    vector<string> text = {""}; 
    int cursorX = 0; 
    int cursorY = 0; 
    int previousCursorX = 0; // Previous cursor X position
    int previousCursorY = 0; // Previous cursor Y position
    int topLine = 0;  // Variable to track the top line currently displayed
    int screenWidth, screenHeight; 
    string statusMessage; 
    bool inCommandMode;
    string commandBuffer;

    void displayAllText() {
        clear(); // Clear the screen to avoid previous content being shown

        // Display all lines of text
        for (int i = 0; i < int(text.size()); ++i) {
            mvprintw(i, 4, "%s", text[i].c_str()); // Print each line
        }

        // Display line numbers for all lines
        for (int i = 0; i < int(text.size()); ++i) {
            attron(COLOR_PAIR(1)); // Turn on line number color
            mvprintw(i, 0, "%2d: ", i + 1); // Print line number
            attroff(COLOR_PAIR(1)); // Turn off line number color
        }

        // Move cursor to the correct position
        move(cursorY, cursorX + 4);  // Adjust cursor position to account for line numbers
        refresh(); // Refresh the screen to update the display
    }
    void displayText() {
        int visibleLines = screenHeight - 2;  // Adjust for the status line at the bottom

        // Display only the lines that fit within the screen, starting from `topLine`
        for (int i = 0; i < visibleLines && i + topLine < int(text.size()); ++i) {
            mvprintw(i, 5, "%s", text[i + topLine].c_str()); // Print each line
        }

        // Display line numbers for all visible lines
        for (int i = 0; i < visibleLines && i + topLine < int(text.size()); ++i) {
            attron(COLOR_PAIR(1)); // Turn on line number color
            mvprintw(i, 0, "%3d: ", i + 1 + topLine); // Print line number
            attroff(COLOR_PAIR(1)); // Turn off line number color
        }

        // Move cursor to the correct position, adjusting for scrolling
        int visibleCursorY = cursorY - topLine;
        move(visibleCursorY, cursorX + 5);  // Adjust cursor position to account for line numbers
        refresh(); // Refresh the screen to update the display
    }

    // void displayText() {
    //     // Refresh the current line and the line where the cursor was previously
    //     if (cursorY != previousCursorY) {
    //         // Clear the previous line
    //         mvprintw(previousCursorY, 4, "%s", text[previousCursorY].c_str()); 
    //     }
        
    //     // Display the current line
    //     mvprintw(cursorY, 4, "%s", text[cursorY].c_str());

    //     // Display line numbers for all lines
    //     for (int i = 0; i < int(text.size()); ++i) {
    //         attron(COLOR_PAIR(1)); // Turn on line number color
    //         mvprintw(i, 0, "%2d: ",  i + 1 + topLine); // Print line number
    //         attroff(COLOR_PAIR(1)); // Turn off line number color
    //     }

    //     attron(COLOR_PAIR(2)); // Turn on status message color
    //     mvprintw(screenHeight - 1, 0, statusMessage.c_str());
    //     attroff(COLOR_PAIR(2)); // Turn off status message color
        
    //     // Move cursor to the correct position
    //     move(cursorY, cursorX + 4);  // Adjust cursor position to account for line numbers
    //     refresh();

    //     // Update the previous cursor position
    //     previousCursorX = cursorX;
    //     previousCursorY = cursorY;
    // }

    void scrollDown() {
        int visibleLines = screenHeight - 2; // Number of visible lines on the screen

        // Only scroll if the cursor is at the bottom of the screen
        if (cursorY >= topLine + visibleLines - 1) {
            topLine++;  // Scroll the view down

            // Clear the top line and shift the rest of the lines up by one
            move(0, 0);
            insdelln(-1);  // Delete the top line (scrolling up the rest)

            // Render the new bottom line
            int newLineIndex = topLine + visibleLines - 1;  // Index of the new bottom line
            if (newLineIndex < int(text.size())) {
                mvprintw(visibleLines - 1, 4, "%s", text[newLineIndex].c_str()); // Print the new line
                attron(COLOR_PAIR(1));  // Line number color
                mvprintw(visibleLines - 1, 0, "%2d: ", newLineIndex + 1); // Print line number
                attroff(COLOR_PAIR(1));  // Turn off line number color
            }
            refresh();
        }
    }

    void scrollUp() {
        if (cursorY < topLine) {
            topLine--;  // Scroll the view up

            // Move all lines down by 1 on the screen to simulate scrolling up
            move(0, 0);  // Move cursor to the top of the screen
            insdelln(1);  // Insert a blank line at the top (pushing lines down)

            // Render the new top line
            mvprintw(0, 4, "%s", text[topLine].c_str());  // Print the new top line
            attron(COLOR_PAIR(1));  // Line number color
            mvprintw(0, 0, "%3d: ", topLine + 1);  // Print the line number for the new top line
            attroff(COLOR_PAIR(1));  // Turn off line number color

            refresh();  // Refresh the screen to reflect changes
        }
    }


    void moveCursorDown() {
        if (cursorY < int(text.size()) - 1) {
            cursorY++;
            scrollDown();  // Scroll down if needed
            cursorX = min(cursorX, (int)text[cursorY].length());

            // Move the cursor to the correct position on screen
            int visibleCursorY = cursorY - topLine;
            move(visibleCursorY, cursorX + 4);  // Adjust for line numbers
            refresh();
        }
    }

    void moveCursorUp() {
        if (cursorY > 0) {
            cursorY--;
            scrollUp();  // Scroll up if needed
            cursorX = min(cursorX, (int)text[cursorY].length());

            // Move the cursor to the correct position on screen
            int visibleCursorY = cursorY - topLine;
            move(visibleCursorY, cursorX + 4);  // Adjust for line numbers
            refresh();
        }
    }

    void moveCursorLeft() {
        if (cursorX > 0) {
            cursorX--;
        } else if (cursorY > 0) {
            cursorY--;
            cursorX = text[cursorY].length();
        }
    }

    void moveCursorRight() {
        if (cursorX < int(text[cursorY].length())) {
            cursorX++;
        } else if (cursorY < int(text.size()) - 1) {
            cursorY++;
            cursorX = 0;
        }
    }

    void insertChar(int ch) {
        text[cursorY].insert(cursorX, 1, (char)ch);
        cursorX++;
    }

    void backspace() {
        if (cursorX > 0) {
            // Remove the character at the cursor position
            text[cursorY].erase(cursorX - 1, 1);
            cursorX--;
        } else if (cursorY > 0) {
            // If at the start of the line, merge with the previous line
            cursorX = text[cursorY - 1].length();
            text[cursorY - 1] += text[cursorY];  // Merge the lines
            text.erase(text.begin() + cursorY);  // Remove the current line
            cursorY--;  // Move the cursor up

            // After merging lines, clear the line where the original line was
            move(cursorY + 1, 4); // Move to the line that was merged and is now removed (adjust for line numbers)
            clrtoeol();  // Clear the line to remove any leftover characters
        }

        // Clear the rest of the current line to handle any leftover characters
        move(cursorY, 4); // Move to the correct line (offset by 5 for line numbers)
        clrtoeol();  // Clear from cursor to the end of the line

        // Refresh the entire screen to reflect the changes
        displayText();
    }

    void insertNewline() {
        string newLine = text[cursorY].substr(cursorX);
        text[cursorY] = text[cursorY].substr(0, cursorX);
        text.insert(text.begin() + cursorY + 1, newLine);
        cursorY++;
        cursorX = 0;
    }

    void showStatusMessage(const string& message) {
        statusMessage = message;
        displayText(); // Call displayText to show the status message
        refresh();
        this_thread::sleep_for(chrono::milliseconds(500)); 
        statusMessage = "";
    }

    void enterCommandMode() {
        inCommandMode = true;
        commandBuffer = ":";  
        move(screenHeight - 1, 0);  
        clrtoeol();  
        printw(":");
        refresh();
    }

    void handleCommandMode() {
        int ch = getch();

        if (ch == '\n') {
            executeCommand();
            inCommandMode = false;
            commandBuffer.clear();
            statusMessage = "";  
        } else if (ch == 27) {  
            inCommandMode = false;
            statusMessage = "";
            displayText(); // Return to normal mode
        } else if (ch == KEY_BACKSPACE || ch == 127) {  
            if (!commandBuffer.empty()) {
                commandBuffer.pop_back();
                move(screenHeight - 1, 0);
                clrtoeol();
                printw("%s", commandBuffer.c_str());
                refresh();
            }
        } else {
            commandBuffer.push_back(ch);
            move(screenHeight - 1, 0);
            clrtoeol();
            printw("%s", commandBuffer.c_str());
            refresh();
        }
    }

    void executeCommand() {
        if (commandBuffer == ":q") {
            endwin();
            exit(0);
        } else if (commandBuffer == ":w") {
            saveFile("output.txt");
        } else if (commandBuffer == ":wq") {
            saveFile("output.txt");
            endwin();
            exit(0);
        } else if (commandBuffer == ":m") {
            showFileMenu();
        } else {
            showStatusMessage("Unknown command");
        }
    }

    void showFileMenu() {
        clear();
        vector<string> files;

        // Read all files in the current directory using dirent.h
        DIR* dir;
        struct dirent* entry;

        if ((dir = opendir(".")) != nullptr) {
            while ((entry = readdir(dir)) != nullptr) {
                if (entry->d_type == DT_REG) {  // Only regular files
                    files.push_back(entry->d_name);
                }
            }
            closedir(dir);
        } else {
            showStatusMessage("Failed to open directory.");
            return;
        }

        // Display file menu
        mvprintw(0, 0, "Select a file to edit:");
        for (size_t i = 0; i < files.size(); ++i) {
            mvprintw(i + 1, 0, "%zu. %s", i + 1, files[i].c_str());
        }
        mvprintw(files.size() + 2, 0, "Press '0' to create a new file.");
        mvprintw(files.size() + 3, 0, "Press 'q' to exit.");
        refresh();

        int choice = getch();
        if (choice == 'q') {
            endwin();
            exit(0);
        } else if (choice == '0') {
            createNewFile();
        } else if (choice >= '1' && choice <= '9' && (size_t)(choice - '1') < files.size()) {
            loadFile(files[choice - '1']);  // Load selected file
        }
    }

    void createNewFile() {
        clear();
        mvprintw(0, 0, "Enter new file name: ");
        refresh();
        string newFileName;
        char ch;
        while ((ch = getch()) != '\n') {  // Get new file name
            newFileName.push_back(ch);
            printw("%c", ch);
            refresh();
        }
        saveFile(newFileName);  // Save the new file
        loadFile(newFileName);   // Load the new file into the editor
    }
};

int main() {
    Editor editor;
    editor.run();
    return 0;
}
