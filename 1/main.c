#include <stdio.h>
#include "ast.h"

// Эти функции и переменные создаются Bison/Flex
int yyparse(void);
int yylex_destroy(void);
extern FILE *yyin;
extern int yylineno;
extern ASTNode *root; // объяви root в parser.y как глобальную переменную

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output dot file>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Error opening input file");
        return 1;
    }
    yyin = in;

    printf("Parsing %s...\n", argv[1]);
    int result = yyparse();
    fclose(in);

    if (result == 0) {
        printf("Parse successful.\n");

        FILE *out = fopen(argv[2], "w");
        if (!out) {
            perror("Error opening output file");
            return 1;
        }
        print_dot(root, out);
        fclose(out);
        printf("AST written to %s\n", argv[2]);
    } else {
        printf("Parse failed at line %d.\n", yylineno);
    }

    yylex_destroy();
    free_ast(root);
    return 0;
}
