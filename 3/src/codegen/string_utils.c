#include "../../../2/cfg.h"
#include "../asm_types.h"
#include "../codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to process escape sequences in a string literal static
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

void collect_string_literals_from_cfg(CFG **cfgs, int cfg_count,
                                      VMProgram *program) {
  int string_count = 0;

// We'll use a simple array to track which string IDs we've already processed
// This is a simple implementation - in a real compiler you'd use a hash table
#define MAX_STRING_CACHE 256
  char *string_cache[MAX_STRING_CACHE];

  for (int i = 0; i < MAX_STRING_CACHE; i++) {
    string_cache[i] = NULL;
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

        // Check if this operation has an AST node with string literals
        if (op->ast_node) {
          // For function calls, check arguments for string literals
          if (op->type == OP_CALL && op->ast_node->child_count >= 2) {
            ASTNode *arg_list = op->ast_node->children[1];
            if (arg_list && strcmp(arg_list->type, "exprList") == 0) {
              for (int a = 0; a < arg_list->child_count; a++) {
                ASTNode *arg = arg_list->children[a];
                if (arg && strcmp(arg->type, "literal") == 0 && arg->value) {
                  char *str_data = extract_string_from_literal(arg->value);
                  if (str_data) {
                    // Generate .data entry
                    char str_label[128];
                    snprintf(str_label, sizeof(str_label), "str_%d",
                             string_count++);
                    add_data_bytes(program, str_label, str_data,
                                   strlen(str_data) + 1);
                    free(str_data);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Clean up string cache
  for (int i = 0; i < MAX_STRING_CACHE; i++) {
    if (string_cache[i]) {
      free(string_cache[i]);
      string_cache[i] = NULL;
    }
  }
}
