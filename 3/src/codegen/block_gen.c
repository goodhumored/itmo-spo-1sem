#include "block_gen.h"
#include "operation_gen.h"

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

void generate_block(FunctionContext *ctx, BasicBlock *block) {
  if (!block->operations || block->op_count == 0)
    return;

  // Generate code for each operation in the block
  for (int i = 0; i < block->op_count; i++) {
    Operation *op = block->operations[i];
    generate_operation(ctx, op);
  }
}

// Expose sanitize_label for use in function_gen.c
void sanitize_label_public(char *label) { sanitize_label(label); }
