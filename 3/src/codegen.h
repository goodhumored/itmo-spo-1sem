#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen/register_allocator.h"
#include "codegen/function_context.h"
#include "codegen/function_gen.h"
#include "codegen/operand_utils.h"
#include "codegen/operation_gen.h"
#include "codegen/block_gen.h"
#include "codegen/string_utils.h"

// Collect all string literals from CFG and generate .data section entries
// Note: Declaration is in string_utils.h, but referenced here for convenience
void collect_string_literals_from_cfg(CFG **cfgs, int cfg_count, VMProgram *program);

#endif