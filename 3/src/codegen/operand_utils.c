#include "operand_utils.h"
#include "string_utils.h"
#include <stdio.h>
#include <string.h>

bool is_variable_on_stack(FunctionContext *ctx, const char *var_name) {
  int64_t offset = get_variable_stack_offset(ctx, var_name);
  return offset != 0;
}

void init_operand(FunctionContext *ctx, Operand *ir_op, VMRegister target_reg) {
  if (!ir_op)
    return;

  switch (ir_op->kind) {
  case OPND_CONST:
    add_instruction(
        ctx->program, VM_MOV, vm_create_register_operand(target_reg),
        vm_create_immediate_operand((int32_t)ir_op->value.const_val));
    break;
  case OPND_VAR: {
    if (is_variable_on_stack(ctx, ir_op->value.name)) {
      int64_t offset = get_variable_stack_offset(ctx, ir_op->value.name);
      add_instruction(ctx->program, VM_LOAD,
                      vm_create_register_operand(target_reg),
                      vm_create_bp_offset_operand(offset));
    } else {
      uint32_t var_ram_addr = get_variable_ram_address(ctx, ir_op->value.name);
      if (var_ram_addr > 0) {
        add_instruction(ctx->program, VM_LOAD,
                        vm_create_register_operand(target_reg),
                        vm_create_memory_operand(var_ram_addr));
      }
    }
    break;
  }
  case OPND_STRING_LITERAL: {
    // String literal - get the label from string_utils
    const char *str_label = get_string_label(ir_op->value.name);
    if (str_label) {
      // Load string (pointer) from label into register
      // Assuming the string is stored in memory and we load its address
      // For simple string comparison, we might want to compare character values
      add_instruction(ctx->program, VM_LOAD,
                      vm_create_register_operand(target_reg),
                      vm_create_label_operand(str_label));
    }
    break;
  }
  case OPND_TEMP:
    // Temp should already be in a register
    break;
  case OPND_LABEL:
  case OPND_UNDEF:
    break;
  }
}

VMRegister load_operand_to_reg(FunctionContext *ctx, Operand *ir_op) {
  if (!ir_op)
    return R0;

  switch (ir_op->kind) {
  case OPND_CONST: {
    // Allocate a register and load the constant into it
    VMRegister reg = allocate_register(&ctx->reg_allocator);
    add_instruction(
        ctx->program, VM_MOV, vm_create_register_operand(reg),
        vm_create_immediate_operand((int32_t)ir_op->value.const_val));
    return reg;
  }
  case OPND_VAR: {
    // Get register for variable
    VMRegister reg = get_variable_register(ctx, ir_op->value.name);
    if (is_variable_on_stack(ctx, ir_op->value.name)) {
      int64_t offset = get_variable_stack_offset(ctx, ir_op->value.name);
      add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(reg),
                      vm_create_bp_offset_operand(offset));
    } else {
      uint32_t var_ram_addr = get_variable_ram_address(ctx, ir_op->value.name);
      if (var_ram_addr > 0) {
        add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(reg),
                        vm_create_memory_operand(var_ram_addr));
      }
    }
    return reg;
  }
  case OPND_TEMP:
    // Get register for temporary
    return get_temp_register(ctx, ir_op->value.temp_id);
  case OPND_STRING_LITERAL: {
    // String literal - allocate a register and load from label
    VMRegister reg = allocate_register(&ctx->reg_allocator);
    const char *str_label = get_string_label(ir_op->value.name);
    if (str_label) {
      add_instruction(ctx->program, VM_LOAD,
                      vm_create_register_operand(reg),
                      vm_create_label_operand(str_label));
    }
    return reg;
  }
  case OPND_LABEL:
  case OPND_UNDEF:
    return R0;
  }
  return R0;
}

void store_operand(FunctionContext *ctx, Operand *dest_op,
                   VMRegister value_reg) {
  if (!dest_op || dest_op->kind != OPND_VAR)
    return;

  if (is_variable_on_stack(ctx, dest_op->value.name)) {
    int64_t offset = get_variable_stack_offset(ctx, dest_op->value.name);
    add_instruction(ctx->program, VM_STORE, vm_create_bp_offset_operand(offset),
                    vm_create_register_operand(value_reg));
  } else {
    uint32_t var_ram_addr = get_variable_ram_address(ctx, dest_op->value.name);
    if (var_ram_addr > 0) {
      add_instruction(ctx->program, VM_STORE,
                      vm_create_memory_operand(var_ram_addr),
                      vm_create_register_operand(value_reg));
    }
  }
}
