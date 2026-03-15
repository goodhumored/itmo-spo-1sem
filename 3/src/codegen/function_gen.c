#include "function_gen.h"
#include "block_gen.h"
#include "operation_gen.h"
#include "operand_utils.h"
#include "string_utils.h"
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

  generate_function_prologue(ctx);

  // Generate code for each block
  for (int i = 0; i < cfg->block_count; i++) {
    BasicBlock *block = cfg->blocks[i];

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
        if (succ != cfg->function->exit_block) {
          char succ_label[128];
          snprintf(succ_label, sizeof(succ_label), "%s_%s",
                   ctx->function->name, succ->label);
          sanitize_label_public(succ_label);
          add_instruction(ctx->program, VM_JMP,
                          vm_create_label_operand(succ_label), vm_create_operand());
        }
      }
    }
  }

  // Note: We don't add epilogue here because functions can have multiple
  // return statements. The OP_RETURN operation in CFG handles epilogue.
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
