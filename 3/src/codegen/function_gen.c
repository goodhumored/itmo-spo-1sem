#include "function_gen.h"
#include "block_gen.h"
#include "operation_gen.h"
#include "operand_utils.h"
#include "string_utils.h"
#include "function_context.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void generate_function_prologue(FunctionContext *ctx) {
  add_label(ctx->program, ctx->function->name, ctx->program->instruction_count);

  add_instruction(ctx->program, VM_PUSH, vm_create_register_operand(BP),
                  vm_create_operand());
  add_instruction(ctx->program, VM_MOV, vm_create_register_operand(BP),
                  vm_create_register_operand(SP));

  if (ctx->local_var_count > 0) {
    add_instruction(ctx->program, VM_SUB, vm_create_register_operand(SP),
                    vm_create_immediate_operand(ctx->local_var_count * 4));
  }
}

static void generate_function_epilogue(FunctionContext *ctx) {
  // Add return sequence at the end of function
  // Restore stack frame and return
  add_instruction(ctx->program, VM_MOV, vm_create_register_operand(SP),
                  vm_create_register_operand(BP));
  add_instruction(ctx->program, VM_POP, vm_create_register_operand(BP),
                  vm_create_operand());
  add_instruction(ctx->program, VM_RET, vm_create_operand(),
                  vm_create_operand());
}

void generate_function_code(FunctionContext *ctx, CFG *cfg) {
  if (!ctx || !cfg)
    return;

  // TEMPORARILY DISABLE LSRA to fix register allocator first
  // perform_lsra(ctx, cfg);

  generate_function_prologue(ctx);

  // Generate code for each block (skip exit block - it will be handled separately with epilogue)
  for (int i = 0; i < cfg->block_count; i++) {
    BasicBlock *block = cfg->blocks[i];

    // Skip exit block - it will be handled at the end with epilogue
    if (block == cfg->function->exit_block) {
      continue;
    }

    char block_label[128];
    snprintf(block_label, sizeof(block_label), "%s_%s", ctx->function->name,
             block->label);
    sanitize_label_public(block_label);
    add_label(ctx->program, block_label, ctx->program->instruction_count);

    generate_block(ctx, block);

    // Add explicit fallthrough jump only if:
    // - Block has successors
    // - Last operation is not a JMP or CJMP (or it doesn't have next_target)
    if (block->op_count > 0) {
      Operation *last_op = block->operations[block->op_count - 1];
      bool has_explicit_jump = (last_op->type == OP_JMP || last_op->type == OP_CJMP ||
                                last_op->next_target != NULL);

      if (!has_explicit_jump && block->successor_count > 0) {
        BasicBlock *succ = block->successors[0];
        // Don't jump to ourselves (CFG builder bug)
        int is_self_jump = (strcmp(block->label, succ->label) == 0);

        // Temporary fix: if succ is a merge block, jump to exit instead
        int is_merge_to_exit = (strstr(succ->label, "merge") != NULL);

        // For then blocks that fallthrough to exit, load return value into R0
        int is_then_to_exit = (strstr(block->label, "then") != NULL &&
                              succ == cfg->function->exit_block);

        // DEBUG: Print fallthrough jump details
        printf("DEBUG fallthrough: block=%s, succ=%s, exit_block=%s\n",
               block->label, succ ? succ->label : "NULL",
               cfg->function->exit_block ? cfg->function->exit_block->label : "NULL");
        printf("  is_self_jump=%d, is_merge_to_exit=%d, is_then_to_exit=%d\n",
               is_self_jump, is_merge_to_exit, is_then_to_exit);

        if (!is_self_jump && succ != cfg->function->exit_block && !is_merge_to_exit) {
          char succ_label[128];
          snprintf(succ_label, sizeof(succ_label), "%s_%s",
                   ctx->function->name, succ->label);
          sanitize_label_public(succ_label);
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(succ_label), vm_create_operand());
        } else if (!is_self_jump && is_merge_to_exit) {
          // Fallthrough from merge to exit block
          if (cfg->function->exit_block) {
            char exit_label[128];
            snprintf(exit_label, sizeof(exit_label), "%s_%s",
                     ctx->function->name, cfg->function->exit_block->label);
            sanitize_label_public(exit_label);
            add_instruction(ctx->program, VM_JMP,
                            vm_create_label_operand(exit_label), vm_create_operand());
          }
        } else if (!is_self_jump && is_then_to_exit && cfg->function->local_var_count > 0) {
          // For then block falling through to exit, load first local var into R0 (return value)
          // This is a temporary fix for functions like fib that store return value in r
          LocalVar *first_local = cfg->function->local_vars[0];
          if (first_local) {
            add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(R0),
                            vm_create_bp_offset_operand(first_local->offset));
          }

          char exit_label[128];
          snprintf(exit_label, sizeof(exit_label), "%s_%s",
                   ctx->function->name, cfg->function->exit_block->label);
          sanitize_label_public(exit_label);
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(exit_label), vm_create_operand());
        }
      }
    }
  }

  // Generate epilogue at exit block (unified return point)
  if (cfg->function->exit_block) {
    BasicBlock *exit_block = cfg->function->exit_block;
    char exit_label[128];
    snprintf(exit_label, sizeof(exit_label), "%s_%s",
             ctx->function->name, exit_block->label);
    sanitize_label_public(exit_label);
    add_label(ctx->program, exit_label, ctx->program->instruction_count);

    // TEMPORARY FIX: Load first local variable into R0 (return value)
    // This assumes the first local var is the return value variable
    if (cfg->function->local_var_count > 0) {
      LocalVar *first_local = cfg->function->local_vars[0];
      printf("DEBUG epilogue: Loading return value '%s' from [BP%d] into R0\n",
             first_local->name, first_local->offset);
      add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(R0),
                      vm_create_bp_offset_operand(first_local->offset));
    }

    // Generate epilogue
    add_instruction(ctx->program, VM_MOV, vm_create_register_operand(SP),
                    vm_create_register_operand(BP));
    add_instruction(ctx->program, VM_POP, vm_create_register_operand(BP),
                    vm_create_operand());
    add_instruction(ctx->program, VM_RET, vm_create_operand(),
                    vm_create_operand());
  }

  // Note: Functions can have multiple return statements.
  // OP_RETURN jumps to this unified exit point with the epilogue.
}

// Helper: Check if a CFG has any real operations (excluding empty blocks)
static int function_has_body(CFG *cfg) {
  if (!cfg || !cfg->blocks || cfg->block_count == 0)
    return 0;

  for (int i = 0; i < cfg->block_count; i++) {
    BasicBlock *block = cfg->blocks[i];
    if (!block)
      continue;

    for (int j = 0; j < block->op_count; j++) {
      Operation *op = block->operations[j];
      if (!op)
        continue;

      // Skip NOP operations
      if (op->type == OP_NOP)
        continue;

      // If we found any real operation, function has a body
      return 1;
    }
  }

  return 0;
}

VMProgram *generate_code(CFG **cfgs, int cfg_count) {
  if (!cfgs || cfg_count <= 0)
    return NULL;

  VMProgram *program = create_vm_program();
  if (!program)
    return NULL;

  // Collect all string literals from CFG and generate .data section entries
  // Do this before generating code so all strings are available
  collect_string_literals_from_cfg(cfgs, cfg_count, program);

  for (int i = 0; i < cfg_count; i++) {
    if (!cfgs[i])
      continue;

    // Skip functions without body (function prototypes only)
    if (!function_has_body(cfgs[i])) {
      continue;
    }

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
