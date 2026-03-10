#include "function_gen.h"
#include "expression_gen.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Helper: Load IR operand into a VM register
static VMRegister load_ir_operand(FunctionContext *ctx, Operand *ir_op) {
  if (!ir_op) return R0;

  switch (ir_op->kind) {
    case OPND_CONST:
      // Load constant into a new register
      return allocate_register(&ctx->reg_allocator);
    case OPND_VAR:
      // Get register for variable
      return get_variable_register(ctx, ir_op->value.name);
    case OPND_TEMP:
      // Get register for temporary
      return get_temp_register(ctx, ir_op->value.temp_id);
    case OPND_LABEL:
    case OPND_UNDEF:
      return R0;
  }
  return R0;
}

// Helper: Generate code to initialize an operand (load value into register)
static void init_operand(FunctionContext *ctx, Operand *ir_op, VMRegister target_reg) {
  if (!ir_op) return;

  switch (ir_op->kind) {
    case OPND_CONST:
      add_instruction(ctx->program, VM_MOV,
                      vm_create_register_operand(target_reg),
                      vm_create_immediate_operand((int32_t)ir_op->value.const_val));
      break;
    case OPND_VAR: {
      int64_t var_offset = get_variable_stack_offset(ctx, ir_op->value.name);
      if (var_offset >= 0) {
        add_instruction(ctx->program, VM_LOAD,
                        vm_create_register_operand(target_reg),
                        vm_create_bp_offset_operand(-var_offset));
      } else {
        VMRegister var_reg = get_variable_register(ctx, ir_op->value.name);
        if (var_reg != target_reg) {
          add_instruction(ctx->program, VM_MOV,
                          vm_create_register_operand(target_reg),
                          vm_create_register_operand(var_reg));
        }
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

static void generate_single_operation(FunctionContext *ctx, Operation *op) {
  if (!op) return;

  VMRegister left_reg = R0;
  VMRegister right_reg = R1;
  VMRegister dest_reg = R2;

  // Load operands
  init_operand(ctx, &op->op1, left_reg);
  init_operand(ctx, &op->op2, right_reg);

  // Generate instruction based on operation type
  switch (op->type) {
    case OP_ADD:
      add_instruction(ctx->program, VM_ADD,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_SUB:
      add_instruction(ctx->program, VM_SUB,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_MUL:
      add_instruction(ctx->program, VM_MUL,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_DIV:
      add_instruction(ctx->program, VM_DIV,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_LT:
      add_instruction(ctx->program, VM_CMP,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_GT:
      add_instruction(ctx->program, VM_CMP,
                      vm_create_register_operand(left_reg),
                      vm_create_register_operand(right_reg));
      break;
    case OP_STORE:
      // op1 is value, op2 is destination (variable)
      if (op->op2.kind == OPND_VAR) {
        int64_t var_offset = get_variable_stack_offset(ctx, op->op2.value.name);
        if (var_offset >= 0) {
          add_instruction(ctx->program, VM_STORE,
                          vm_create_bp_offset_operand(-var_offset),
                          vm_create_register_operand(left_reg));
        } else {
          VMRegister var_reg = get_variable_register(ctx, op->op2.value.name);
          if (left_reg != var_reg) {
            add_instruction(ctx->program, VM_MOV,
                            vm_create_register_operand(var_reg),
                            vm_create_register_operand(left_reg));
          }
        }
      }
      break;
    case OP_CONST:
      // Load constant into destination
      if (op->dest.kind != OPND_UNDEF) {
        VMRegister reg;
        if (op->dest.kind == OPND_TEMP) {
          reg = get_temp_register(ctx, op->dest.value.temp_id);
        } else {
          reg = R0;
        }
        add_instruction(ctx->program, VM_MOV,
                        vm_create_register_operand(reg),
                        vm_create_immediate_operand((int32_t)op->op1.value.const_val));
      }
      break;
    case OP_CALL:
      // Function call
// Загружаем аргументы из AST (если есть)
      if (op->ast_node && op->ast_node->child_count >= 2) {
        ASTNode *arg_list = op->ast_node->children[1];
        if (strcmp(arg_list->type, "exprList") == 0) {
          for (int i = 0; i < arg_list->child_count && i < 5; i++) {
            VMRegister arg_reg = (VMRegister)(R3 + i);
            generate_expression(ctx, arg_list->children[i], arg_reg);
          }
        }
      }
      
      if (op->op1.kind == OPND_LABEL) {
        add_instruction(ctx->program, VM_CALL,
                        vm_create_label_operand(op->op1.value.name),
                        vm_create_operand());
      }
      break;
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
        
        // Для literal conditions (while(1), if(0)):
        // Загружаем константу в регистр, сравниваем с 0
        if (op->op1.kind == OPND_CONST) {
          int64_t const_val = op->op1.value.const_val;
          
          // Загружаем константу в регистр (если еще не загружена)
          if (left_reg != R0) {
            add_instruction(ctx->program, VM_MOV,
                            vm_create_register_operand(R0),
                            vm_create_immediate_operand((int32_t)const_val));
            cmp_reg = R0;
          }
          
          // Сравниваем с 0
          add_instruction(ctx->program, VM_CMP,
                          vm_create_register_operand(cmp_reg),
                          vm_create_immediate_operand(0));
          
          // Определяем тип conditional jump:
          // - Если const_val != 0 (true), прыгаем на true_target если условие верно
          // - Для всех сравнений (LT, GT) уже сделали CMP в OP_LT/OP_GT
          
          // Для literal условий:
          if (const_val != 0) {
            // true: прыгаем на true_target
            // false: прыгаем на false_target (никогда не выполнится для const != 0)
            add_instruction(ctx->program, VM_JNE,
                            vm_create_label_operand(op->true_target),
                            vm_create_operand());
            add_instruction(ctx->program, VM_JMP,
                            vm_create_label_operand(op->false_target),
                            vm_create_operand());
          } else {
            // const_val == 0 (false):
            // Всегда прыгаем на false_target
            add_instruction(ctx->program, VM_JMP,
                            vm_create_label_operand(op->false_target),
                            vm_create_operand());
          }
        } else {
          // Обычное сравнение (a > 5)
          // Операнды уже загружены в left_reg и right_reg
          // Сравнение уже было сделано в OP_LT/OP_GT
          
          // Определяем conditional jump based on cmp_type
          VMInstructionType jump_type = VM_JE;
          
          switch (op->cmp_type) {
            case OP_LT:
              jump_type = VM_JL;  // Jump if Less (true means left < right)
              break;
            case OP_GT:
              jump_type = VM_JG;  // Jump if Greater (true means left > right)
              break;
            // TODO: Add more comparison types
            default:
              jump_type = VM_JE;  // Default to equal
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
  if (op->dest.kind != OPND_UNDEF && op->type != OP_STORE && op->type != OP_CONST) {
    if (op->dest.kind == OPND_VAR) {
      VMRegister var_reg = get_variable_register(ctx, op->dest.value.name);
      if (left_reg != var_reg) {
        add_instruction(ctx->program, VM_MOV,
                        vm_create_register_operand(var_reg),
                        vm_create_register_operand(left_reg));
      }
    } else if (op->dest.kind == OPND_TEMP) {
      // Result already in left_reg (or we need to move it)
      // For now, keep it in the register
    }
  }
}

static void sanitize_label(char *label) {
  for (int i = 0; label[i]; i++) {
    if (label[i] == '<' || label[i] == '>' || label[i] == '=' ||
        label[i] == ' ' || label[i] == '"' || label[i] == '\'' ||
        label[i] == '(' || label[i] == ')' || label[i] == ',' ||
        label[i] == '\t' || label[i] == '\n') {
      label[i] = '_';
    }
  }
}

static void generate_conditional_jump_from_comparison(FunctionContext *ctx,
                                                       OperationType cmp_type,
                                                       const char *true_label,
                                                       const char *false_label) {
  if (!true_label || !false_label) return;

  VMInstructionType jump_type;

  // Determine the appropriate conditional jump instruction
  switch (cmp_type) {
    case OP_LT:
      jump_type = VM_JL;
      break;
    case OP_GT:
      jump_type = VM_JG;
      break;
    // TODO: Add more comparison types when implemented
    // case OP_LE:
    // case OP_GE:
    // case OP_EQ:
    // case OP_NE:
    default:
      // For now, default to JE (can be improved later)
      jump_type = VM_JE;
      break;
  }

  add_instruction(ctx->program, jump_type,
                  vm_create_label_operand(true_label),
                  vm_create_operand());
  add_instruction(ctx->program, VM_JMP,
                  vm_create_label_operand(false_label),
                  vm_create_operand());
}

static void generate_conditional_jump(FunctionContext *ctx, const char *op,
                                       const char *true_label, 
                                       const char *false_label) {
  if (strcmp(op, ">") == 0) {
    add_instruction(ctx->program, VM_JG,
                    vm_create_label_operand(true_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(false_label), vm_create_operand());
  } else if (strcmp(op, "<") == 0) {
    add_instruction(ctx->program, VM_JL,
                    vm_create_label_operand(true_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(false_label), vm_create_operand());
  } else if (strcmp(op, "==") == 0) {
    add_instruction(ctx->program, VM_JE,
                    vm_create_label_operand(true_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(false_label), vm_create_operand());
  } else if (strcmp(op, "!=") == 0) {
    add_instruction(ctx->program, VM_JNE,
                    vm_create_label_operand(true_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(false_label), vm_create_operand());
  } else if (strcmp(op, ">=") == 0) {
    add_instruction(ctx->program, VM_JL,
                    vm_create_label_operand(false_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(true_label), vm_create_operand());
  } else if (strcmp(op, "<=") == 0) {
    add_instruction(ctx->program, VM_JG,
                    vm_create_label_operand(false_label), vm_create_operand());
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(true_label), vm_create_operand());
  } else {
    add_instruction(ctx->program, VM_JMP,
                    vm_create_label_operand(false_label), vm_create_operand());
  }
}

static void generate_block_operation(FunctionContext *ctx, BasicBlock *block) {
  if (!block->operations || block->op_count == 0) return;
  
  // Generate code for each operation in the block
  for (int i = 0; i < block->op_count; i++) {
    Operation *op = block->operations[i];
    generate_single_operation(ctx, op);
  }
}

void generate_function_code(FunctionContext *ctx, CFG *cfg) {
  if (!ctx || !cfg)
    return;

  add_label(ctx->program, ctx->function->name, ctx->program->instruction_count);

  add_instruction(ctx->program, VM_PUSH, vm_create_register_operand(BP),
                  vm_create_operand());
  add_instruction(ctx->program, VM_MOV, vm_create_register_operand(BP),
                  vm_create_register_operand(SP));

  if (ctx->local_var_count > 0) {
    add_instruction(ctx->program, VM_SUB, vm_create_register_operand(SP),
                    vm_create_immediate_operand(ctx->local_var_count * 4));
  }

  // Generate code for each operation
  // Note: Control flow (jumps) is handled by Operations themselves
  // We still iterate through blocks and generate their operations

  for (int i = 0; i < cfg->block_count; i++) {
    BasicBlock *block = cfg->blocks[i];

    char block_label[128];
    snprintf(block_label, sizeof(block_label), "%s_%s", ctx->function->name,
             block->label);
    sanitize_label(block_label);
    add_label(ctx->program, block_label, ctx->program->instruction_count);

    generate_block_operation(ctx, block);

    // Add explicit fallthrough jump only if:
    // - Block has successors
    // - Last operation is not a JMP or CJMP (or it doesn't have next_target)
    if (block->op_count > 0) {
      Operation *last_op = block->operations[block->op_count - 1];
      bool has_explicit_jump = (last_op->type == OP_JMP || last_op->type == OP_CJMP ||
                                last_op->next_target != NULL);

      if (!has_explicit_jump && block->successor_count > 0) {
        BasicBlock *succ = block->successors[0];
        if (succ != cfg->function->exit_block) {
          char succ_label[128];
          snprintf(succ_label, sizeof(succ_label), "%s_%s",
                   ctx->function->name, succ->label);
          sanitize_label(succ_label);
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(succ_label), vm_create_operand());
        }
      }
    }
  }
}

VMProgram *generate_code(CFG **cfgs, int cfg_count) {
  if (!cfgs || cfg_count <= 0)
    return NULL;

  VMProgram *program = create_vm_program();
  if (!program)
    return NULL;

  for (int i = 0; i < cfg_count; i++) {
    if (!cfgs[i])
      continue;

    FunctionContext *ctx = create_function_context(program, cfgs[i]->function);
    if (!ctx) {
      free_vm_program(program);
      return NULL;
    }

    generate_function_code(ctx, cfgs[i]);

    free_function_context(ctx);
  }

  return program;
}
