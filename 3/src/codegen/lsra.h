#ifndef LSRA_H
#define LSRA_H

#include "../../../2/cfg.h"
#include "../asm_types.h"
#include "liveness.h"

// Linear Scan Register Allocator configuration
#define MAX_REGISTERS 8

// Register assignment for a variable
typedef struct {
    int var_id;
    int reg;              // register index (0-7) or -1 if spilled
    int stack_offset;     // stack offset if spilled
    bool in_register;     // true if in register, false if spilled
} RegisterAssignment;

// Result of LSRA
typedef struct {
    RegisterAssignment *assignments;  // Array indexed by var_id
    int count;
    FunctionLiveness *liveness;
    LiveIntervalSet *intervals;
} LSRA_Result;

// Perform linear scan register allocation
LSRA_Result* lsra_allocate(CFG *cfg, VariableMap *var_map, int num_physical_regs);

// Get register assignment for a variable
int lsra_get_register(LSRA_Result *ra, int var_id);

// Check if variable is spilled
bool lsra_is_spilled(LSRA_Result *ra, int var_id);

// Get stack offset for spilled variable
int lsra_get_stack_offset(LSRA_Result *ra, int var_id);

// Free LSRA result
void lsra_free_result(LSRA_Result *ra);

// Print allocation for debugging
void lsra_print_allocation(LSRA_Result *ra, const VariableMap *var_map);

#endif