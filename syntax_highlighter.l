

%{
#include <iostream>
#include <FlexLexer.h>

void apply_color();

%}

%option noyywrap
%option c++

%%

"int"|"float"|"if"|"else"|"return" {
    std::cout << "\033[31m";             
    std::cout << yytext;           
    std::cout << "\033[0m";              
}

\"([^"\\]|\\.)*\" { 
    std::cout << "\033[32m";               
    std::cout << yytext;
    std::cout << "\033[0m";
}

"//".* { 
    std::cout << "\033[33m";               
    std::cout << yytext;
    std::cout << "\033[0m";
}

"/*"([^*]|\*+[^*/])*\*+"/" {
    std::cout << "\033[33m";       
    std::cout << yytext;
    std::cout << "\033[0m";
}

[-+]?[0-9]+(\.[0-9]+)? {
    std::cout << "\033[34m";           
    std::cout << yytext;
    std::cout << "\033[0m";
}

[a-zA-Z_][a-zA-Z0-9_]* {
    std::cout << "\033[35m";              
    std::cout << yytext;
    std::cout << "\033[0m";
}

. { 
    std::cout << yytext;         
}

\n { 
    std::cout << "\n";           
}

%%

/* Custom Method Implementation */
void apply_color(char i) {
    yyFlexLexer lexer;  
    lexer.yylex();  
}
