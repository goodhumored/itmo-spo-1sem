#include "operation_gen.h"
#include "operand_utils.h"
#include "block_gen.h"
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

  printf("    DEBUG generate_simple_expression: expr->type='%s', value='%s', child_count=%d\n",
         expr->type, expr->value ? expr->value : "NULL", expr->child_count);

  if (strcmp(expr->type, "literal") == 0 && expr->value) {
    // Load constant literal into register
    int64_t val;
    if (sscanf(expr->value, "%lld", (long long *)&val) == 1) {
      add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                      vm_create_immediate_operand((int32_t)val));
      printf("      Loaded literal %lld into R%d\n", val, target_reg);
    }
    // Note: String literals should be handled separately via collect_string_literals_from_cfg
    // For function arguments, we don't support string literals yet
    return;
  }

  if (strcmp(expr->type, "identifier") == 0 && expr->value) {
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
    return;
  }

  // Note: Binary expressions, function calls, etc. should already be
  // processed into temporaries by CFG builder at this point.
  // But we handle simple binary expressions for function arguments like `n-2`

  // Handle binary expressions (e.g., n-2, n+1 for function arguments)
  // Note: CFG builder uses 'binary' type, not 'binaryExpr'
  if (strcmp(expr->type, "binary") == 0) {
    printf("      BINARY EXPRESSION DETECTED: op='%s'\n", expr->value);
    ASTNode *left = expr->children[0];
    ASTNode *right = expr->children[1];
    const char *op = expr->value;

    // Generate code for left operand
    printf("      Generating left operand (type=%s) into R%d\n", left->type, target_reg);
    generate_simple_expression(ctx, left, target_reg);

    // Allocate register for right operand
    VMRegister right_reg = allocate_register(&ctx->reg_allocator);
    printf("      Generating right operand (type=%s) into R%d\n", right->type, right_reg);
    generate_simple_expression(ctx, right, right_reg);

    // Perform the operation
    VMInstructionType instr_type = VM_MOV;

    if (strcmp(op, "+") == 0) {
      instr_type = VM_ADD;
    } else if (strcmp(op, "-") == 0) {
      instr_type = VM_SUB;
    } else if (strcmp(op, "*") == 0) {
      instr_type = VM_MUL;
    } else if (strcmp(op, "/") == 0) {
      instr_type = VM_DIV;
    }

    if (instr_type != VM_MOV) {
      printf("      Emitting %s R%d, R%d\n", (instr_type == VM_ADD ? "ADD" : instr_type == VM_SUB ? "SUB" : instr_type == VM_MUL ? "MUL" : "DIV"),
             target_reg, right_reg);
      add_instruction(ctx->program, instr_type,
                      vm_create_register_operand(target_reg),
                      vm_create_register_operand(right_reg));
    }

    free_register(&ctx->reg_allocator, right_reg);
  }
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

  // Load operands into registers using helper function
  // IMPORTANT: For TYPE=STORE, don't load operands!
  // Source will be loaded in the STORE case itself
  printf("  DEBUG: op->type=%d (OP_STORE=%d), would load op1=%d\n", op->type, OP_STORE, op->op1.kind);

  VMRegister left_reg = R0;
  VMRegister right_reg = R1;

  if (op->type != OP_STORE && op->op1.kind != OPND_UNDEF && op->op1.kind != OPND_LABEL) {
    printf("    Loading op1...\n");
    left_reg = load_operand_to_reg(ctx, &op->op1);
    freed_left_const = (op->op1.kind == OPND_CONST || op->op1.kind == OPND_STRING_LITERAL);
    printf("    Loaded op1 in reg R%d\n", left_reg);
  } else {
    printf("    Skipping op1 load (type=%d or op1.kind=%d)\n", op->type, op->op1.kind);
  }

  if (op->type != OP_STORE && op->op2.kind != OPND_UNDEF && op->op2.kind != OPND_LABEL) {
    printf("    Loading op2 (kind=%d)...\n", op->op2.kind);
    right_reg = load_operand_to_reg(ctx, &op->op2);
    freed_right_const = (op->op2.kind == OPND_CONST || op->op2.kind == OPND_STRING_LITERAL);
    printf("    Loaded op2 in reg R%d\n", right_reg);
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
  printf("  DEBUG STORE: op1.kind=%d, op2.kind=%d\n", op->op1.kind, op->op2.kind);
  // Load op1 into a register
  VMRegister src_reg = load_operand_to_reg(ctx, &op->op1);
  printf("  STORE: loaded op1 into R%d\n", src_reg);
  // Store to destination
  store_operand(ctx, &op->op2, src_reg);
  // Free register if it was allocated for const/string literal
  if (op->op1.kind == OPND_CONST || op->op1.kind == OPND_STRING_LITERAL) {
    free_register(&ctx->reg_allocator, src_reg);
  }
  break;
  case OP_CONST: // Load constant into register, then store to destination
    // Load constant into register using helper
    left_reg = load_operand_to_reg(ctx, &op->op1);
    freed_left_const = true;

    if (op->dest.kind == OPND_VAR &&
        is_variable_on_stack(ctx, op->dest.value.name)) {
      store_operand(ctx, &op->dest, left_reg);
    }
    break;
  case OP_CALL: {
    // Function call - push arguments in reverse order (stdcall convention)
    if (op->ast_node && op->ast_node->child_count >= 2) {
      ASTNode *arg_list = op->ast_node->children[1];
      if (strcmp(arg_list->type, "exprList") == 0) {
        for (int i = arg_list->child_count - 1; i >= 0; i--) {
          // Allocate register for each argument
          VMRegister arg_reg = allocate_register(&ctx->reg_allocator);
          generate_simple_expression(ctx, arg_list->children[i], arg_reg);
          // Push the argument onto stack
          add_instruction(ctx->program, VM_PUSH, vm_create_register_operand(arg_reg), vm_create_operand());
          // Free the argument register
          free_register(&ctx->reg_allocator, arg_reg);
        }
      }
    }

    if (op->op1.kind == OPND_LABEL) {
      add_instruction(ctx->program, VM_CALL,
                      vm_create_label_operand(op->op1.value.name),
                      vm_create_operand());
    }

    // Clean up stack from pushed arguments (stdcall - callee cleans up)
    // For our implementation, we'll use caller-cleanup (cdecl) for simplicity
    if (op->ast_node && op->ast_node->child_count >= 2) {
      ASTNode *arg_list = op->ast_node->children[1];
      if (strcmp(arg_list->type, "exprList") == 0) {
        int arg_count = arg_list->child_count;
        if (arg_count > 0) {
          add_instruction(ctx->program, VM_ADD, vm_create_register_operand(SP),
                          vm_create_immediate_operand(arg_count * 4));
        }
      }
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
      if (op->op1.kind == OPND_CONST || op->op1.kind == OPND_STRING_LITERAL) {
        // Direct condition without comparison: while (1), while (0), while ("abc")
        // Value already loaded in left_reg by load_operand_to_reg()
        add_instruction(ctx->program, VM_CMP,
                        vm_create_register_operand(left_reg),
                        vm_create_immediate_operand(0));

        // Runtime check: if value != 0 then true, else false
        add_instruction(ctx->program, VM_JNE,
                        vm_create_label_operand(op->true_target),
                        vm_create_operand());
        add_instruction(ctx->program, VM_JMP,
                        vm_create_label_operand(op->false_target),
                        vm_create_operand());
      } else {
        // Normal comparison from OP_LT/OP_GT/OP_EQ - already done
        // Comparison result is in flags, just need to jump based on cmp_type

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
  case OP_RETURN: {
    // Function return - load return value into R0 and jump to exit block
    if (op->op1.kind != OPND_UNDEF) {
      // Load operand into register
      VMRegister ret_reg = load_operand_to_reg(ctx, &op->op1);
      // Move to R0 (standard calling convention)
      if (ret_reg != R0) {
        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(R0),
                        vm_create_register_operand(ret_reg));
      }
      // Free register if it was allocated for const/string literal
      if (op->op1.kind == OPND_CONST || op->op1.kind == OPND_STRING_LITERAL) {
        free_register(&ctx->reg_allocator, ret_reg);
      }
    }

    // Jump to function exit block where epilogue will be generated
    if (ctx->function->exit_block) {
      char exit_label[128];
      snprintf(exit_label, sizeof(exit_label), "%s_%s",
               ctx->function->name, ctx->function->exit_block->label);
      sanitize_label_public(exit_label);
      add_instruction(ctx->program, VM_JMP,
                      vm_create_label_operand(exit_label), vm_create_operand());
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

  // Free temporarily allocated registers for constants and string literals
  // Don't free for operations that handle their own register cleanup (STORE, RETURN)
  if (freed_left_const && !result_stored && op->dest.kind != OPND_CONST &&
      op->type != OP_STORE && op->type != OP_RETURN) {
    free_register(&ctx->reg_allocator, left_reg);
  }
  if (freed_right_const && op->type != OP_STORE && op->type != OP_RETURN) {
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
