%{
#include <ncurses.h>
//#include <FlexLexer.h>
#include <iostream>
#include <sstream>

extern int yylex();
void runLexer(const std::string& input);
void apply_color(int color);

int row = 0, col = 0;
%}

%option noyywrap
%option c++

%%

"int"|"float"|"if"|"else"|"return" {
    apply_color(1);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

\"([^"\\]|\\.)*\" {
    apply_color(2);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

"//".* {
    apply_color(3);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

"/*"([^*]|\*+[^*/])*\*+"/" {
    apply_color(3);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

[-+]?[0-9]+(\.[0-9]+)? {
    apply_color(4);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    apply_color(5);
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
    apply_color(0);
}

. { 
    mvprintw(row, col, "%s", yytext);
    col = col + strlen(yytext);
    refresh();
}

\n {
    mvprintw(row++, col, "%s", yytext);
    col = 0;
    refresh();
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

void runLexer(const std::string& input) {
    std::istringstream iss(input);
    yyFlexLexer lexer(&iss);

    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

    lexer.yylex();

    

    getch();
    endwin();
}