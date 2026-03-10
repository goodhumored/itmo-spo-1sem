#include "cfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Operand creation functions

Operand create_const_operand(int64_t val) {
    Operand op = {0};
    op.kind = OPND_CONST;
    op.value.const_val = val;
    return op;
}

Operand create_var_operand(const char *name) {
    Operand op = {0};
    op.kind = OPND_VAR;
    op.value.name = name ? strdup(name) : NULL;
    return op;
}

Operand create_temp_operand(int temp_id) {
    Operand op = {0};
    op.kind = OPND_TEMP;
    op.value.temp_id = temp_id;
    return op;
}

Operand create_label_operand(const char *label) {
    Operand op = {0};
    op.kind = OPND_LABEL;
    op.value.name = label ? strdup(label) : NULL;
    return op;
}

Operand create_undef_operand() {
    Operand op = {0};
    op.kind = OPND_UNDEF;
    return op;
}

void copy_operand(Operand *dest, const Operand *src) {
    if (!dest || !src) return;

    dest->kind = src->kind;
    switch (src->kind) {
        case OPND_CONST:
            dest->value.const_val = src->value.const_val;
            break;
        case OPND_VAR:
        case OPND_LABEL:
            dest->value.name = src->value.name ? strdup(src->value.name) : NULL;
            break;
        case OPND_TEMP:
            dest->value.temp_id = src->value.temp_id;
            break;
        case OPND_UNDEF:
            break;
    }
}

void free_operand(Operand *op) {
    if (!op) return;
    if (op->kind == OPND_VAR || op->kind == OPND_LABEL) {
        free(op->value.name);
        op->value.name = NULL;
    }
    op->kind = OPND_UNDEF;
}

// Operation creation functions

Operation* create_operation(OperationType type, Operand op1, Operand op2, Operand dest) {
    Operation *op = malloc(sizeof(Operation));
    if (!op) {
        fprintf(stderr, "Error: Failed to allocate Operation\n");
        exit(1);
    }

    op->type = type;
    copy_operand(&op->op1, &op1);
    copy_operand(&op->op2, &op2);
    copy_operand(&op->dest, &dest);

    op->label = NULL;
    op->true_target = NULL;
    op->false_target = NULL;
    op->next_target = NULL;
    op->cmp_type = OP_NOP;
    op->line_no = -1;
    op->ast_node = NULL;

    return op;
}

void free_operation(Operation *op) {
    if (!op) return;

    free_operand(&op->op1);
    free_operand(&op->op2);
    free_operand(&op->dest);

    free(op->label);
    free(op->true_target);
    free(op->false_target);
    free(op->next_target);

    free(op);
}

void add_operation_to_block(BasicBlock *block, Operation *op) {
    if (!block || !op) return;

    if (block->op_count >= block->max_ops) {
        block->max_ops = block->max_ops == 0 ? 4 : block->max_ops * 2;
        block->operations = realloc(block->operations,
                                   block->max_ops * sizeof(Operation*));
    }
    block->operations[block->op_count++] = op;
}

BasicBlock* create_basic_block(CFG *cfg, const char *label_prefix) {
    BasicBlock *block = malloc(sizeof(BasicBlock));
    if (!block) {
        fprintf(stderr, "Error: Failed to allocate BasicBlock\n");
        exit(1);
    }

    block->id = cfg->next_block_id++;

    size_t label_size = strlen(label_prefix) + 20;
    block->label = malloc(label_size);
    snprintf(block->label, label_size, "%s_%d", label_prefix, block->id);

    block->operations = NULL;
    block->op_count = 0;
    block->max_ops = 0;
    block->successors = NULL;
    block->successor_count = 0;
    block->max_successors = 0;
    block->predecessors = NULL;
    block->predecessor_count = 0;
    block->max_predecessors = 0;
    block->line_start = -1;
    block->line_end = -1;
    block->is_visited = false;

    return block;
}

void add_successor(BasicBlock *from, BasicBlock *to) {
    if (from->successor_count >= from->max_successors) {
        from->max_successors = from->max_successors == 0 ? 2 : from->max_successors * 2;
        from->successors = realloc(from->successors, 
                                   from->max_successors * sizeof(BasicBlock*));
    }
    from->successors[from->successor_count++] = to;
    add_predecessor(to, from);
}

