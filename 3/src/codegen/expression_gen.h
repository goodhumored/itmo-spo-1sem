#ifndef EXPRESSION_GEN_H
#define EXPRESSION_GEN_H

#include "function_context.h"
#include "../../../1/ast.h"

void generate_expression(FunctionContext *ctx, ASTNode *expr, VMRegister target_reg);

#endif
