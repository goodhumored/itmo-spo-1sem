#ifndef AST_H
#define AST_H

#include <stdio.h>

// AST node type constants (from parser - verified against a.dot output)
#define AST_IF            "if"
#define AST_WHILE         "while"
#define AST_DO            "do"
#define AST_BREAK_STMT    "breakStmt"
#define AST_EXPR_STMT     "exprStmt"
#define AST_BLOCK         "block"
#define AST_VAR_DECL      "varDecl"
#define AST_BINARY        "binary"
#define AST_CALL          "call"
#define AST_UNARY         "unary"
#define AST_LITERAL       "literal"
#define AST_IDENTIFIER    "identifier"
#define AST_EXPR_LIST     "exprList"
#define AST_FUNC_DEF      "funcDef"
#define AST_FUNC_SIGNATURE "funcSignature"
#define AST_ARG_DEF_LIST  "argDefList"
#define AST_ARG_DEF       "argDef"
#define AST_VAR_LIST      "varList"
#define AST_BUILTIN_TYPE  "builtinType"
#define AST_CUSTOM_TYPE   "customType"
#define AST_ARRAY_TYPE    "arrayType"
#define AST_DIMENSIONS    "dimensions"

typedef struct ASTNode {
    char *type;              // тип узла (например "if", "varDecl", "literal", "binaryExpr")
    char *value;             // текстовое значение (например имя переменной, оператор)
    struct ASTNode **children;
    int child_count;
} ASTNode;

ASTNode *create_node(const char *type, const char *value);
void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_dot(ASTNode *node, FILE *out);

#endif
