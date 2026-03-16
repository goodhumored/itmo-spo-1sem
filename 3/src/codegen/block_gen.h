#ifndef BLOCK_GEN_H

#define BLOCK_GEN_H

#include "../../../2/cfg.h"
#include "function_context.h"

// Generate code for a single basic block (label + operations)
void generate_block(FunctionContext *ctx, CFG *cfg, BasicBlock *block);

// Generate block termination (successors and fallthrough jumps)
void generate_block_termination(FunctionContext *ctx, BasicBlock *block);

// Utility function for sanitizing labels (removes special characters)
void sanitize_label_public(char *label);

#endif
