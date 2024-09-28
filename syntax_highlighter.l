%{
#include <ncurses.h>
#include <iostream>
#include <FlexLexer.h> // Include this header for yyFlexLexer

// Declare yyFlexLexer to make it accessible in other files
//class yyFlexLexer; // Forward declaration
extern "C" int yylex(); // No need to declare yylex() here
void apply_color(int color);
%}

%option noyywrap
%option c++

%{
    class MyLexer : public yyFlexLexer {
    public:
        void apply_color(int color);
    };
%}

%%

"int"|"float"|"if"|"else"|"return" {
    apply_color(1);
    std::cout << yytext;
    apply_color(0);
}

\"([^"\\]|\\.)*\" { 
    apply_color(2);
    std::cout << yytext;
    apply_color(0);
}

"//".* { 
    apply_color(3);
    std::cout << yytext;
    apply_color(0);
}

"/*"([^*]|\*+[^*/])*\*+"/" {
    apply_color(3);
    std::cout << yytext;
    apply_color(0);
}

[-+]?[0-9]+(\.[0-9]+)? {
    apply_color(4);
    std::cout << yytext;
    apply_color(0);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    apply_color(5);
    std::cout << yytext;
    apply_color(0);
}

. { 
    std::cout << yytext; 
}

\n { 
    std::cout << "\n"; 
}

%%

void MyLexer::apply_color(int color) {
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
