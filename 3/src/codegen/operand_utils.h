#ifndef OPERAND_UTILS_H

#define OPERAND_UTILS_H

#include "function_context.h"
#include "../../../2/cfg.h"
#include "../asm_types.h"

// Check if variable is located on stack (as opposed to global RAM)
bool is_variable_on_stack(FunctionContext *ctx, const char *var_name);

// Generate code to initialize an operand (load value into register)
void init_operand(FunctionContext *ctx, Operand *ir_op, VMRegister target_reg);

// Load IR operand value into a VM register (returns register to use)
VMRegister load_operand_to_reg(FunctionContext *ctx, Operand *ir_op);

// Generate code to store value from register to operand destination
void store_operand(FunctionContext *ctx, Operand *dest_op, VMRegister value_reg);

#endif
