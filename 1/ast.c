#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *create_node(const char *type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = strdup(type);
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    parent->children = realloc(parent->children, sizeof(ASTNode*) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++)
        free_ast(node->children[i]);
    free(node->children);
    free(node->type);
    if (node->value) free(node->value);
    free(node);
}

void print_dot_rec(ASTNode *node, FILE *out) {
    if (!node) return;
    fprintf(out, "node%p [label=\"%s", (void*)node, node->type);
    if (node->value) fprintf(out, "\\n%s", node->value);
    fprintf(out, "\"];\n");
    for (int i = 0; i < node->child_count; i++) {
        fprintf(out, "node%p -> node%p;\n", (void*)node, (void*)node->children[i]);
        print_dot_rec(node->children[i], out);
    }
}

void print_dot(ASTNode *node, FILE *out) {
    fprintf(out, "digraph AST {\n");
    print_dot_rec(node, out);
    fprintf(out, "}\n");
}
