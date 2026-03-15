#include "./string_utils.h"
#include "../../../2/cfg.h"
#include "../asm_types.h"
#include "../codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_CACHE 256

// Helper function to process escape sequences in a string literal
static
void process_escape_sequences(char *str, int *len) {
  int i = 0, j = 0;
  while (i < *len) {
    if (str[i] == '\\' && i + 1 < *len) {
      switch (str[i + 1]) {
      case 'n':
        str[j++] = '\n';
        i += 2;
        break;
      case 'r':
        str[j++] = '\r';
        i += 2;
        break;
      case 't':
        str[j++] = '\t';
        i += 2;
        break;
      case '\\':
        str[j++] = '\\';
        i += 2;
        break;
      case '"':
        str[j++] = '"';
        i += 2;
        break;
      case '\'':
        str[j++] = '\'';
        i += 2;
        break;
      default:
        str[j++] = str[i + 1];
        i += 2;
        break;
      }
    } else {
      str[j++] = str[i++];
    }
  }
  *len = j;
}

// Helper to extract string content from literal (including quotes)
// Returns NULL if not a string literal, otherwise returns malloc'ed string
// static
char *extract_string_from_literal(const char *value) {
  if (!value || strlen(value) < 2) {
    return NULL;
  }

  if (value[0] != '"' || value[strlen(value) - 1] != '"') {
    return NULL;
  }

  int len = strlen(value);
  char *str_data = malloc(len);
  memcpy(str_data, value + 1, len - 2);
  str_data[len - 2] = '\0';

  // Process escape sequences
  process_escape_sequences(str_data, &len);

  return str_data;
}

// Helper structure for string collection context
typedef struct {
  VMProgram *program;
  int string_count;
  char *string_cache[MAX_STRING_CACHE];
  int cache_index;
} StringCollectionContext;

// String-to-label mapping
#define MAX_STRING_LABELS 128
typedef struct {
  char *literal_value;
  char *label;
} StringLabelMapping;

static StringLabelMapping string_mappings[MAX_STRING_LABELS];
static int mapping_count = 0;

// Check if string is already in cache
static int is_string_cached(StringCollectionContext *ctx, const char *str) {
  for (int i = 0; i < ctx->cache_index; i++) {
    if (ctx->string_cache[i] && strcmp(ctx->string_cache[i], str) == 0) {
      return 1;
    }
  }
  return 0;
}

// Add string to cache
static void add_string_to_cache(StringCollectionContext *ctx, char *str) {
  if (ctx->cache_index < MAX_STRING_CACHE) {
    ctx->string_cache[ctx->cache_index++] = str;
  }
}

// Recursive helper to collect string literals from AST node and its children
// Also collect string literals directly from OPND_STRING_LITERAL operands
static void collect_strings_from_ast_or_operand(StringCollectionContext *ctx, ASTNode *node, Operand *op) {
  // Check operand for string literal
  if (op && op->kind == OPND_STRING_LITERAL && op->value.name) {
    char *str_data = extract_string_from_literal(op->value.name);
    if (str_data) {
      // Check if we've already processed this string
      if (!is_string_cached(ctx, str_data)) {
        // Generate .data entry
        char str_label[128];
        snprintf(str_label, sizeof(str_label), "str_%d", ctx->string_count++);
        add_data_bytes(ctx->program, str_label, str_data, strlen(str_data) + 1);
        
        // Add to mapping (use the original literal value as key)
        if (mapping_count < MAX_STRING_LABELS) {
          string_mappings[mapping_count].literal_value = strdup(op->value.name);
          string_mappings[mapping_count++].label = strdup(str_label);
        }
        
        add_string_to_cache(ctx, str_data);
      } else {
        free(str_data);
      }
    }
  }

  if (!node)
    return;

  // Check if this node is a string literal
  if (strcmp(node->type, "literal") == 0 && node->value) {
    char *str_data = extract_string_from_literal(node->value);
    if (str_data) {
      // Check if we've already processed this string
      if (!is_string_cached(ctx, str_data)) {
        // Generate .data entry
        char str_label[128];
        snprintf(str_label, sizeof(str_label), "str_%d", ctx->string_count++);
        add_data_bytes(ctx->program, str_label, str_data, strlen(str_data) + 1);
        
        // Add to mapping
        if (mapping_count < MAX_STRING_LABELS) {
          string_mappings[mapping_count].literal_value = strdup(node->value);
          string_mappings[mapping_count++].label = strdup(str_label);
        }
        
        add_string_to_cache(ctx, str_data);
      } else {
        free(str_data);
      }
    }
  }

  // Recursively process all children
  for (int i = 0; i < node->child_count; i++) {
    if (node->children[i]) {
      collect_strings_from_ast_or_operand(ctx, node->children[i], NULL);
    }
  }
}

void collect_string_literals_from_cfg(CFG **cfgs, int cfg_count,
                                      VMProgram *program) {
  StringCollectionContext ctx;
  ctx.program = program;
  ctx.string_count = 0;
  ctx.cache_index = 0;

  for (int i = 0; i < MAX_STRING_CACHE; i++) {
    ctx.string_cache[i] = NULL;
  }

  for (int f = 0; f < cfg_count; f++) {
    if (!cfgs[f])
      continue;

    CFG *cfg = cfgs[f];

    for (int b = 0; b < cfg->block_count; b++) {
      BasicBlock *block = cfg->blocks[b];
      if (!block)
        continue;

      for (int o = 0; o < block->op_count; o++) {
        Operation *op = block->operations[o];
        if (!op)
          continue;

        // Check if this operation has an AST node
        if (op->ast_node) {
          // Collect strings from the entire AST subtree
          collect_strings_from_ast_or_operand(&ctx, op->ast_node, NULL);
        }
        
        // Check operands for string literals
        collect_strings_from_ast_or_operand(&ctx, NULL, &op->op1);
        collect_strings_from_ast_or_operand(&ctx, NULL, &op->op2);
        collect_strings_from_ast_or_operand(&ctx, NULL, &op->dest);
      }
    }
  }

  // Clean up string cache
  for (int i = 0; i < ctx.cache_index; i++) {
    if (ctx.string_cache[i]) {
      free(ctx.string_cache[i]);
      ctx.string_cache[i] = NULL;
    }
  }
}

const char* get_string_label(const char *literal_value) {
  if (!literal_value)
    return NULL;
  
  for (int i = 0; i < mapping_count; i++) {
    if (string_mappings[i].literal_value && 
        strcmp(string_mappings[i].literal_value, literal_value) == 0) {
      return string_mappings[i].label;
    }
  }
  
  return NULL;
}

void cleanup_string_literals() {
  for (int i = 0; i < mapping_count; i++) {
    free(string_mappings[i].literal_value);
    free(string_mappings[i].label);
    string_mappings[i].literal_value = NULL;
    string_mappings[i].label = NULL;
  }
  mapping_count = 0;
}
