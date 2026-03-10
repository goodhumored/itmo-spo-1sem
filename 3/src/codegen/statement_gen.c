#include "statement_gen.h"
#include "expression_gen.h"
#include <stdio.h>
#include <string.h>

static void generate_store(FunctionContext *ctx, const char *var_name,
                          VMRegister value_reg) {
  int64_t var_offset = get_variable_stack_offset(ctx, var_name);
  if (var_offset >= 0) {
    add_instruction(ctx->program, VM_STORE,
                    vm_create_bp_offset_operand(-var_offset),
                    vm_create_register_operand(value_reg));
  } else {
    VMRegister var_reg = get_variable_register(ctx, var_name);
    if (var_reg != value_reg) {
      add_instruction(ctx->program, VM_MOV,
                      vm_create_register_operand(var_reg),
                      vm_create_register_operand(value_reg));
    }
  }
}

void generate_statement(FunctionContext *ctx, ASTNode *stmt) {
  if (!stmt)
    return;

  if (strcmp(stmt->type, AST_BINARY) == 0) {
    if (stmt->value && strcmp(stmt->value, "=") == 0 &&
        stmt->child_count >= 2) {
      if (stmt->children[0]->value) {
        VMRegister temp_reg = allocate_register(&ctx->reg_allocator);
        generate_expression(ctx, stmt->children[1], temp_reg);
        generate_store(ctx, stmt->children[0]->value, temp_reg);
        free_register(&ctx->reg_allocator, temp_reg);
      }
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
        strcmp(expr->value, "=") == 0) {
      if (expr->child_count >= 2 && expr->children[0]->value) {
        VMRegister temp_reg = allocate_register(&ctx->reg_allocator);
        generate_expression(ctx, expr->children[1], temp_reg);
        generate_store(ctx, expr->children[0]->value, temp_reg);
        free_register(&ctx->reg_allocator, temp_reg);
      }
    } else {
      generate_expression(ctx, expr, R0);
    }
  } else if (strcmp(stmt->type, "assign") == 0 && stmt->child_count >= 2) {
    ASTNode *left = stmt->children[0];
    ASTNode *right = stmt->children[1];

    if (left && left->value) {
      VMRegister temp_reg = allocate_register(&ctx->reg_allocator);
      generate_expression(ctx, right, temp_reg);
      generate_store(ctx, left->value, temp_reg);
      free_register(&ctx->reg_allocator, temp_reg);
    }
  }
}
