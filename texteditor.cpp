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
        initscr();  // Start ncurses mode
        start_color(); // Initialize color support
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Line number color
        init_pair(2, COLOR_GREEN, COLOR_BLACK); // Status message color

        init_pair(3, COLOR_WHITE, 234);
        init_pair(4, 106, 234);

        init_pair(5, 107, 235);
        init_pair(6, COLOR_WHITE, 235);
        bkgd(COLOR_PAIR(3));

        raw();                  
        keypad(stdscr, TRUE);    
        noecho();                
        curs_set(1);             
        getmaxyx(stdscr, screenHeight, screenWidth); 
        statusMessage = "";
        inCommandMode = false;

        // WINDOW* win = newwin(height, width, start_y, start_x);
        // wbkgd(win, COLOR_PAIR(1)); // Set background for this window
        // wrefresh(win);             // Refresh the window
    }

    ~Editor() {
        endwin();
    }

    void run() {
        showFileMenu(); // Show file menu at the start
        while (true) {
            if (!inCommandMode) {
                displayText();
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

        // Reset cursor position and previous cursor tracking
        cursorY = 0; // Start at the top of the file
        cursorX = 0; // Start at the beginning of the first line
        previousCursorY = 0; // Reset previous cursor position
        previousCursorX = 0;
        topLine = 0;

        ifstream file(filename);
        if (!file) return;
        text.clear();  
        string line;
        while (getline(file, line)) {
            text.push_back(line);
        }
        file.close();

        // Clear the screen and display the loaded text
        clear();
        displayText(); // Call displayText to show the content of the loaded file
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
    bool allText = true;

    void displayText() {
        int visibleLines = screenHeight;  // Adjust for the status line at the bottom

        // Display line numbers for all visible lines
        
        for (int i = 0; i < visibleLines && i + topLine < int(text.size()); ++i) {
            
            if ((i + topLine) == cursorY) {
                
                attron(COLOR_PAIR(5)); // Turn on line number color
                mvprintw(i, 0, "%4d: ", i + 1 + topLine); // Print line number
                attroff(COLOR_PAIR(5)); // Turn off line number color 
  
                attron(COLOR_PAIR(6)); // Turn on line number color
                for (int j = 0; j < screenWidth; j++) {
                    addch(' ');
                }
                mvprintw(i, 6, "%s", text[i + topLine].c_str()); // Print each line
                attroff(COLOR_PAIR(6)); // Turn off line number color
                           
            } else {
                //clrtoeol();

                mvprintw(i, 6, "%s", text[i + topLine].c_str()); // Print each line
                attron(COLOR_PAIR(4)); // Turn on line number color
                mvprintw(i, 0, "%4d: ", i + 1 + topLine); // Print line number
                attroff(COLOR_PAIR(4)); // Turn off line number color
            
                
            }
        }

        // Move cursor to the correct position, adjusting for scrolling
        // int visibleCursorY = previousCursorY - topLine;
        // move(visibleCursorY, cursorX + 6);
        // clrtoeol();
        // refresh();
        int visibleCursorY = cursorY - topLine;
        move(visibleCursorY, cursorX + 6);  // Adjust cursor position to account for line numbers
        refresh(); // Refresh the screen to update the display
    }

    void scrollDown() {
        
        // Only scroll if the cursor is at the bottom of the screen
        if (cursorY >= topLine + screenHeight - 2 && topLine + screenHeight != (int)text.size()) {
            topLine++;  // Scroll the view down

            // Clear the top line and shift the rest of the lines up by one
            move(0, 1);  // Adjust for line numbers
            insdelln(-1);  // Delete the top line (scrolling up the rest)
            
            // Render the new bottom line
            int newLineIndex = topLine + screenHeight - 1;  // Index of the new bottom line

            mvprintw(screenHeight - 1, 4, "%s", text[newLineIndex].c_str()); // Print the new line
            attron(COLOR_PAIR(4));  // Line number color
            mvprintw(screenHeight - 1, 0, "%3d: ", newLineIndex); // Print line number
            attroff(COLOR_PAIR(4));  // Turn off line number color
            
            refresh();
        }
    }

    void scrollUp() {
        if (cursorY < topLine + 2 && topLine != 0) {
            topLine--;  // Scroll the view up

            // Move all lines down by 1 on the screen to simulate scrolling up
            move(0, 1);  // Move cursor to the top of the screen
            insdelln(1);  // Insert a blank line at the top (pushing lines down)

            // Render the new top line
            mvprintw(0, 4, "%s", text[topLine].c_str());  // Print the new top line
            attron(COLOR_PAIR(4));  // Line number color
            mvprintw(0, 0, "%3d: ", topLine + 1);  // Print the line number for the new top line
            attroff(COLOR_PAIR(4));  // Turn off line number color
    
            refresh();  // Refresh the screen to reflect changes
        }
    }

    void moveCursorUp() {
        if (cursorY > 0) {
            cursorY--;
            clrtoeol();
            scrollUp();  // Scroll up if needed
            cursorX = min(cursorX, (int)text[cursorY].length());

            // Move the cursor to the correct position on screen
            int visibleCursorY = cursorY - topLine;
            move(visibleCursorY, cursorX + 6);  // Adjust for line numbers
            refresh();
        }
    }

    void moveCursorDown() {
        if (cursorY < int(text.size()) - 1) {
            cursorY++;
            clrtoeol();
            scrollDown();  // Scroll down if needed
            cursorX = min(cursorX, (int)text[cursorY].length());

            // Move the cursor to the correct position on screen
            int visibleCursorY = cursorY - topLine;
            move(visibleCursorY, cursorX + 6);  // Adjust for line numbers
            refresh();
        }
    }
    // okey
    void moveCursorLeft() {
        clrtoeol();
        if (cursorX > 0) {
            cursorX--;
        } else if (cursorY < topLine + 2 && topLine != 0) {
            moveCursorUp();
        }  else if (cursorY > 0) {
            cursorY--;
            cursorX = text[cursorY].length();
        }
    }
    // okey
    void moveCursorRight() {
        clrtoeol();
        if (cursorX < int(text[cursorY].length())) {
            cursorX++;
        } else if (cursorY >= topLine + screenHeight - 2 && topLine + screenHeight != (int)text.size()) {
            moveCursorDown();
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
            // When in line changes
            text[cursorY].erase(cursorX - 1, 1);
            cursorX--;
        } else if (cursorY > 0) {
            // When changing lines
            cursorX = text[cursorY - 1].length();
            text[cursorY - 1] += text[cursorY];
            text.erase(text.begin() + cursorY);
            cursorY--;
            for (int i=0; i<int(text.size()); i++) {
                move(cursorY + i, 6);
                clrtoeol();
            }
            insdelln(-1);
            refresh();
        } else {
            // When in the initial possition
            return;
        }
        // Clear the rest of the current line to handle any leftover characters
        move(cursorY - topLine, 1); // Move to the correct line (offset by 5 for line numbers)
        clrtoeol();  // Clear from cursor to the end of the line
    }

    // for one \n it copies the text above
    void insertNewline() {
        string newLine = text[cursorY].substr(cursorX);
        text[cursorY] = text[cursorY].substr(0, cursorX);
        text.insert(text.begin() + cursorY + 1, newLine);
        cursorX = 0;
        for (int i=0; i<int(text.size()); i++) {
            move(cursorY + i, 6);
            clrtoeol();
        }
        cursorY++;
        move(cursorY - topLine, 1); // Move to the correct line (offset by 5 for line numbers)
        clrtoeol();  // Clear from cursor to the end of the line
        insdelln(1);
        refresh(); 
    }

    void showStatusMessage(const string& message) {
        statusMessage = message;
        displayText();
        refresh();
        this_thread::sleep_for(chrono::seconds(1)); 
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
            displayText();
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
