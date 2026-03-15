#ifndef OPERATION_GEN_H
#define OPERATION_GEN_H

#include "../../../2/cfg.h"
#include "function_context.h"

// Generate code for a single CFG operation (OP_ADD, OP_STORE, OP_CALL, etc.)
void generate_operation(FunctionContext *ctx, Operation *op);

#endif
