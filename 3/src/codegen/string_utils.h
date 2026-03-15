#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "../../../2/cfg.h"
#include "../asm_types.h"

// Collect all string literals from CFG and generate .data section entries
// This should be called before generating code to ensure all strings are added
// to the program
void collect_string_literals_from_cfg(CFG **cfgs, int cfg_count,
                                      VMProgram *program);

// Get the data section label for a string literal
// Returns NULL if the string was not found in the collected strings
const char* get_string_label(const char *literal_value);

// Clean up string resources (optional, for memory management)
void cleanup_string_literals();

#endif
