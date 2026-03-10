#include "function_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FunctionContext *create_function_context(VMProgram *program,
                                         Function *function) {
  FunctionContext *ctx = malloc(sizeof(FunctionContext));
  if (!ctx) {
    fprintf(stderr, "Error: Failed to allocate FunctionContext\n");
    return NULL;
  }

  ctx->program = program;
  ctx->function = function;
  ctx->stack_offset = 0;
  ctx->current_address = 0;
  ctx->next_temp_id = 0;
  ctx->string_idx = 0;
  free_all_registers(&ctx->reg_allocator);

  ctx->local_var_count = function->local_var_count;
  if (ctx->local_var_count > 0) {
    ctx->local_vars = malloc(ctx->local_var_count * sizeof(LocalVarMapping));
    for (int i = 0; i < ctx->local_var_count; i++) {
      ctx->local_vars[i].name = strdup(function->local_vars[i]->name);
      ctx->local_vars[i].offset = function->local_vars[i]->offset;
      ctx->local_vars[i].reg = R0;
      ctx->local_vars[i].in_register = false;
    }
  } else {
    ctx->local_vars = NULL;
  }

  ctx->arg_count = function->arg_count;
  if (ctx->arg_count > 0) {
    ctx->args = malloc(ctx->arg_count * sizeof(ArgumentMapping));
    for (int i = 0; i < ctx->arg_count; i++) {
      ctx->args[i].name = strdup(function->args[i]->name);
      ctx->args[i].index = function->args[i]->index;
      ctx->args[i].reg = R0;
      ctx->args[i].in_register = false;
    }
  } else {
    ctx->args = NULL;
  }

  return ctx;
}

void free_function_context(FunctionContext *ctx) {
  if (!ctx)
    return;

  for (int i = 0; i < ctx->local_var_count; i++) {
    free(ctx->local_vars[i].name);
  }
  free(ctx->local_vars);

  for (int i = 0; i < ctx->arg_count; i++) {
    free(ctx->args[i].name);
  }
  free(ctx->args);

  free(ctx);
}

VMRegister get_variable_register(FunctionContext *ctx, const char *var_name) {

  for (int i = 0; i < ctx->local_var_count; i++) {
    if (strcmp(ctx->local_vars[i].name, var_name) == 0) {
      if (!ctx->local_vars[i].in_register) {

        ctx->local_vars[i].reg = allocate_register(&ctx->reg_allocator);
        ctx->local_vars[i].in_register = true;
      }
      return ctx->local_vars[i].reg;
    }
  }

  for (int i = 0; i < ctx->arg_count; i++) {
    if (strcmp(ctx->args[i].name, var_name) == 0) {
      if (!ctx->args[i].in_register) {

        ctx->args[i].reg = allocate_register(&ctx->reg_allocator);
        ctx->args[i].in_register = true;
      }
      return ctx->args[i].reg;
    }
  }

  return R0;
}

int64_t get_variable_stack_offset(FunctionContext *ctx, const char *var_name) {
  for (int i = 0; i < ctx->local_var_count; i++) {
    if (strcmp(ctx->local_vars[i].name, var_name) == 0) {
      return (int64_t)ctx->local_vars[i].offset;
    }
  }
  return -1;
}

VMRegister get_temp_register(FunctionContext *ctx, int temp_id) {
  return allocate_register(&ctx->reg_allocator);
}

void free_temp_register(FunctionContext *ctx, int temp_id) {}
