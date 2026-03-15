#include "operation_gen.h"
#include "operand_utils.h"
#include "../utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper: Generate comment string from CFG operation
static char* generate_operation_comment(Operation *op) {
    if (!op) return NULL;

    static char comment[256];
    const char *op_type_str = "";

    switch (op->type) {
        case OP_ADD: op_type_str = "ADD"; break;
        case OP_SUB: op_type_str = "SUB"; break;
        case OP_MUL: op_type_str = "MUL"; break;
        case OP_DIV: op_type_str = "DIV"; break;
        case OP_LT: op_type_str = "LT (<)"; break;
        case OP_GT: op_type_str = "GT (>)"; break;
        case OP_EQ: op_type_str = "EQ (=)"; break;
        case OP_STORE: op_type_str = "STORE"; break;
        case OP_LOAD: op_type_str = "LOAD"; break;
        case OP_CONST: op_type_str = "CONST"; break;
        case OP_CALL: op_type_str = "CALL"; break;
        case OP_JMP: op_type_str = "JMP"; break;
        case OP_CJMP: op_type_str = "CJMP"; break;
        case OP_RETURN: op_type_str = "RETURN"; break;
        case OP_NOP: op_type_str = "NOP"; break;
        default: op_type_str = "UNKNOWN"; break;
    }

    // Build comment with operands
    char op1_str[64] = "";
    char op2_str[64] = "";
    char dest_str[64] = "";

    if (op->op1.kind == OPND_VAR) {
        snprintf(op1_str, sizeof(op1_str), "%s", op->op1.value.name);
    } else if (op->op1.kind == OPND_CONST) {
        snprintf(op1_str, sizeof(op1_str), "%lld", op->op1.value.const_val);
    } else if (op->op1.kind == OPND_TEMP) {
        snprintf(op1_str, sizeof(op1_str), "t%d", op->op1.value.temp_id);
    } else if (op->op1.kind == OPND_LABEL) {
        snprintf(op1_str, sizeof(op1_str), "%s", op->op1.value.name);
    }

    if (op->op2.kind == OPND_VAR) {
        snprintf(op2_str, sizeof(op2_str), "%s", op->op2.value.name);
    } else if (op->op2.kind == OPND_CONST) {
        snprintf(op2_str, sizeof(op2_str), "%lld", op->op2.value.const_val);
    } else if (op->op2.kind == OPND_TEMP) {
        snprintf(op2_str, sizeof(op2_str), "t%d", op->op2.value.temp_id);
    }

    if (op->dest.kind == OPND_VAR) {
        snprintf(dest_str, sizeof(dest_str), "%s", op->dest.value.name);
    } else if (op->dest.kind == OPND_TEMP) {
        snprintf(dest_str, sizeof(dest_str), "t%d", op->dest.value.temp_id);
    }

    // Build final comment
    if (op->type == OP_STORE) {
        snprintf(comment, sizeof(comment), "%s: %s to %s", op_type_str, op1_str, op2_str);
    } else if (op->type == OP_CONST) {
        snprintf(comment, sizeof(comment), "%s: %s to %s", op_type_str, op1_str, dest_str);
    } else if (op->type >= OP_ADD && op->type <= OP_DIV) {
        snprintf(comment, sizeof(comment), "%s: %s = %s %s %s", op_type_str, dest_str, op1_str, op_type_str, op2_str);
    } else if (op->type == OP_CALL) {
        snprintf(comment, sizeof(comment), "%s: %s", op_type_str, op1_str);
    } else if (op->type == OP_CJMP) {
        const char *cmp_str = "";
        switch (op->cmp_type) {
            case OP_LT: cmp_str = "LT"; break;
            case OP_GT: cmp_str = "GT"; break;
            case OP_EQ: cmp_str = "EQ"; break;
            default: cmp_str = "?"; break;
        }
        snprintf(comment, sizeof(comment), "%s(%s): %s, targets: true=%s, false=%s", op_type_str, cmp_str, op1_str, op->true_target, op->false_target);
    } else {
        snprintf(comment, sizeof(comment), "%s", op_type_str);
    }

    return comment;
}

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

  // Generate comment for this operation
  char *op_comment = generate_operation_comment(op);
  int initial_instr_count = ctx->program->instruction_count;

  printf("\n=== Processing operation ===\n");
  printf("Type: %d, op1.kind=%d, op2.kind=%d, dest.kind=%d\n", op->type, op->op1.kind, op->op2.kind, op->dest.kind);
  if (op_comment) printf("Comment: %s\n", op_comment);

  // Track register allocations for constants (need to free them)
  bool freed_left_const = false;
  bool freed_right_const = false;

  // Allocate registers dynamically
  VMRegister left_reg = R0;
  VMRegister right_reg = R1;

  // Load operands into registers
  // IMPORTANT: For TYPE=STORE, don't load operands!
  // Source will be loaded in the STORE case itself
  printf("  DEBUG: op->type=%d (OP_STORE=8), would load op1=%d\n", op->type, OP_STORE, op->op1.kind);
  if (op->type != OP_STORE && op->op1.kind != OPND_UNDEF && op->op1.kind != OPND_LABEL) {
    printf("    Loading op1...\n");
    if (op->op1.kind == OPND_VAR) {
      left_reg = get_variable_register(ctx, op->op1.value.name);
      // Load variable from memory into register
      init_operand(ctx, &op->op1, left_reg);
    } else if (op->op1.kind == OPND_TEMP) {
      left_reg = get_temp_register(ctx, op->op1.value.temp_id);
      printf("    Loaded TEMP in reg R%d\n", left_reg);
    } else {
      // Constant - allocate register and load
      left_reg = allocate_register(&ctx->reg_allocator);
      init_operand(ctx, &op->op1, left_reg);
      freed_left_const = true;  // Mark as temporarly allocated
    }
  } else {
    printf("    Skipping op1 load (type=%d or op1.kind=%d)\n", op->type, op->op1.kind);
  }

  if (op->type != OP_STORE && op->op2.kind != OPND_UNDEF && op->op2.kind != OPND_LABEL) {
    if (op->op2.kind == OPND_VAR) {
      right_reg = get_variable_register(ctx, op->op2.value.name);
      // Load variable from memory into register
      init_operand(ctx, &op->op2, right_reg);
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
    add_instruction_with_comment(ctx->program, VM_ADD, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg), op_comment);
    if (op->dest.kind == OPND_TEMP) {
        set_temp_register(ctx, op->dest.value.temp_id, left_reg);
    }
    break;
  case OP_SUB:
    add_instruction_with_comment(ctx->program, VM_SUB, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg), op_comment);
    if (op->dest.kind == OPND_TEMP) {
        set_temp_register(ctx, op->dest.value.temp_id, left_reg);
    }
    break;
  case OP_MUL:
    add_instruction_with_comment(ctx->program, VM_MUL, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg), op_comment);
    if (op->dest.kind == OPND_TEMP) {
        set_temp_register(ctx, op->dest.value.temp_id, left_reg);
    }
    break;
  case OP_DIV:
    add_instruction_with_comment(ctx->program, VM_DIV, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg), op_comment);
    if (op->dest.kind == OPND_TEMP) {
        set_temp_register(ctx, op->dest.value.temp_id, left_reg);
    }
    break;
  case OP_LT:
  case OP_GT:
  case OP_EQ:
    add_instruction_with_comment(ctx->program, VM_CMP, vm_create_register_operand(left_reg),
                    vm_create_register_operand(right_reg), op_comment);
    if (op->dest.kind == OPND_TEMP) {
        set_temp_register(ctx, op->dest.value.temp_id, left_reg);
    }
    break;
  case OP_STORE: // op1 is value, op2 is destination (variable)
  // For STORE, we need to store op1 to op2
  printf("  DEBUG STORE: op1.kind=%d, op2.kind=%d, left_reg=%d\n", op->op1.kind, op->op2.kind, left_reg);
  // Use correct register for TEMP from temp mapping
  if (op->op1.kind == OPND_TEMP) {
    // TEMP from previous CALCULATION - get register from temp mapping
    VMRegister temp_reg = get_temp_register(ctx, op->op1.value.temp_id);
    printf("  STORE: TEMP - using stored register R%d\n", temp_reg);
    store_operand(ctx, &op->op2, temp_reg);
  } else if (op->op1.kind == OPND_VAR) {
    // op1 is a variable - load and store
    VMRegister src_reg = get_variable_register(ctx, op->op1.value.name);
    printf("  STORE: VAR - loading then storing, src_reg=R%d\n", src_reg);
    init_operand(ctx, &op->op1, src_reg);  // Load value from memory
    store_operand(ctx, &op->op2, src_reg);  // Store value to dest
  } else {
    // Constant or other - value is in left_reg
    printf("  STORE: CONST - storing left_reg\n");
    store_operand(ctx, &op->op2, left_reg);
  }
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

    // Set result register for CALL (standard calling convention: return value in R0)
    if (op->dest.kind == OPND_TEMP) {
      set_temp_register(ctx, op->dest.value.temp_id, R0);
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
  // because the operations are in-place (result is stored in first operand)
  // We need to store result NOW so that subsequent STORE can use it
  bool result_stored = false;
  if (op->dest.kind != OPND_UNDEF && op->type != OP_STORE && op->type != OP_CONST) {
    if (op->dest.kind == OPND_VAR && is_variable_on_stack(ctx, op->dest.value.name)) {
      // Store result from left_reg immediately
      store_operand(ctx, &op->dest, left_reg);
      result_stored = true;
    } else if (op->dest.kind == OPND_TEMP) {
      // For TEMP, also store to memory so STORE can load it
      // This prevents loss of intermediate results
      store_operand(ctx, &op->dest, left_reg);
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

  int added_instructions = ctx->program->instruction_count - initial_instr_count;
  printf("Added %d instructions (total: %d)\n", added_instructions, ctx->program->instruction_count);

  // DEBUG: Print details of added instructions
  printf("  Instructions %d to %d:\n", initial_instr_count + 1, ctx->program->instruction_count);
  for (int i = initial_instr_count; i < ctx->program->instruction_count; i++) {
    VMInstruction *instr = &ctx->program->instructions[i];
    printf("    [%d] %s", i + 1, get_instruction_name(instr->type));
    if (instr->operand_count > 0) {
      printf(" ");
      for (int j = 0; j < instr->operand_count; j++) {
        if (j > 0) printf(", ");
        VMOperand op = instr->operands[j];
        if (op.type == OP_REGISTER) {
          printf("R%d", op.value.reg);
        } else if (op.type == OP_IMMEDIATE) {
          printf("%d", op.value.immediate);
        } else if (op.type == OP_MEMORY) {
          printf("[%d]", op.value.address);
        } else if (op.type == OP_BP_OFFSET) {
          printf("[BP+%d]", op.value.offset);
        } else if (op.type == OP_LABEL) {
          printf("%s", op.value.label);
        }
      }
    }
    if (instr->comment) {
      printf("  // %s", instr->comment);
    }
    printf("\n");
  }
  printf("===============================\n\n");
}
