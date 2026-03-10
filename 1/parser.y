%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
void yyerror(const char *s);

ASTNode *root = NULL;
%}

%code requires {
#include "ast.h"
}

%union {
    int bval;
    char cval;
    char *sval;
    ASTNode *node;
}

%token <sval> IDENTIFIER STR_LITERAL HEX_LITERAL BITS_LITERAL DEC_LITERAL
%token <bval> BOOL_LITERAL
%token <cval> CHAR_LITERAL

%token BOOL_TYPE BYTE_TYPE INT_TYPE UINT_TYPE LONG_TYPE ULONG_TYPE CHAR_TYPE STRING_TYPE
%token IF ELSE WHILE DO BREAK

%type <node> source sourceItem funcDef funcSignature argDefList argDef typeRef builtinType
%type <node> statement varDecl varList varItem ifStmt block statementList
%type <node> whileStmt doStmt breakStmt exprStmt expr exprList literal commaList

%right '='
%left '|'
%left '^'
%left '&'
%left '<' '>'
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' UNARY
%left '(' '['
%nonassoc THEN
%nonassoc ELSE

%%

source:
    /* empty */ { root = create_node("source", NULL); }
    | source sourceItem { if (!root) root = create_node("source", NULL); add_child(root, $2); }
    ;

sourceItem:
    funcDef { $$ = $1; }
    ;

funcDef:
    funcSignature ';' {
        $$ = create_node("funcDef", NULL);
        add_child($$, $1);
    }
    | funcSignature block {
        $$ = create_node("funcDef", NULL);
        add_child($$, $1);
        add_child($$, $2);
    }
    ;

funcSignature:
    typeRef IDENTIFIER '(' argDefList ')' {
        $$ = create_node("funcSignature", $2);
        add_child($$, $1);
        add_child($$, $4);
    }
    | IDENTIFIER '(' argDefList ')' {
        $$ = create_node("funcSignature", $1);
        add_child($$, $3);
    }
    ;

argDefList:
    /* empty */ { $$ = create_node("argDefList", NULL); }
    | argDef { $$ = create_node("argDefList", NULL); add_child($$, $1); }
    | argDefList ',' argDef { add_child($1, $3); $$ = $1; }
    ;

argDef:
    typeRef IDENTIFIER {
        $$ = create_node("argDef", $2);
        add_child($$, $1);
    }
    | IDENTIFIER { $$ = create_node("argDef", $1); }
    ;

