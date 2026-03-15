#include "operation_gen.h"
#include "operand_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper: Generate code for a simple expression (literal or identifier)
// Note: This is a simplified version for function arguments only.
// Complex expressions should already be processed into temporaries by CFG builder.
// String literals are handled by collect_string_literals_from_cfg() - not here!
static void generate_simple_expression(FunctionContext *ctx, ASTNode *expr, VMRegister target_reg) {
  if (!expr) return;

  if (strcmp(expr->type, "literal") == 0 && expr->value) {
    const char *value = expr->value;

    // String literal - label should exist from collect_string_literals_from_cfg()
    if (value[0] == '"' && strlen(value) >= 2) {
        char str_label[256];
        snprintf(str_label, sizeof(str_label), "str_%d", ctx->string_idx++);
        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                        vm_create_label_operand(str_label));
    } else {
        // Numeric literal
        int32_t num_value = atoi(value);
        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                        vm_create_immediate_operand(num_value));
    }
  } else if (strcmp(expr->type, "identifier") == 0 && expr->value) {
    // Variable or temporary
    int64_t offset = get_variable_stack_offset(ctx, expr->value);
    uint32_t var_ram_addr = get_variable_ram_address(ctx, expr->value);

    if (offset != 0) {
      add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(target_reg),
                      vm_create_bp_offset_operand(offset));
    } else if (var_ram_addr > 0) {
      add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(target_reg),
                      vm_create_memory_operand(var_ram_addr));
    }
  }
  // Note: Binary expressions, function calls, etc. should already be
  // processed into temporaries by CFG builder at this point.
}

