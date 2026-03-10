#include "expression_gen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void process_escape_sequences(char *str, int *len) {
    int i = 0, j = 0;
    while (i < *len) {
        if (str[i] == '\\' && i + 1 < *len) {
            switch (str[i + 1]) {
                case 'n': str[j++] = '\n'; i += 2; break;
                case 'r': str[j++] = '\r'; i += 2; break;
                case 't': str[j++] = '\t'; i += 2; break;
                case '\\': str[j++] = '\\'; i += 2; break;
                case '\"': str[j++] = '\"'; i += 2; break;
                case '\'': str[j++] = '\''; i += 2; break;
                default: str[j++] = str[i + 1]; i += 2; break;
            }
        } else {
            str[j++] = str[i++];
        }
    }
    *len = j;
}

void generate_expression(FunctionContext *ctx, ASTNode *expr,
                         VMRegister target_reg) {
  if (!expr)
    return;
  printf("type: %s, value: %s, children: %d\n", expr->type, expr->value, expr->child_count);
  if (strcmp(expr->type, "literal") == 0 && expr->value) {
    const char *value = expr->value;

    if (value[0] == '"' && strlen(value) >= 2) {
        char str_label[256];
        snprintf(str_label, sizeof(str_label), "str_%d", ctx->string_idx++);

        int len = strlen(value);
        if (len >= 2) {
            char *str_data = malloc(len);
            memcpy(str_data, value + 1, len - 2);
            str_data[len - 2] = '\0';

            int str_len = len - 2;
            process_escape_sequences(str_data, &str_len);

            add_data_bytes(ctx->program, str_label, str_data, str_len);

            free(str_data);
        }

        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                        vm_create_label_operand(str_label));
    } else {
        int32_t num_value = atoi(value);
        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                        vm_create_immediate_operand(num_value));
    }
  } else if (strcmp(expr->type, "identifier") == 0 && expr->value) {
    int64_t var_offset = get_variable_stack_offset(ctx, expr->value);
    if (var_offset >= 0) {
      add_instruction(ctx->program, VM_LOAD, vm_create_register_operand(target_reg),
                      vm_create_bp_offset_operand(-var_offset));
    } else {
      VMRegister var_reg = get_variable_register(ctx, expr->value);
      if (var_reg != target_reg) {
        add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                        vm_create_register_operand(var_reg));
      }
    }
  } else if (strcmp(expr->type, AST_BINARY) == 0 && expr->child_count >= 2) {

    ASTNode *left = expr->children[0];
    ASTNode *right = expr->children[1];

    VMRegister left_reg = allocate_register(&ctx->reg_allocator);
    VMRegister right_reg = allocate_register(&ctx->reg_allocator);

    generate_expression(ctx, left, left_reg);

    generate_expression(ctx, right, right_reg);

    if (expr->value) {
      if (strcmp(expr->value, "+") == 0) {
        add_instruction(ctx->program, VM_ADD, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, "-") == 0) {
        add_instruction(ctx->program, VM_SUB, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, "*") == 0) {
        add_instruction(ctx->program, VM_MUL, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, "/") == 0) {
        add_instruction(ctx->program, VM_DIV, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, ">") == 0) {
        add_instruction(ctx->program, VM_CMP, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, "<") == 0) {
        add_instruction(ctx->program, VM_CMP, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      } else if (strcmp(expr->value, "==") == 0) {
        add_instruction(ctx->program, VM_CMP, vm_create_register_operand(left_reg),
                        vm_create_register_operand(right_reg));
      }
    }

    if (left_reg != target_reg) {
      add_instruction(ctx->program, VM_MOV, vm_create_register_operand(target_reg),
                      vm_create_register_operand(left_reg));
    }

    free_register(&ctx->reg_allocator, right_reg);
    free_register(&ctx->reg_allocator, left_reg);
  } else if (strcmp(expr->type, "call") == 0) {
    if (expr->child_count >= 1 && expr->children[0] &&
        expr->children[0]->value) {
      if (expr->child_count >= 2 && expr->children[1]) {
        ASTNode *arg_list = expr->children[1];
        if (strcmp(arg_list->type, "exprList") == 0) {
          for (int i = 0; i < arg_list->child_count && i < 5; i++) {
            VMRegister arg_reg = (VMRegister)(R3 + i);
            generate_expression(ctx, arg_list->children[i], arg_reg);
          }
        }
      }

      add_instruction(ctx->program, VM_CALL,
                      vm_create_label_operand(expr->children[0]->value),
                      vm_create_operand());
      if (target_reg != R0) {
        add_instruction(ctx->program, VM_MOV,
                        vm_create_register_operand(target_reg),
                        vm_create_register_operand(R0));
      }
    }
  }
}