void add_predecessor(BasicBlock *to, BasicBlock *from) {
    for (int i = 0; i < to->predecessor_count; i++) {
        if (to->predecessors[i] == from) {
            return;
        }
    }
    
    if (to->predecessor_count >= to->max_predecessors) {
        to->max_predecessors = to->max_predecessors == 0 ? 2 : to->max_predecessors * 2;
        to->predecessors = realloc(to->predecessors,
                                   to->max_predecessors * sizeof(BasicBlock*));
    }
    to->predecessors[to->predecessor_count++] = from;
}

CFG* create_cfg(const char *func_name, const char *return_type) {
    CFG *cfg = malloc(sizeof(CFG));
    if (!cfg) {
        fprintf(stderr, "Error: Failed to allocate CFG\n");
        exit(1);
    }
    
    cfg->function = malloc(sizeof(Function));
    cfg->function->name = strdup(func_name);
    cfg->function->return_type = return_type ? strdup(return_type) : NULL;
    cfg->function->args = NULL;
    cfg->function->arg_count = 0;
    cfg->function->local_vars = NULL;
    cfg->function->local_var_count = 0;
    cfg->function->source_file = NULL;
    cfg->function->entry_block = NULL;
    cfg->function->exit_block = NULL;
    
    cfg->blocks = NULL;
    cfg->block_count = 0;
    cfg->max_blocks = 0;
    cfg->next_block_id = 0;
    
    return cfg;
}

void add_block_to_cfg(CFG *cfg, BasicBlock *block) {
    if (cfg->block_count >= cfg->max_blocks) {
        cfg->max_blocks = cfg->max_blocks == 0 ? 4 : cfg->max_blocks * 2;
        cfg->blocks = realloc(cfg->blocks, cfg->max_blocks * sizeof(BasicBlock*));
    }
    cfg->blocks[cfg->block_count++] = block;
}

void add_argument(Function *func, const char *name, const char *type) {
    Argument *arg = malloc(sizeof(Argument));
    arg->name = strdup(name);
    arg->type = type ? strdup(type) : NULL;
    arg->index = func->arg_count;
    
    func->args = realloc(func->args, (func->arg_count + 1) * sizeof(Argument*));
    func->args[func->arg_count++] = arg;
}

void add_local_var(Function *func, const char *name, const char *type) {
    LocalVar *var = malloc(sizeof(LocalVar));
    var->name = strdup(name);
    var->type = type ? strdup(type) : NULL;
    var->offset = (func->local_var_count + 1) * 4;

    func->local_vars = realloc(func->local_vars,
                               (func->local_var_count + 1) * sizeof(LocalVar*));
    func->local_vars[func->local_var_count++] = var;
}

void free_basic_block(BasicBlock *block) {
    if (!block) return;

    free(block->label);

    for (int i = 0; i < block->op_count; i++) {
        free_operation(block->operations[i]);
    }
    free(block->operations);

    free(block->successors);
    free(block->predecessors);
    free(block);
}

void free_cfg(CFG *cfg) {
    if (!cfg) return;
    
    if (cfg->function) {
        free(cfg->function->name);
        free(cfg->function->return_type);
        free(cfg->function->source_file);
        
        for (int i = 0; i < cfg->function->arg_count; i++) {
            free(cfg->function->args[i]->name);
            free(cfg->function->args[i]->type);
            free(cfg->function->args[i]);
        }
        free(cfg->function->args);
        
        for (int i = 0; i < cfg->function->local_var_count; i++) {
            free(cfg->function->local_vars[i]->name);
            free(cfg->function->local_vars[i]->type);
            free(cfg->function->local_vars[i]);
        }
        free(cfg->function->local_vars);
        
        free(cfg->function);
    }
    
    for (int i = 0; i < cfg->block_count; i++) {
        free_basic_block(cfg->blocks[i]);
    }
    free(cfg->blocks);
    
    free(cfg);
}
