#include "function_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEMPS 256

static uint32_t next_ram_address = 0x1000;

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

  // Initialize LSRA structures
  ctx->var_map = create_variable_map();
  ctx->lsra_result = NULL;

  ctx->local_var_count = function->local_var_count;
  if (ctx->local_var_count > 0) {
    ctx->local_vars = malloc(ctx->local_var_count * sizeof(LocalVarMapping));
    for (int i = 0; i < ctx->local_var_count; i++) {
      ctx->local_vars[i].name = strdup(function->local_vars[i]->name);
      ctx->local_vars[i].offset = function->local_vars[i]->offset;
      ctx->local_vars[i].reg = R0;
      ctx->local_vars[i].in_register = false;
      ctx->local_vars[i].is_initialized = false;
      ctx->local_vars[i].ram_address = 0;

      // Register local variable in var_map
      get_variable_id(ctx->var_map, function->local_vars[i]->name);
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

      // Register argument in var_map
      get_variable_id(ctx->var_map, function->args[i]->name);
    }
  } else {
    ctx->args = NULL;
  }

  // Initialize temporary variable mapping
  ctx->temp_count = 0;
  ctx->max_temps = MAX_TEMPS;
  ctx->temps = malloc(ctx->max_temps * sizeof(TempMapping));
  for (int i = 0; i < ctx->max_temps; i++) {
    ctx->temps[i].temp_id = -1;
    ctx->temps[i].reg = R0;
    ctx->temps[i].stack_offset = 0;
    ctx->temps[i].in_reg = false;
  }

  // Initialize register cache
  ctx->reg_cache_count = 0;
  for (int i = 0; i < MAX_REGISTER_CACHE; i++) {
    ctx->reg_cache[i].var_name = NULL;
    ctx->reg_cache[i].reg = R0;
    ctx->reg_cache[i].has_reg = false;
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

  free(ctx->temps);

  // Free LSRA structures
  if (ctx->var_map) {
    free_variable_map(ctx->var_map);
  }
  if (ctx->lsra_result) {
    lsra_free_result(ctx->lsra_result);
  }

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

void mark_variable_initialized(FunctionContext *ctx, const char *var_name) {
    if (!ctx || !var_name) return;

    for (int i = 0; i < ctx->local_var_count; i++) {
        if (strcmp(ctx->local_vars[i].name, var_name) == 0) {
            ctx->local_vars[i].is_initialized = true;
            return;
        }
    }
}

int64_t get_variable_stack_offset(FunctionContext *ctx, const char *var_name) {
  // First check local variables
  for (int i = 0; i < ctx->local_var_count; i++) {
    if (strcmp(ctx->local_vars[i].name, var_name) == 0) {
      return (int64_t)ctx->local_vars[i].offset;
    }
  }

  // Then check arguments
  for (int i = 0; i < ctx->arg_count; i++) {
    if (strcmp(ctx->args[i].name, var_name) == 0) {
      // Arguments are at positive offsets from BP
      // First argument is at BP+8 (after return address at BP+4 and old BP at BP)
      return (int64_t)(8 + ctx->args[i].index * 4);
    }
  }

  return -1;
}

uint32_t get_variable_ram_address(FunctionContext *ctx, const char *var_name) {
  for (int i = 0; i < ctx->local_var_count; i++) {
    if (strcmp(ctx->local_vars[i].name, var_name) == 0) {
      return ctx->local_vars[i].ram_address;
    }
  }
  return 0;
}

VMRegister get_temp_register(FunctionContext *ctx, int temp_id) {
  // Check if this temp already has a register
  for (int i = 0; i < ctx->temp_count; i++) {
    if (ctx->temps[i].temp_id == temp_id && ctx->temps[i].in_reg) {
      return ctx->temps[i].reg;
    }
  }

  // Need to allocate a new register for this temp
  VMRegister reg = allocate_register(&ctx->reg_allocator);

  // Add to temp mapping array
  if (ctx->temp_count >= ctx->max_temps) {
    // Expand temp mapping array
    ctx->max_temps *= 2;
    ctx->temps = realloc(ctx->temps, ctx->max_temps * sizeof(TempMapping));
    for (int i = ctx->temp_count; i < ctx->max_temps; i++) {
      ctx->temps[i].temp_id = -1;
      ctx->temps[i].reg = R0;
      ctx->temps[i].stack_offset = 0;
      ctx->temps[i].in_reg = false;
    }
  }

  ctx->temps[ctx->temp_count].temp_id = temp_id;
  ctx->temps[ctx->temp_count].reg = reg;
  ctx->temps[ctx->temp_count].in_reg = true;
  ctx->temp_count++;

  return reg;
}

void free_temp_register(FunctionContext *ctx, int temp_id) {
  for (int i = 0; i < ctx->temp_count; i++) {
    if (ctx->temps[i].temp_id == temp_id && ctx->temps[i].in_reg) {
      free_register(&ctx->reg_allocator, ctx->temps[i].reg);
      ctx->temps[i].in_reg = false;
      return;
    }
  }
}

void set_temp_register(FunctionContext *ctx, int temp_id, VMRegister reg) {
  // Check if this temp already exists
  for (int i = 0; i < ctx->temp_count; i++) {
    if (ctx->temps[i].temp_id == temp_id) {
      ctx->temps[i].reg = reg;
      ctx->temps[i].in_reg = true;
      return;
    }
  }

  // Need to add a new temp entry
  if (ctx->temp_count >= ctx->max_temps) {
    // Expand temp mapping array
    ctx->max_temps *= 2;
    ctx->temps = realloc(ctx->temps, ctx->max_temps * sizeof(TempMapping));
    for (int i = ctx->temp_count; i < ctx->max_temps; i++) {
      ctx->temps[i].temp_id = -1;
      ctx->temps[i].reg = R0;
      ctx->temps[i].stack_offset = 0;
      ctx->temps[i].in_reg = false;
    }
  }

  ctx->temps[ctx->temp_count].temp_id = temp_id;
  ctx->temps[ctx->temp_count].reg = reg;
  ctx->temps[ctx->temp_count].in_reg = true;
  ctx->temp_count++;
}

bool has_temp_register(FunctionContext *ctx, int temp_id) {
  for (int i = 0; i < ctx->temp_count; i++) {
    if (ctx->temps[i].temp_id == temp_id && ctx->temps[i].in_reg) {
      return true;
    }
  }
  return false;
}

VMRegister get_cached_register(FunctionContext *ctx, const char *var_name) {
  if (!var_name)
    return R0;
    
  for (int i = 0; i < ctx->reg_cache_count; i++) {
    if (ctx->reg_cache[i].var_name && 
        strcmp(ctx->reg_cache[i].var_name, var_name) == 0 &&
        ctx->reg_cache[i].has_reg) {
      return ctx->reg_cache[i].reg;
    }
  }
  return R0;  // Not found in cache
}

void set_cached_register(FunctionContext *ctx, const char *var_name, VMRegister reg) {
  if (!var_name)
    return;
    
  // Check if already in cache
  for (int i = 0; i < ctx->reg_cache_count; i++) {
    if (ctx->reg_cache[i].var_name && strcmp(ctx->reg_cache[i].var_name, var_name) == 0) {
      ctx->reg_cache[i].reg = reg;
      ctx->reg_cache[i].has_reg = true;
      return;
    }
  }
  
  // Add new entry
  if (ctx->reg_cache_count < MAX_REGISTER_CACHE) {
    ctx->reg_cache[ctx->reg_cache_count].var_name = strdup(var_name);
    ctx->reg_cache[ctx->reg_cache_count].reg = reg;
    ctx->reg_cache[ctx->reg_cache_count].has_reg = true;
    ctx->reg_cache_count++;
  }
}

void clear_register_cache(FunctionContext *ctx) {
  for (int i = 0; i < ctx->reg_cache_count; i++) {
    if (ctx->reg_cache[i].var_name) {
      free(ctx->reg_cache[i].var_name);
    }
    ctx->reg_cache[i].var_name = NULL;
    ctx->reg_cache[i].reg = R0;
    ctx->reg_cache[i].has_reg = false;
  }
  ctx->reg_cache_count = 0;
}

// LSRA integration functions
void perform_lsra(FunctionContext *ctx, CFG *cfg) {
    if (!ctx || !cfg) return;
    if (ctx->lsra_result) {
        lsra_free_result(ctx->lsra_result);
        ctx->lsra_result = NULL;
    }
    printf("\nLSRA for function: %s\n", cfg->function->name);
    ctx->lsra_result = lsra_allocate(cfg, ctx->var_map, VM_GENERAL_REGISTERS);
}

void set_lsra_result(FunctionContext *ctx, LSRA_Result *result) {
    if (!ctx) return;
    if (ctx->lsra_result) {
        lsra_free_result(ctx->lsra_result);
    }
    ctx->lsra_result = result;
}

VariableMap* get_variable_map(FunctionContext *ctx) {
    if (!ctx) return NULL;
    return ctx->var_map;
}

LSRA_Result* get_lsra_result(FunctionContext *ctx) {
    if (!ctx) return NULL;
    return ctx->lsra_result;
}

int get_ctx_variable_id(FunctionContext *ctx, const char *var_name) {
    if (!ctx || !ctx->var_map || !var_name) return -1;
    return get_variable_id(ctx->var_map, var_name);
}
