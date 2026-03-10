#include <stdio.h>
#include <stdlib.h>
#include "../1/ast.h"

extern int yyparse(void);
extern int yylex_destroy(void);
extern FILE *yyin;
extern ASTNode *root;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Error opening input file");
        return 1;
    }
    
    yyin = in;
    printf("Parsing %s...\n", argv[1]);
    
    int parse_result = yyparse();
    fclose(in);
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse failed\n");
        yylex_destroy();
        return 1;
    }
    
    if (root) {
        print_dot(root, stdout);
    }
    
    yylex_destroy();
    free_ast(root);
    
    return 0;
}