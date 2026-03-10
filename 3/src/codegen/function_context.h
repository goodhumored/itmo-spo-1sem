#ifndef FUNCTION_CONTEXT_H
#define FUNCTION_CONTEXT_H

#include "register_allocator.h"
#include "../asm_types.h"
#include "../../../2/cfg.h"

typedef struct {
    char *name;
    int offset;
    VMRegister reg;
    bool in_register;
} LocalVarMapping;

typedef struct {
    char *name;
    int index;
    VMRegister reg;
    bool in_register;
} ArgumentMapping;

typedef struct {
    VMProgram *program;
    Function *function;
    LocalVarMapping *local_vars;
    int local_var_count;
    ArgumentMapping *args;
    int arg_count;
    RegisterAllocator reg_allocator;
    int stack_offset;
    int current_address;
    int next_temp_id;  // для генерации t0, t1, t2, ...
    int string_idx;    // для генерации уникальных имен строк (str_0, str_1, ...)
} FunctionContext;

FunctionContext *create_function_context(VMProgram *program, Function *function);
void free_function_context(FunctionContext *ctx);
VMRegister get_variable_register(FunctionContext *ctx, const char *var_name);
int64_t get_variable_stack_offset(FunctionContext *ctx, const char *var_name);
VMRegister get_temp_register(FunctionContext *ctx, int temp_id);
void free_temp_register(FunctionContext *ctx, int temp_id);

#endif
