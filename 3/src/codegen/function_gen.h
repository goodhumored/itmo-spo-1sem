#ifndef FUNCTION_GEN_H
#define FUNCTION_GEN_H

#include "function_context.h"
#include "../../../2/cfg.h"

void generate_function_code(FunctionContext *ctx, CFG *cfg);
VMProgram *generate_code(CFG **cfgs, int cfg_count);

#endif