typeRef:
    builtinType { $$ = $1; }
    | IDENTIFIER { $$ = create_node("customType", $1); }
    | typeRef '[' commaList ']' {
        $$ = create_node("arrayType", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    ;

builtinType:
    BOOL_TYPE { $$ = create_node("builtinType", "bool"); }
    | BYTE_TYPE { $$ = create_node("builtinType", "byte"); }
    | INT_TYPE { $$ = create_node("builtinType", "int"); }
    | UINT_TYPE { $$ = create_node("builtinType", "uint"); }
    | LONG_TYPE { $$ = create_node("builtinType", "long"); }
    | ULONG_TYPE { $$ = create_node("builtinType", "ulong"); }
    | CHAR_TYPE { $$ = create_node("builtinType", "char"); }
    | STRING_TYPE { $$ = create_node("builtinType", "string"); }
    ;

commaList:
    /* empty */ { $$ = create_node("dimensions", "0"); }
    | commaList ',' {
        char buf[32];
        int dim = atoi($1->value ? $1->value : "0") + 1;
        sprintf(buf, "%d", dim);
        free($1->value);
        $1->value = strdup(buf);
        $$ = $1;
    }
    ;

statement:
    varDecl { $$ = $1; }
    | ifStmt { $$ = $1; }
    | block { $$ = $1; }
    | whileStmt { $$ = $1; }
    | doStmt { $$ = $1; }
    | breakStmt { $$ = $1; }
    | exprStmt { $$ = $1; }
    ;

varDecl:
    typeRef varList ';' {
        $$ = create_node("varDecl", NULL);
        add_child($$, $1);
        add_child($$, $2);
    }
    ;

varList:
    varItem { $$ = create_node("varList", NULL); add_child($$, $1); }
    | varList ',' varItem { add_child($1, $3); $$ = $1; }
    ;

varItem:
    IDENTIFIER { $$ = create_node("var", $1); }
    | IDENTIFIER '=' expr {
        $$ = create_node("varInit", $1);
        add_child($$, $3);
    }
    ;

ifStmt:
    IF '(' expr ')' statement %prec THEN {
        $$ = create_node("if", NULL);
        add_child($$, $3);
        add_child($$, $5);
    }
    | IF '(' expr ')' statement ELSE statement {
        $$ = create_node("if", NULL);
        add_child($$, $3);
        add_child($$, $5);
        add_child($$, $7);
    }
    ;

block:
    '{' statementList '}' { $$ = $2; }
    ;

statementList:
    /* empty */ { $$ = create_node("block", NULL); }
    | statementList statement { add_child($1, $2); $$ = $1; }
    ;

whileStmt:
    WHILE '(' expr ')' statement {
        $$ = create_node("while", NULL);
        add_child($$, $3);
        add_child($$, $5);
    }
    ;

doStmt:
    DO block WHILE '(' expr ')' ';' {
        $$ = create_node("do", NULL);
        add_child($$, $2);
        add_child($$, $5);
    }
    ;

breakStmt:
    BREAK ';' { $$ = create_node("break", NULL); }
    ;

exprStmt:
    expr ';' { $$ = create_node("exprStmt", NULL); add_child($$, $1); }
    ;

expr:
    expr '=' expr { $$ = create_node("binary", "="); add_child($$, $1); add_child($$, $3); }
    | expr '+' expr { $$ = create_node("binary", "+"); add_child($$, $1); add_child($$, $3); }
    | expr '-' expr { $$ = create_node("binary", "-"); add_child($$, $1); add_child($$, $3); }
    | expr '*' expr { $$ = create_node("binary", "*"); add_child($$, $1); add_child($$, $3); }
    | expr '/' expr { $$ = create_node("binary", "/"); add_child($$, $1); add_child($$, $3); }
    | expr '%' expr { $$ = create_node("binary", "%"); add_child($$, $1); add_child($$, $3); }
    | expr '<' expr { $$ = create_node("binary", "<"); add_child($$, $1); add_child($$, $3); }
    | expr '>' expr { $$ = create_node("binary", ">"); add_child($$, $1); add_child($$, $3); }
    | expr '&' expr { $$ = create_node("binary", "&"); add_child($$, $1); add_child($$, $3); }
    | expr '|' expr { $$ = create_node("binary", "|"); add_child($$, $1); add_child($$, $3); }
    | expr '^' expr { $$ = create_node("binary", "^"); add_child($$, $1); add_child($$, $3); }
    | '!' expr %prec UNARY { $$ = create_node("unary", "!"); add_child($$, $2); }
    | '~' expr %prec UNARY { $$ = create_node("unary", "~"); add_child($$, $2); }
    | '-' expr %prec UNARY { $$ = create_node("unary", "-"); add_child($$, $2); }
    | '+' expr %prec UNARY { $$ = create_node("unary", "+"); add_child($$, $2); }
    | '(' expr ')' { $$ = create_node("braces", NULL); add_child($$, $2); }
    | expr '(' exprList ')' {
        $$ = create_node("call", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    | expr '[' exprList ']' {
        $$ = create_node("indexer", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    | IDENTIFIER { $$ = create_node("identifier", $1); }
    | literal { $$ = $1; }
    ;

exprList:
    /* empty */ { $$ = create_node("exprList", NULL); }
    | expr { $$ = create_node("exprList", NULL); add_child($$, $1); }
    | exprList ',' expr { add_child($1, $3); $$ = $1; }
    ;

literal:
    BOOL_LITERAL { $$ = create_node("literal", $1 ? "true" : "false"); }
    | STR_LITERAL { $$ = create_node("literal", $1); }
    | CHAR_LITERAL { char buf[2] = {$1, 0}; $$ = create_node("literal", buf); }
    | HEX_LITERAL { $$ = create_node("literal", $1); }
    | BITS_LITERAL { $$ = create_node("literal", $1); }
    | DEC_LITERAL { $$ = create_node("literal", $1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}