void generate_operation(FunctionContext *ctx, Operation *op) {
  if (!op)
    return;

  // Track register allocations for constants (need to free them)
  bool freed_left_const = false;
  bool freed_right_const = false;

  // Allocate registers dynamically
  VMRegister left_reg = R0;
  VMRegister right_reg = R1;

  // Load operands into registers
  // For simple operands (const, var, temp), we need to target registers
  if (op->op1.kind != OPND_UNDEF && op->op1.kind != OPND_LABEL) {
    if (op->op1.kind == OPND_VAR) {
      left_reg = get_variable_register(ctx, op->op1.value.name);
    } else if (op->op1.kind == OPND_TEMP) {
      left_reg = get_temp_register(ctx, op->op1.value.temp_id);
    } else {
      // Constant - allocate register and load
      left_reg = allocate_register(&ctx->reg_allocator);
      init_operand(ctx, &op->op1, left_reg);
      freed_left_const = true;  // Mark as temporarly allocated
    }
  }

  if (op->op2.kind != OPND_UNDEF && op->op2.kind != OPND_LABEL) {
    if (op->op2.kind == OPND_VAR) {
      right_reg = get_variable_register(ctx, op->op2.value.name);
    } else if (op->op2.kind == OPND_TEMP) {
      right_reg = get_temp_register(ctx, op->op2.value.temp_id);
    } else {
      // Constant - allocate register and load
      right_reg = allocate_register(&ctx->reg_allocator);
      init_operand(ctx, &op->op2, right_reg);
      freed_right_const = true;  // Mark as temporarily allocated
    }
  }

  // Generate instruction based on operation type
  switch (op->type) {
  case OP_ADD:
    add_instruction(ctx->program, VM_ADD, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg));
    break;
  case OP_SUB:
    add_instruction(ctx->program, VM_SUB, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg));
    break;
  case OP_MUL:
    add_instruction(ctx->program, VM_MUL, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg));
    break;
  case OP_DIV:
    add_instruction(ctx->program, VM_DIV, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg));
    break;
  case OP_LT:
  case OP_GT:
  case OP_EQ:
    add_instruction(ctx->program, VM_CMP, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg));
    break;
  case OP_STORE: // op1 is value, op2 is destination (variable)
    store_operand(ctx, &op->op2, left_reg);  // Use left_reg instead of R0
    break;
  case OP_CONST: // Load constant into register, then store to destination
    // Load constant into left_reg (already allocated above for constants)
    if (op->op1.kind == OPND_CONST) {
      if (!freed_left_const) {
        // left_reg wasn't allocated for constant, need to allocate now
        left_reg = allocate_register(&ctx->reg_allocator);
        freed_left_const = true;
      }
      add_instruction(
          ctx->program, VM_MOV, vm_create_register_operand(left_reg),
          vm_create_immediate_operand((int32_t)op->op1.value.const_val));

      if (op->dest.kind == OPND_VAR &&
          is_variable_on_stack(ctx, op->dest.value.name)) {
        store_operand(ctx, &op->dest, left_reg);
      }
    }
    break;
  case OP_CALL: {
    // Function call
    if (op->ast_node && op->ast_node->child_count >= 2) {
      ASTNode *arg_list = op->ast_node->children[1];
      if (strcmp(arg_list->type, "exprList") == 0) {
        for (int i = 0; i < arg_list->child_count && i < 5; i++) {
          // Allocate register dynamically for each argument
          VMRegister arg_reg = allocate_register(&ctx->reg_allocator);
          generate_simple_expression(ctx, arg_list->children[i], arg_reg);
          // Register is freed after call in standard calling convention
          // For now, we'll leave it allocated as the calling convention isn't fully defined
        }
      }
    }

    if (op->op1.kind == OPND_LABEL) {
      add_instruction(ctx->program, VM_CALL,
                      vm_create_label_operand(op->op1.value.name),
                      vm_create_operand());
    }
    break;
  }
  case OP_JMP:
    if (op->next_target) {
      add_instruction(ctx->program, VM_JMP,
                      vm_create_label_operand(op->next_target),
                      vm_create_operand());
    }
    break;
  case OP_CJMP: {
    // Conditional jump - need to compare and jump
    if (op->true_target && op->false_target) {
      VMRegister cmp_reg = R0;

      if (op->op1.kind == OPND_CONST) {
        int64_t const_val = op->op1.value.const_val;

        if (left_reg != R0) {
          add_instruction(ctx->program, VM_MOV, vm_create_register_operand(R0),
                          vm_create_immediate_operand((int32_t)const_val));
          cmp_reg = R0;
        }

        add_instruction(ctx->program, VM_CMP,
                        vm_create_register_operand(cmp_reg),
                        vm_create_immediate_operand(0));

        if (const_val != 0) {
          add_instruction(ctx->program, VM_JNE,
                          vm_create_label_operand(op->true_target),
                          vm_create_operand());
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(op->false_target),
                          vm_create_operand());
        } else {
          // const_val == 0 (false):
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(op->false_target),
                          vm_create_operand());
        }
      } else {
        // Normal comparison (a > 5)
        // Operands already loaded in left_reg and right_reg
        // Comparison already done in OP_LT/OP_GT

        // Determine conditional jump based on cmp_type
        VMInstructionType jump_type = VM_JE;

        switch (op->cmp_type) {
        case OP_LT:
          jump_type = VM_JL; // Jump if Less (true means left < right)
          break;
        case OP_GT:
          jump_type = VM_JG; // Jump if Greater (true means left > right)
          break;
        case OP_EQ:
          jump_type = VM_JE; // Jump if Equal (true means left == right)
          break;
        default:
          jump_type = VM_JE; // Default to equal
          break;
        }

        add_instruction(ctx->program, jump_type,
                        vm_create_label_operand(op->true_target),
                        vm_create_operand());
        add_instruction(ctx->program, VM_JMP,
                        vm_create_label_operand(op->false_target),
                        vm_create_operand());
      }
    }
    break;
  }
  default:
    break;
  }

  // Store result if needed
  // Note: For arithmetic operations, left_reg contains the result
  // because the operations are in-place (op1 = op1 op op2)
  bool result_stored = false;
  if (op->dest.kind != OPND_UNDEF && op->type != OP_STORE && op->type != OP_CONST) {
    if (op->dest.kind == OPND_VAR &&
        is_variable_on_stack(ctx, op->dest.value.name)) {
      store_operand(ctx, &op->dest, left_reg);  // left_reg contains result
      result_stored = true;
    }
  }

  // Free temporarily allocated registers for constants
  // Only free if they weren't saved as the result
  if (freed_left_const && !result_stored && op->dest.kind != OPND_CONST) {
    free_register(&ctx->reg_allocator, left_reg);
  }
  if (freed_right_const) {
    free_register(&ctx->reg_allocator, right_reg);
  }
}
