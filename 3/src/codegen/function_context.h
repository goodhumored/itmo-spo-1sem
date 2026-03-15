#ifndef FUNCTION_CONTEXT_H
#define FUNCTION_CONTEXT_H

#include "../../../2/cfg.h"
#include "../asm_types.h"
#include "register_allocator.h"

typedef struct {
  char *name;
  int offset;
  VMRegister reg;
  bool in_register;
  bool is_initialized;
  uint32_t ram_address;
} LocalVarMapping;

typedef struct {
  char *name;
  int index;
  VMRegister reg;
  bool in_register;
} ArgumentMapping;

typedef struct {
  int temp_id;
  VMRegister reg;
  int32_t stack_offset;
  bool in_reg;
} TempMapping;

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
  int next_temp_id;
  int string_idx;
  TempMapping *temps;
  int temp_count;
  int max_temps;
} FunctionContext;

FunctionContext *create_function_context(VMProgram *program,
                                         Function *function);
void free_function_context(FunctionContext *ctx);
VMRegister get_variable_register(FunctionContext *ctx, const char *var_name);
void mark_variable_initialized(FunctionContext *ctx, const char *var_name);
int64_t get_variable_stack_offset(FunctionContext *ctx, const char *var_name);
uint32_t get_variable_ram_address(FunctionContext *ctx, const char *var_name);
VMRegister get_temp_register(FunctionContext *ctx, int temp_id);
void free_temp_register(FunctionContext *ctx, int temp_id);

#endif
