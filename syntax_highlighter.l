%{
#include <ncurses.h>
#include <sstream>  // Include the required header for std::istringstream
#include <cstring>  // For strlen
#include <string>   // For std::string

extern int yylex();
void apply_color(int color);  // Function to apply colors
void print_token(const char* text);
int row = 0, col = 0;  // Track cursor position
%}

%option noyywrap
%option c++
%option input

%%

"int"|"float"|"if"|"else"|"return" {
    apply_color(1);  // Apply keyword color (e.g., red)
    print_token(yytext);
    apply_color(0);  // Reset color
}

\"([^"\\]|\\.)*\" {
    apply_color(2);  // Apply string color (e.g., green)
    print_token(yytext);
    apply_color(0);
}

"//".* {
    apply_color(3);  // Apply comment color (e.g., yellow)
    print_token(yytext);
    apply_color(0);
}

"/*"([^*]|\*+[^*/])*\*+"/" {
    apply_color(3);  // Apply comment color (e.g., yellow)
    print_token(yytext);
    apply_color(0);
}

[-+]?[0-9]+(\.[0-9]+)? {
    apply_color(4);  // Apply number color (e.g., blue)
    print_token(yytext);
    apply_color(0);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    apply_color(5);  // Apply identifier color (e.g., magenta)
    print_token(yytext);
    apply_color(0);
}

. { 
    print_token(yytext);  // For any single character, print it without color
}

\n {
    row++;
    col = 0;
}

%%

void apply_color(int color) {
    switch (color) {
        case 1:
            attron(COLOR_PAIR(1));
            break;
        case 2:
            attron(COLOR_PAIR(2));
            break;
        case 3:
            attron(COLOR_PAIR(3));
            break;
        case 4:
            attron(COLOR_PAIR(4));
            break;
        case 5:
            attron(COLOR_PAIR(5));
            break;
        default:
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(5));
            break;
    }
}

// Helper function to print tokens and update cursor position
void print_token(const char* text) {
    mvprintw(row, col, "%s", text);  // Print at current row and column
    col += strlen(text);             // Move the cursor forward
}

void runLexer(const std::string& input) {
    std::istringstream iss(input);   // Ensure the <sstream> header is included
    yyFlexLexer lexer(&iss);         // Use the input stream with the lexer
    row = 0;
    col = 0;  // Reset cursor position
    lexer.yylex();  // Tokenize and colorize input
}