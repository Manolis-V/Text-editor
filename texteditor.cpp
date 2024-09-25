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
        // Display the line numbers
        for (int i = 0; i < int(text.size()); ++i) {
            mvprintw(i, 0, "%2d: %s", i + 1, text[i].c_str());  // Print line number
        }
        mvprintw(screenHeight - 1, 0, statusMessage.c_str());
        move(cursorY, cursorX + 4);  // Adjust cursor position to account for line numbers
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
