#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <dirent.h>     // For directory operations on UNIX systems

using namespace std;
extern void runLexer(const string& input, int startRow, bool selected);     // Updated function declaration

class Editor {
public:
    Editor() {
        initscr();      // Start ncurses mode
        start_color();  // Initialize color support

        init_pair(1, COLOR_RED, 234);       // For keywords like "int", "float"
        init_pair(2, COLOR_GREEN, 234);     // For strings
        init_pair(3, COLOR_YELLOW, 234);    // For comments
        init_pair(4, COLOR_BLUE, 234);      // For numbers
        init_pair(5, 186, 234);             // For other identifiers

        init_pair(6, 50, 234);              // For <>
        init_pair(7, 74, 234);              // For #include
        init_pair(8, 62, 234);              // For functions

        init_pair(101, COLOR_RED, 235);     // For keywords like "int", "float"
        init_pair(102, COLOR_GREEN, 235);   // For strings
        init_pair(103, COLOR_YELLOW, 235);  // For comments
        init_pair(104, COLOR_BLUE, 235);    // For numbers
        init_pair(105, 186, 235);           // For other identifiers

        init_pair(106, 50, 235);    // For <>
        init_pair(107, 74, 235);    // For #include
        init_pair(108, 62, 235);    // For functions


        init_pair(13, COLOR_WHITE, 234);
        init_pair(14, 106, 234);    // for line numbers other

        init_pair(15, 107, 235);    // for line numbers selected
        init_pair(16, COLOR_WHITE, 235);

        init_pair(22, COLOR_BLACK, COLOR_WHITE);


        bkgd(COLOR_PAIR(13));

        raw();                  
        keypad(stdscr, TRUE);    
        noecho();                
        curs_set(1);             
        getmaxyx(stdscr, screenHeight, screenWidth); 
        statusMessage = "";
        file_opened = "";
        inCommandMode = false;

    }

    ~Editor() {
        endwin();
    }

