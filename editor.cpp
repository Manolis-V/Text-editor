#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>   // For file input/output
#include <chrono>
#include <thread>

using namespace std;

class Editor {
public:
    Editor() {
        // Initialize ncurses
        initscr();
        raw();                  // Disable line buffering
        keypad(stdscr, TRUE);    // Enable special keys like arrows
        noecho();                // Don't echo typed characters
        curs_set(1);             // Show cursor
        getmaxyx(stdscr, screenHeight, screenWidth); // Get screen dimensions
        statusMessage = "";
    }

    ~Editor() {
        endwin(); // Restore normal terminal behavior
    }

    void run() {
        while (true) {
            displayText();
            int ch = getch();

            if (ch == 'q') {
                break; // Quit if 'q' is pressed
            } else if (ch == 'w') {
                saveFile("output.txt");  // Save file if 'w' is pressed
                showStatusMessage("File saved as output.txt");
            } else if (ch == KEY_UP) {
                moveCursorUp();
            } else if (ch == KEY_DOWN) {
                moveCursorDown();
            } else if (ch == KEY_LEFT) {
                moveCursorLeft();
            } else if (ch == KEY_RIGHT) {
                moveCursorRight();
            } else if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
                backspace();
            } else if (ch == '\n') {  // Handle newline
                insertNewline();
            } else {
                insertChar(ch);
            }
        }
    }

    void loadFile(const string& filename) {
        ifstream file(filename);  // Input file stream
        if (!file) return;  // Return if the file cannot be opened
        text.clear();  // Clear existing text
        string line;
        while (getline(file, line)) {
            text.push_back(line);  // Load lines into the text buffer
        }
        file.close();
    }

    void saveFile(const string& filename) {
        ofstream file(filename);  // Output file stream
        if (!file) {
            showStatusMessage("Error saving file.");
            return;
        }
        for (const auto& line : text) {
            file << line << '\n';  // Write each line to the file
        }
        file.close();
    }

private:
    vector<string> text = {""}; // Lines of text
    int cursorX = 0; // Horizontal cursor position
    int cursorY = 0; // Vertical cursor position
    int screenWidth, screenHeight; // Screen dimensions
    string statusMessage; // Status message to show

    void displayText() {
        clear();
        // Display text
        for (int i = 0; i < text.size(); ++i) {
            mvprintw(i, 0, text[i].c_str()); // Display each line
        }
        // Display status message at the bottom of the screen
        mvprintw(screenHeight - 1, 0, statusMessage.c_str());
        move(cursorY, cursorX); // Move cursor to current position
        refresh();
    }

    void moveCursorUp() {
        if (cursorY > 0) {
            cursorY--;
            cursorX = min(cursorX, (int)text[cursorY].length());
        }
    }

    void moveCursorDown() {
        if (cursorY < text.size() - 1) {
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
        if (cursorX < text[cursorY].length()) {
            cursorX++;
        } else if (cursorY < text.size() - 1) {
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
        this_thread::sleep_for(chrono::seconds(2)); // Display the message for 2 seconds
        statusMessage = ""; // Clear the status message
    }
};

int main() {
    Editor editor;
    editor.loadFile("output.txt");  // Load text from a file (optional)
    editor.run();
    return 0;
}
