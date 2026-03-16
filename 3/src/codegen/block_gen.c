#include "block_gen.h"
#include "operation_gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void generate_block(FunctionContext *ctx, CFG *cfg, BasicBlock *block) {
  printf("%s, %d ops, %d successors\n", block->label, block->op_count, block->successor_count);
  if (!block || (block->op_count == 0 && block->successor_count == 0)) {
    printf("skip\n");
    return;
  }

  // Generate code for each operation in the block
  for (int i = 0; i < block->op_count; i++) {
    Operation *op = block->operations[i];
    generate_operation(ctx, op);
  }

  // Generate block termination (successors and fallthrough)
  generate_block_termination(ctx, block);
}

void generate_block_termination(FunctionContext *ctx, BasicBlock *block) {
  // For fallthrough blocks, generate jump to successor
  if (block->successor_count > 0) {
    BasicBlock *succ = block->successors[0];
    printf("%s -> %s\n", block->label, succ->label);

    // Skip self-jumps (CFG artifact)
    if (strcmp(block->label, succ->label) == 0) {
      printf("DEBUG: Skipping self-jump from %s to %s\n", block->label,
             succ->label);
      return;
    }

    char succ_label[128];
    snprintf(succ_label, sizeof(succ_label), "%s_%s", ctx->function->name,
             succ->label);
    sanitize_label_public(succ_label);
    add_instruction(ctx->program, VM_JMP, vm_create_label_operand(succ_label),
                    vm_create_operand());

    printf("DEBUG: Generated fallthrough jump from %s to %s\n", block->label,
           succ->label);
  }
}

// Expose sanitize_label for use in function_gen.c
void sanitize_label_public(char *label) { sanitize_label(label); }
