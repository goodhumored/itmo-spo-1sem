%{
#include "parser.tab.h"
#include <string.h>
%}

%option noyywrap
%option yylineno

%%

"bool"      { return BOOL_TYPE; }
"byte"      { return BYTE_TYPE; }
"int"       { return INT_TYPE; }
"uint"      { return UINT_TYPE; }
"long"      { return LONG_TYPE; }
"ulong"     { return ULONG_TYPE; }
"char"      { return CHAR_TYPE; }
"string"    { return STRING_TYPE; }

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"break"     { return BREAK; }

"true"      { yylval.bval = 1; return BOOL_LITERAL; }
"false"     { yylval.bval = 0; return BOOL_LITERAL; }

0[xX][0-9A-Fa-f]+   { yylval.sval = strdup(yytext); return HEX_LITERAL; }
0[bB][01]+          { yylval.sval = strdup(yytext); return BITS_LITERAL; }
[0-9]+              { yylval.sval = strdup(yytext); return DEC_LITERAL; }

\"([^\"\\\n]|\\.)*\"    { yylval.sval = strdup(yytext); return STR_LITERAL; }
'[^'\n]'                { yylval.cval = yytext[1]; return CHAR_LITERAL; }

[a-zA-Z_][a-zA-Z_0-9]*  { yylval.sval = strdup(yytext); return IDENTIFIER; }

"("         { return '('; }
")"         { return ')'; }
"{"         { return '{'; }
"}"         { return '}'; }
"["         { return '['; }
"]"         { return ']'; }
";"         { return ';'; }
","         { return ','; }
"="         { return '='; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }
"<"         { return '<'; }
"=="         { return EQEQ; }
">"         { return '>'; }
"!"         { return '!'; }
"&"         { return '&'; }
"|"         { return '|'; }
"^"         { return '^'; }
"~"         { return '~'; }

[ \t\n\r]+  { /* skip whitespace */ }
"//".*      { /* skip line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* skip block comments */ }

.           { fprintf(stderr, "Unknown character: %s\n", yytext); }

%%
