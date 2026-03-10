#ifndef CFG_H
#define CFG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "../1/ast.h"

typedef struct BasicBlock BasicBlock;
typedef struct CFG CFG;
typedef struct Function Function;
typedef struct LocalVar LocalVar;
typedef struct Argument Argument;
typedef struct Operand Operand;
typedef struct Operation Operation;

struct LocalVar {
    char *name;
    char *type;
    int offset;
};

struct Argument {
    char *name;
    char *type;
    int index;
};

typedef enum {
    OPND_VAR,
    OPND_CONST,
    OPND_TEMP,
    OPND_LABEL,
    OPND_UNDEF
} OperandKind;

typedef struct Operand {
    OperandKind kind;
    union {
        int64_t const_val;
        char *name;
        int temp_id;
    } value;
} Operand;

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_LT, OP_GT,
    OP_STORE, OP_LOAD,
    OP_CALL,
    OP_CONST,
    OP_PHI,
    OP_JMP,
    OP_CJMP,
    OP_RETURN,
    OP_NOP
} OperationType;

typedef struct Operation {
    OperationType type;
    Operand op1;
    Operand op2;
    Operand dest;

    char *label;
    char *true_target;
    char *false_target;
    char *next_target;
    
    // For OP_CJMP: stores the comparison type (OP_LT, OP_GT, etc.)
    OperationType cmp_type;

    int line_no;

    ASTNode *ast_node;
} Operation;

struct BasicBlock {
    int id;
    char *label;
    Operation **operations;
    int op_count;
    int max_ops;
    BasicBlock **successors;
    int successor_count;
    int max_successors;
    BasicBlock **predecessors;
    int predecessor_count;
    int max_predecessors;
    int line_start;
    int line_end;
    bool is_visited;
};

struct Function {
    char *name;
    char *return_type;
    Argument **args;
    int arg_count;
    LocalVar **local_vars;
    int local_var_count;
    char *source_file;
    BasicBlock *entry_block;
    BasicBlock *exit_block;
};

struct CFG {
    Function *function;
    BasicBlock **blocks;
    int block_count;
    int max_blocks;
    int next_block_id;
};

BasicBlock* create_basic_block(CFG *cfg, const char *label_prefix);
void add_successor(BasicBlock *from, BasicBlock *to);
void add_predecessor(BasicBlock *to, BasicBlock *from);

Operand create_const_operand(int64_t val);
Operand create_var_operand(const char *name);
Operand create_temp_operand(int temp_id);
Operand create_label_operand(const char *label);
Operand create_undef_operand();
void copy_operand(Operand *dest, const Operand *src);
void free_operand(Operand *op);

Operation* create_operation(OperationType type, Operand op1, Operand op2, Operand dest);
void free_operation(Operation *op);

void add_operation_to_block(BasicBlock *block, Operation *op);

CFG* create_cfg(const char *func_name, const char *return_type);
void add_block_to_cfg(CFG *cfg, BasicBlock *block);
void free_cfg(CFG *cfg);

CFG* build_cfg_from_function(ASTNode *func_node);

void print_cfg_dot(CFG *cfg, FILE *out);
void print_call_graph_dot(CFG **cfgs, int cfg_count, FILE *out);

void add_argument(Function *func, const char *name, const char *type);
void add_local_var(Function *func, const char *name, const char *type);

#endif