    /**
     * @brief This is the main loop. It waits for user input, then handles it
     */
    void run() {
        showFileMenu();     // Show file menu at the start
        while (true) {
            if (!inCommandMode) {
                displayText();
                int ch = getch();

                if (ch == ':') {
                    enterCommandMode();
                } else if (ch == KEY_UP ||ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
                    handleCursor(ch);
                }  else if (ch == KEY_BACKSPACE || ch == 127) {
                    backspace();
                } else if (ch == '\n') {
                    insertNewline();
                } else if (ch == 402 || ch == 393 || ch == 337 || ch == 336) {
                    handleShift(ch);
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
        cursorY = 0;            // Start at the top of the file
        cursorX = 0;            // Start at the beginning of the first line
        previousCursorY = 0;    // Reset previous cursor position
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

        
        clear();        // Clear the screen and display the loaded text
        displayText();  // Call displayText to show the content of the loaded file
    }

    void saveFile(const string& filename) {
        clear();
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
    int previousCursorX = 0;            // Previous cursor X position
    int previousCursorY = 0;            // Previous cursor Y position
    int topLine = 0;                    // Variable to track the top line currently displayed
    int screenWidth, screenHeight; 
    string statusMessage, file_opened; 
    bool inCommandMode;
    string commandBuffer;
    bool allText = true;

    bool selecting = false;             // Is selection mode on
    int mark_start_X = -1;
    int mark_end_X = -1;
    int mark_start_Y = -1;
    int mark_end_Y = -1;

    /**
     * @brief displays the visible text 
     */
    void displayText() {
        int visibleLines = screenHeight;  // Adjust for the status line at the bottom

        // Display line numbers for all visible lines
        
        for (int i = 0; i < visibleLines && i + topLine < int(text.size()); ++i) {
            
            if ((i + topLine) == cursorY) {
                
                attron(COLOR_PAIR(15));                     // Turn on line number color
                mvprintw(i, 0, "%4d: ", i + 1 + topLine);   // Print line number
                attroff(COLOR_PAIR(15));                    // Turn off line number color 
  
                attron(COLOR_PAIR(16));            
                for (int j = 0; j < screenWidth; j++) {
                    addch(' ');
                }
                
                runLexer(text[i + topLine].c_str(), i, true);
                attroff(COLOR_PAIR(16));
                
            } else {

                runLexer(text[i + topLine].c_str(), i, false);
                attron(COLOR_PAIR(14));
                mvprintw(i, 0, "%4d: ", i + 1 + topLine);   // Print line number
                attroff(COLOR_PAIR(14));
                
            }
            // added this for marked text
            if (selecting == true) {

                attron(COLOR_PAIR(14));                     // Turn on line number color
                mvprintw(i, 0, "%4d: ", i + 1 + topLine);   // Print line number
                attroff(COLOR_PAIR(14));                    // Turn off line number color

                if (mark_start_Y == mark_end_Y) {           // in the same line
        
                    if (mark_start_X < mark_end_X) {        // shift + right arrows
                        int k;
                        attron(COLOR_PAIR(22));
                        for (k = mark_start_X; k < mark_end_X; k++) {
                            const string& line = text[mark_end_Y];
                            mvprintw(mark_start_Y - topLine, k+6, "%c", line[k]);
                        }
                        attroff(COLOR_PAIR(22));
                    } else {                                // shift + left arrows
                        int k;
                        attron(COLOR_PAIR(22));
                        for (k = mark_end_X; k < mark_start_X; k++) {
                            const string& line = text[mark_end_Y];
                            mvprintw(mark_start_Y - topLine, k+6, "%c", line[k]);
                        }
                        attroff(COLOR_PAIR(22));
                    }
                } else {
                    int row, col;
                    attron(COLOR_PAIR(22));
                    if (mark_start_Y < mark_end_Y) {
                        for (col = mark_start_X; col < int(text[mark_start_Y].length()); col++) {   // first
                            const string& line = text[mark_start_Y];
                            mvprintw(mark_start_Y - topLine, col+6, "%c", line[col]);
                        }
                        for (col = 0; col < mark_end_X; col++) {                                    // last
                            const string& line = text[mark_end_Y];
                            mvprintw(mark_end_Y - topLine, col+6, "%c", line[col]);
                        }
                        for (row = mark_start_Y + 1; row < mark_end_Y; row++) {                     // middle
                            const string& line = text[row];
                            for (col = 0; col < int(text[row].length()); col++) {
                                mvprintw(row - topLine, col+6, "%c", line[col]);
                            }
                        }
                    } else {
                        for (col = mark_end_X; col < int(text[mark_end_Y].length()); col++) {       // last
                            const string& line = text[mark_end_Y];
                            mvprintw(mark_end_Y - topLine, col+6, "%c", line[col]);
                        }
                        for (col = 0; col < mark_start_X; col++) {                                  // first
                            const string& line = text[mark_start_Y];
                            mvprintw(mark_start_Y - topLine, col+6, "%c", line[col]);
                        }
                        for (row = mark_end_Y + 1; row < mark_start_Y; row++) {                     // middle
                            const string& line = text[row];
                            for (col = 0; col < int(text[row].length()); col++) {
                                mvprintw(row - topLine, col+6, "%c", line[col]);
                            }
                        }
                    }
                    attroff(COLOR_PAIR(22));
                }
            }
        }

        int visibleCursorY = cursorY - topLine;
        move(visibleCursorY, cursorX + 6);      // Adjust cursor position to account for line numbers
        refresh();                              // Refresh the screen to update the display
    }

    /**
     * @brief the following 2 functions handle scrolling when called
     */
    void scrollDown() {
        
        // Only scroll if the cursor is at the bottom of the screen
        if (cursorY >= topLine + screenHeight - 2 && topLine + screenHeight != (int)text.size()) {
            topLine++;  // Scroll the view down

            // Clear the top line and shift the rest of the lines up by one
            move(0, 1);     // Adjust for line numbers
            insdelln(-1);   // Delete the top line (scrolling up the rest)
            
            refresh();
        }
    }

    void scrollUp() {
        if (cursorY < topLine + 2 && topLine != 0) {
            topLine--;      // Scroll the view up

            // Move all lines down by 1 on the screen to simulate scrolling up
            move(0, 1);     // Move cursor to the top of the screen
            insdelln(1);    // Insert a blank line at the top (pushing lines down)

            refresh();      // Refresh the screen to reflect changes
        }
    }

    /**
     * @brief the following 4 functions handle cursor movement
     */
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

    void shiftUp(){
        if (mark_start_Y == -1) {
            mark_start_Y = cursorY;
            mark_start_X = cursorX;
        }
        moveCursorUp();
        mark_end_Y = cursorY;
        mark_end_X = cursorX;
    }

    void shiftDown(){
        if (mark_start_Y == -1) {
            mark_start_Y = cursorY;
            mark_start_X = cursorX;
        }
        moveCursorDown();
        mark_end_Y = cursorY;
        mark_end_X = cursorX;
    }

    void shiftLeft(){
        if (mark_start_X == -1) {
            mark_start_X = cursorX;
            mark_start_Y = cursorY;
        }
        moveCursorLeft();
        mark_end_X = cursorX;
        mark_end_Y = cursorY;
    }

    void shiftRight(){
        if (mark_start_X == -1) {
            mark_start_X = cursorX;
            mark_start_Y = cursorY;
        }
        moveCursorRight();
        mark_end_X = cursorX;
        mark_end_Y = cursorY;
    }

    void handleShift(int ch) {
        selecting = true;
        if (ch == 402) {            // Shift + Right Arrow
            shiftRight();
        } else if (ch == 393) {     // Shift + Left Arrow
            shiftLeft();
        } else if (ch == 337) {     // Shift + Up Arrow
            shiftUp();
        } else if (ch == 336) {     // Shift + Down Arrow
            shiftDown();
        }
    }

    /**
     * @brief handles cursor movement and resets marked text
     * 
     * @note resets marked text when moving cursor without pressing shift
     */
    void handleCursor(int ch) {
        selecting = false;
        mark_start_X = -1;
        mark_end_X = -1;
        mark_start_Y = -1;
        mark_end_Y = -1;
        if (ch == KEY_UP) {
            moveCursorUp();
        } else if (ch == KEY_DOWN) {
            moveCursorDown();
        } else if (ch == KEY_LEFT) {
            moveCursorLeft();
        } else if (ch == KEY_RIGHT) {
            moveCursorRight();
        }
    }

    void insertChar(int ch) {
        if (selecting == true) {
            backspace();
            selecting = false;
        }
        if ((char)ch == '\t') {
            text[cursorY].insert(cursorX, 4, ' ');
            cursorX = cursorX + 4;
        } else {
            text[cursorY].insert(cursorX, 1, (char)ch);
            cursorX++;
        }
    }
    /**
     * @brief Basic functionality: ether it is just a backspace, ether it deletes marked text.
     * 
     * @note No bugs.
     */
    void backspace() {

        if (mark_start_Y != -1 && mark_end_Y != -1 && mark_start_X != -1 && mark_end_X != -1) {
            
            if (mark_start_Y == mark_end_Y) {       // in the same line

                if (mark_start_X < mark_end_X) {    // shift + right arrows
                    text[cursorY].erase(mark_start_X, mark_end_X - mark_start_X);
                    cursorX = mark_start_X;
                } else {                            // shift + left arrows
                    text[cursorY].erase(mark_end_X, mark_start_X - mark_end_X);
                }
            } else {                                // multiline

                if (mark_start_Y > mark_end_Y) {
                    int temp = mark_start_Y;
                    mark_start_Y = mark_end_Y;
                    mark_end_Y = temp;

                    temp = mark_start_X;
                    mark_start_X = mark_end_X;
                    mark_end_X = temp;
                }
                text[mark_start_Y].erase(mark_start_X, text[mark_start_Y].length());    // first and last lines
                text[mark_end_Y].erase(0, mark_end_X);
                text[mark_start_Y] += text[mark_end_Y];

                for (int i = 0; i < mark_end_Y - mark_start_Y; i++) {
                    text.erase(text.begin() + mark_end_Y - i);
                }
                cursorX = mark_start_X;
                cursorY = mark_start_Y;
            }
            mark_start_X = -1;
            mark_end_X = -1;
            mark_start_Y = -1;
            mark_end_Y = -1;
            
            clear();
            return;
        }

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
        move(cursorY - topLine, 1);     // Move to the correct line (offset by 5 for line numbers)
        clrtoeol();                     // Clear from cursor to the end of the line
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
        move(cursorY - topLine, 1);     // Move to the correct line (offset by 5 for line numbers)
        clrtoeol();                     // Clear from cursor to the end of the line
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
            saveFile(file_opened);
        } else if (commandBuffer == ":wq") {
            saveFile(file_opened);
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
            file_opened = files[choice - '1'];
            loadFile(file_opened);  // Load selected file
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
        saveFile(newFileName);      // Save the new file
        loadFile(newFileName);      // Load the new file into the editor
    }
};

int main() {
    Editor editor;
    editor.run();
    return 0;
}