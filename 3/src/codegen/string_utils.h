#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "../../../2/cfg.h"
#include "../asm_types.h"

// Collect all string literals from CFG and generate .data section entries
// This should be called before generating code to ensure all strings are added
// to the program
void collect_string_literals_from_cfg(CFG **cfgs, int cfg_count,
                                      VMProgram *program);

#endif
