#include "cfg.h"
#include "../1/ast.h"
#include <stdio.h>
#include <string.h>

static void print_op_brief(ASTNode *node, FILE *out, int depth) {
    if (!node || depth > 3) return;
    
    if (node->value) {
        // Экранируем кавычки в значении
        for (const char *p = node->value; *p; p++) {
            if (*p == '"') {
                fprintf(out, "\\\"");
            } else if (*p == '\\') {
                fprintf(out, "\\\\");
            } else {
                fprintf(out, "%c", *p);
            }
        }
    } else {
        fprintf(out, "%s", node->type);
    }
    
    if (node->child_count > 0 && depth < 3) {
        fprintf(out, "(");
        for (int i = 0; i < node->child_count && i < 3; i++) {
            if (i > 0) fprintf(out, ", ");
            print_op_brief(node->children[i], out, depth + 1);
        }
        if (node->child_count > 3) {
            fprintf(out, ", ...");
        }
        fprintf(out, ")");
    }
}

void print_cfg_dot(CFG *cfg, FILE *out) {
    if (!cfg || !out) return;
    
    fprintf(out, "digraph CFG_%s {\n", cfg->function->name);
    fprintf(out, "    node [shape=box, style=filled, fillcolor=lightblue];\n");
    fprintf(out, "    rankdir=TB;\n\n");
    
    fprintf(out, "    label=\"Function: %s", cfg->function->name);
    if (cfg->function->return_type) {
        fprintf(out, " : %s", cfg->function->return_type);
    }
    fprintf(out, "\";\n");
    fprintf(out, "    labelloc=t;\n\n");
    
    if (cfg->function->arg_count > 0) {
        fprintf(out, "    args [shape=note, fillcolor=lightyellow, label=\"Arguments:\\n");
        for (int i = 0; i < cfg->function->arg_count; i++) {
            fprintf(out, "%s", cfg->function->args[i]->name);
            if (cfg->function->args[i]->type) {
                fprintf(out, ": %s", cfg->function->args[i]->type);
            }
            if (i < cfg->function->arg_count - 1) {
                fprintf(out, "\\n");
            }
        }
        fprintf(out, "\"];\n\n");
    }
    
    if (cfg->function->local_var_count > 0) {
        fprintf(out, "    locals [shape=note, fillcolor=lightgreen, label=\"Local Variables:\\n");
        for (int i = 0; i < cfg->function->local_var_count; i++) {
            fprintf(out, "%s", cfg->function->local_vars[i]->name);
            if (cfg->function->local_vars[i]->type) {
                fprintf(out, ": %s", cfg->function->local_vars[i]->type);
            }
            fprintf(out, " (offset: %d)", cfg->function->local_vars[i]->offset);
            if (i < cfg->function->local_var_count - 1) {
                fprintf(out, "\\n");
            }
        }
        fprintf(out, "\"];\n\n");
    }
    
    // Выводим все базовые блоки
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        
        if (block == cfg->function->entry_block) {
            fprintf(out, "    BB%d [fillcolor=lightgreen, label=\"BB%d\\n%s\\n[ENTRY]", 
                    block->id, block->id, block->label);
        } else if (block == cfg->function->exit_block) {
            fprintf(out, "    BB%d [fillcolor=lightcoral, label=\"BB%d\\n%s\\n[EXIT]", 
                    block->id, block->id, block->label);
        } else {
            fprintf(out, "    BB%d [label=\"BB%d\\n%s", block->id, block->id, block->label);
        }
        
        // Добавляем мета-информацию о строках
        if (block->line_start > 0 || block->line_end > 0) {
            fprintf(out, "\\nLines: %d-%d", block->line_start, block->line_end);
        }
        
        fprintf(out, "\\nPredecessors:");
        if (block->predecessor_count > 0) {
            for (int j = 0; j < block->predecessor_count; j++) {
                fprintf(out, " BB%d", block->predecessors[j]->id);
            }
        } else {
            fprintf(out, " (none)");
        }
        
        fprintf(out, "\\nSuccessors:");
        if (block->successor_count > 0) {
            for (int j = 0; j < block->successor_count; j++) {
                fprintf(out, " BB%d", block->successors[j]->id);
            }
        } else {
            fprintf(out, " (none)");
        }
        
        if (block->operations) {
            fprintf(out, "\\n");
            print_op_brief(block->operations, out, 0);
        }
        
        fprintf(out, "\"];\n");
    }
    
    fprintf(out, "\n");
    
    // Выводим рёбра successors
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        for (int j = 0; j < block->successor_count; j++) {
            BasicBlock *succ = block->successors[j];
            BasicBlock *prev_block = NULL;
            
            // Определяем label для successor
            if (block->successor_count == 2) {
                if (j == 0) {
                    fprintf(out, "    BB%d -> BB%d [label=\"on condition (true)\", color=green];\n",
                            block->id, succ->id);
                } else {
                    fprintf(out, "    BB%d -> BB%d [label=\"default (false)\", color=red];\n",
                            block->id, succ->id);
                }
            } else {
                fprintf(out, "    BB%d -> BB%d [label=\"default\"];\n", block->id, succ->id);
            }
        }
    }
    
    fprintf(out, "}\n");
}

void print_call_graph_dot(CFG **cfgs, int cfg_count, FILE *out) {
    if (!cfgs || cfg_count <= 0 || !out) return;
    
    fprintf(out, "digraph CallGraph {\n");
    fprintf(out, "    node [shape=ellipse, style=filled, fillcolor=lightblue];\n");
    fprintf(out, "    rankdir=LR;\n");
    fprintf(out, "    label=\"Call Graph\";\n");
    fprintf(out, "    labelloc=t;\n\n");
    
    for (int i = 0; i < cfg_count; i++) {
        if (!cfgs[i] || !cfgs[i]->function) continue;
        
        const char *func_name = cfgs[i]->function->name;
        fprintf(out, "    %s [label=\"%s", func_name, func_name);
        
        if (cfgs[i]->function->return_type) {
            fprintf(out, "\\n: %s", cfgs[i]->function->return_type);
        }
        
        if (strcmp(func_name, "main") == 0) {
            fprintf(out, "\", fillcolor=lightgreen];\n");
        } else {
            fprintf(out, "\"];\n");
        }
    }
    
    fprintf(out, "\n");
    
    for (int i = 0; i < cfg_count; i++) {
        if (!cfgs[i]) continue;
        CFG *cfg = cfgs[i];
        const char *caller = cfg->function->name;
        
        for (int j = 0; j < cfg->block_count; j++) {
            BasicBlock *block = cfg->blocks[j];
            if (!block->operations) continue;
            
            ASTNode *node = block->operations;
            if (node && strcmp(node->type, "AST_EXPR_STMT") == 0 && node->child_count > 0) {
                node = node->children[0];
            }
            
            if (node && strcmp(node->type, AST_CALL) == 0 && 
                node->child_count > 0 && node->children[0]) {
                ASTNode *func_expr = node->children[0];
                if (func_expr->value) {
                    const char *callee = func_expr->value;
                    
                    bool found = false;
                    for (int k = 0; k < cfg_count; k++) {
                        if (cfgs[k] && cfgs[k]->function && 
                            strcmp(cfgs[k]->function->name, callee) == 0) {
                            found = true;
                            break;
                        }
                    }
                    
                    if (found) {
                        fprintf(out, "    %s -> %s;\n", caller, callee);
                    }
                }
            }
        }
    }
    
    fprintf(out, "}\n");
}
