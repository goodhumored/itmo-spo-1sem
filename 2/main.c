#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg.h"
#include "../1/ast.h"

extern int yyparse(void);
extern int yylex_destroy(void);
extern FILE *yyin;
extern ASTNode *root;

static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input_file> <output_dir>\n", prog_name);
    fprintf(stderr, "  <input_file>  - source file to parse\n");
    fprintf(stderr, "  <output_dir>  - directory for output .dot files (default: current dir)\n");
}

static ASTNode* find_child_by_type(ASTNode *node, const char *type) {
    if (!node) return NULL;
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i] && strcmp(node->children[i]->type, type) == 0) {
            return node->children[i];
        }
    }
    return NULL;
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
            printf("Built CFG for function: %s\n", cfgs[i]->function->name);
            
            char filename[512];
            snprintf(filename, sizeof(filename), "%s/%s.cfg.dot", 
                    output_dir, cfgs[i]->function->name);
            
            FILE *out = fopen(filename, "w");
            if (out) {
                print_cfg_dot(cfgs[i], out);
                fclose(out);
                printf("  Wrote CFG to %s\n", filename);
            } else {
                fprintf(stderr, "  Error: Could not write to %s\n", filename);
            }
            for (int j = 0; j < cfgs[i]->block_count; j++) {
            printf("block %d: %s\n",j, cfgs[i]->blocks[j]->label);
            }
        }
    }
    
    char call_graph_file[512];
    snprintf(call_graph_file, sizeof(call_graph_file), "%s/call_graph.dot", output_dir);
    FILE *cg_out = fopen(call_graph_file, "w");
    if (cg_out) {
        print_call_graph_dot(cfgs, func_count, cg_out);
        fclose(cg_out);
        printf("\nWrote call graph to %s\n", call_graph_file);
    }
    
    for (int i = 0; i < func_count; i++) {
        free_cfg(cfgs[i]);
    }
    free(cfgs);
    free(functions);
    
    yylex_destroy();
    free_ast(root);
    
    printf("\nDone! Generate images with:\n");
    printf("  dot -Tpng %s/*.dot -O\n", output_dir);
    
    return 0;
}
