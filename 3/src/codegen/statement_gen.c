#include "statement_gen.h"
#include "expression_gen.h"
#include "function_context.h"
#include <stdio.h>
#include <string.h>

static void generate_store(FunctionContext *ctx, const char *var_name,
                           VMRegister value_reg) {
  VMRegister var_reg = get_variable_register(ctx, var_name);
  if (var_reg != value_reg) {
    add_instruction(ctx->program, VM_MOV, vm_create_register_operand(var_reg),
                    vm_create_register_operand(value_reg));
  }
}

void generate_statement(FunctionContext *ctx, ASTNode *stmt) {
  if (!stmt)
    return;

  // Assignment: var = expr
  if (strcmp(stmt->type, AST_BINARY) == 0) {
    if (stmt->value && strcmp(stmt->value, "=") == 0 &&
        stmt->child_count >= 2 && stmt->children[0]->value) {

      // Generate expression into R0, then store to variable's stack location
      generate_expression(ctx, stmt->children[1], R0);

      int64_t offset = get_variable_stack_offset(ctx, stmt->children[0]->value);
      if (offset != 0) {
        add_instruction(ctx->program, VM_STORE,
                        vm_create_bp_offset_operand(offset),
                        vm_create_register_operand(R0));
      }

      mark_variable_initialized(ctx, stmt->children[0]->value);
    } else {
      generate_expression(ctx, stmt, R0);
    }
    return;
  } else if (strcmp(stmt->type, AST_CALL) == 0) {
    generate_expression(ctx, stmt, R0);
    return;
  } else if (strcmp(stmt->type, AST_UNARY) == 0 ||
             strcmp(stmt->type, AST_LITERAL) == 0 ||
             strcmp(stmt->type, AST_IDENTIFIER) == 0) {
    generate_expression(ctx, stmt, R0);
    return;
  }

  if (strcmp(stmt->type, AST_VAR_DECL) == 0) {
    return;
  } else if (strcmp(stmt->type, AST_EXPR_STMT) == 0 && stmt->child_count > 0) {
    ASTNode *expr = stmt->children[0];

    if (strcmp(expr->type, AST_BINARY) == 0 && expr->value &&
        strcmp(expr->value, "=") == 0 &&
        expr->child_count >= 2 && expr->children[0]->value) {

      // Generate expression into R0, then store to variable's stack location
      generate_expression(ctx, expr->children[1], R0);

      int64_t offset = get_variable_stack_offset(ctx, expr->children[0]->value);
      if (offset != 0) {
        add_instruction(ctx->program, VM_STORE,
                        vm_create_bp_offset_operand(offset),
                        vm_create_register_operand(R0));
      }

      mark_variable_initialized(ctx, expr->children[0]->value);
    } else {
      generate_expression(ctx, expr, R0);
    }
  } else if (strcmp(stmt->type, "assign") == 0 && stmt->child_count >= 2) {
    ASTNode *left = stmt->children[0];
    ASTNode *right = stmt->children[1];

    if (left && left->value) {
      generate_expression(ctx, right, R0);

      int64_t offset = get_variable_stack_offset(ctx, left->value);
      if (offset != 0) {
        add_instruction(ctx->program, VM_STORE,
                        vm_create_bp_offset_operand(offset),
                        vm_create_register_operand(R0));
      }

      mark_variable_initialized(ctx, left->value);
    }
  }
}
