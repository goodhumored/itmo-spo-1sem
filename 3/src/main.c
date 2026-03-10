#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "utils.h"
#include "../../2/cfg.h"

extern int yyparse(void);
extern int yylex_destroy(void);
extern FILE *yyin;
extern ASTNode *root;

static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input_file> <output_dir>\n", prog_name);
    fprintf(stderr, "  <input_file>  - source file to parse\n");
    fprintf(stderr, "  <output_dir>  - directory for output files (default: current dir)\n");
}

static void collect_functions(ASTNode *source, ASTNode ***funcs, int *func_count) {
    if (!source || strcmp(source->type, "source") != 0) return;
    
    *func_count = 0;
    for (int i = 0; i < source->child_count; i++) {
        ASTNode *item = source->children[i];
        if (item && strcmp(item->type, "funcDef") == 0) {
            (*func_count)++;
        }
    }
    
    if (*func_count == 0) return;
    
    *funcs = malloc((*func_count) * sizeof(ASTNode*));
    int idx = 0;
    for (int i = 0; i < source->child_count; i++) {
        ASTNode *item = source->children[i];
        if (item && strcmp(item->type, "funcDef") == 0) {
            (*funcs)[idx++] = item;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_dir = (argc >= 3) ? argv[2] : ".";
    
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Error opening input file");
        return 1;
    }
    
    yyin = in;
    printf("Parsing %s...\n", input_file);
    
    int parse_result = yyparse();
    fclose(in);
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse failed\n");
        yylex_destroy();
        return 1;
    }
    
    printf("Parse successful. Building CFG...\n");
    
    ASTNode **functions = NULL;
    int func_count = 0;
    collect_functions(root, &functions, &func_count);
    
    if (func_count == 0) {
        fprintf(stderr, "No functions found in source\n");
        free(functions);
        yylex_destroy();
        free_ast(root);
        return 1;
    }
    
    printf("Found %d function(s)\n", func_count);
    
    CFG **cfgs = malloc(func_count * sizeof(CFG*));
    
    for (int i = 0; i < func_count; i++) {
        cfgs[i] = build_cfg_from_function(functions[i]);
        
        if (cfgs[i]) {
            printf("Built CFG for function: %s\\n", cfgs[i]->function->name);
        }
    }
    
    printf("\nGenerating code...\n");
    
    // Генерируем код
    VMProgram *program = generate_code(cfgs, func_count);
    if (!program) {
        fprintf(stderr, "Error: Failed to generate code\n");
        
        for (int i = 0; i < func_count; i++) {
            free_cfg(cfgs[i]);
        }
        free(cfgs);
        free(functions);
        yylex_destroy();
        free_ast(root);
        return 1;
    }
    
    printf("Code generation successful.\n");
    
    // Сохраняем ассемблерный листинг
    char listing_file[512];
    snprintf(listing_file, sizeof(listing_file), "%s/program.asm", output_dir);
    FILE *listing_out = fopen(listing_file, "w");
    if (listing_out) {
        print_program_listing(program, listing_out);
        fclose(listing_out);
        printf("Wrote assembly listing to %s\n", listing_file);
    } else {
        fprintf(stderr, "Error: Could not write to %s\n", listing_file);
    }
    
    // Освобождаем ресурсы
    free_vm_program(program);
    
    for (int i = 0; i < func_count; i++) {
        free_cfg(cfgs[i]);
    }
    free(cfgs);
    free(functions);
    
    yylex_destroy();
    free_ast(root);
    
    printf("\nDone!\n");
    
    return 0;
}
