#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;

class Editor {
public:
    Editor() {
        initscr();
        raw();                  
        keypad(stdscr, TRUE);    
        noecho();                
        curs_set(1);             
        getmaxyx(stdscr, screenHeight, screenWidth); 
        statusMessage = "";
        inCommandMode = false;
    }

    ~Editor() {
        endwin();
    }

    void run() {
        while (true) {
            if (!inCommandMode) {
                displayText();
                int ch = getch();

                if (ch == ':') {
                    enterCommandMode();  // Enter command mode if ':' is pressed
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
                handleCommandMode();  // Handle command input
            }
        }
    }

    void loadFile(const string& filename) {
        ifstream file(filename);  
        if (!file) return;
        text.clear();  
        string line;
        while (getline(file, line)) {
            text.push_back(line);  
        }
        file.close();
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
    int screenWidth, screenHeight; 
    string statusMessage; 
    bool inCommandMode;
    string commandBuffer;

    void displayText() {
        clear();
        for (int i = 0; i < int(text.size()); ++i) {
            mvprintw(i, 0, text[i].c_str()); 
        }
        mvprintw(screenHeight - 1, 0, statusMessage.c_str()); 
        move(cursorY, cursorX); 
        refresh();
    }

    void moveCursorUp() {
        if (cursorY > 0) {
            cursorY--;
            cursorX = min(cursorX, (int)text[cursorY].length());
        }
    }

    void moveCursorDown() {
        if (cursorY < int(text.size()) - 1) {
            cursorY++;
            cursorX = min(cursorX, (int)text[cursorY].length());
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
            text[cursorY].erase(cursorX - 1, 1);
            cursorX--;
        } else if (cursorY > 0) {
            cursorX = text[cursorY - 1].length();
            text[cursorY - 1] += text[cursorY];
            text.erase(text.begin() + cursorY);
            cursorY--;
        }
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
        displayText();
        refresh();
        this_thread::sleep_for(chrono::seconds(2)); 
        statusMessage = ""; 
    }

    void enterCommandMode() {
        inCommandMode = true;
        commandBuffer = ":";  // Start with ':'
        move(screenHeight - 1, 0);  // Move cursor to the bottom of the screen
        clrtoeol();  // Clear the command line area
        printw(":");
        refresh();
    }

    void handleCommandMode() {
        int ch = getch();

        if (ch == '\n') {  // Handle enter key
            executeCommand();
            inCommandMode = false;
            commandBuffer.clear();
            statusMessage = "";  // Clear the status line
        } else if (ch == 27) {  // Escape key to cancel command mode
            inCommandMode = false;
            statusMessage = "";
            displayText();
        } else if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace in command mode
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
            exit(0);  // Quit the editor
        } else if (commandBuffer == ":w") {
            saveFile("output.txt");  // Save the file
        } else {
            showStatusMessage("Unknown command");
        }
    }
};

int main() {
    Editor editor;
    editor.loadFile("output.txt");  // Load text from a file (optional)
    editor.run();
    return 0;
}