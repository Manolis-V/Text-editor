%{
#include <ncurses.h>
#include <sstream>  // Include the required header for std::istringstream
#include <cstring>  // For strlen
#include <string>   // For std::string

extern int yylex();
void apply_color(int color);  // Function to apply colors
void print_token(const char* text);

int row = 0, col = 0;  // Track cursor position
bool selected_line = false;
%}

%option noyywrap
%option c++
%option input

%%

"int"|"float"|"else"|"if"|"for"|"while"|"return"|"string"|"void"|"else if" {
    apply_color(1);  // Apply keyword color (e.g., red)
    print_token(yytext);
    apply_color(0);  // Reset color
}

\"([^"\\]|\\.)*\" {
    apply_color(2);  // Apply string color (e.g., green)
    print_token(yytext);
    apply_color(0);
}

\'([^"\\]|\\.)*\' {
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

"<"[a-zA-Z0-9._]+">" {
    apply_color(6);  // Apply string color (e.g., green)
    print_token(yytext);
    apply_color(0);
}

"#include" {
    apply_color(7);  // Apply string color (e.g., green)
    print_token(yytext);
    apply_color(0);
}

[a-zA-Z_][a-zA-Z0-9_]*[ \t]*"(" {
    apply_color(8);  // Apply string color (e.g., green)
    yytext[strlen(yytext)-1] = '\0';  // Remove the "(" from yytext
    print_token(yytext);
    apply_color(0);
    unput('(');  // Put back the '(' for separate handling
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
            if(selected_line == true){
                color = 100 + color;
                for (int j = 0; j < 50; j++) {
                    addch(' ');
                }
            }
            attron(COLOR_PAIR(color));
            break;
        case 2:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 3:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 4:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 5:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 6:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 7:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        case 8:
            if(selected_line == true){
                color = 100 + color; 
            }
            attron(COLOR_PAIR(color));
            break;
        default:
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(5));
            attroff(COLOR_PAIR(6));
            attroff(COLOR_PAIR(7));
            attroff(COLOR_PAIR(8));
            attroff(COLOR_PAIR(101));
            attroff(COLOR_PAIR(102));
            attroff(COLOR_PAIR(103));
            attroff(COLOR_PAIR(104));
            attroff(COLOR_PAIR(105));
            attroff(COLOR_PAIR(106));
            attroff(COLOR_PAIR(107));
            attroff(COLOR_PAIR(108));

            if(selected_line == true){
                attron(COLOR_PAIR(16));
            }

            break;
    }
}

// Helper function to print tokens and update cursor position
void print_token(const char* text) {
    
    mvprintw(row, col, "%s", text);  // Print at current row and column
    col += strlen(text);             // Move the cursor forward
}

// Updated runLexer function to accept row argument
void runLexer(const std::string& input, int startRow, bool selected) {
    std::istringstream iss(input);   // Use string as input stream for lexer
    yyFlexLexer lexer(&iss);
    selected_line = selected;
    row = startRow;  // Initialize row with the passed argument
    col = 6;         // Reset column position
    lexer.yylex();   // Tokenize and colorize input
}
