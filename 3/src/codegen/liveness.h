#ifndef LIVENESS_H
#define LIVENESS_H

#include "../../../2/cfg.h"
#include <stdint.h>
#include <stdbool.h>

// Bitset for tracking live variables
typedef struct {
    uint32_t *bits;
    int size;
} VariableSet;

// Liveness information for a single operation
typedef struct {
    VariableSet use;   // Variables used by this operation
    VariableSet def;   // Variables defined by this operation
    VariableSet live_in;  // Variables live before this operation
    VariableSet live_out; // Variables live after this operation
} OperationLiveness;

// Liveness information for a basic block
typedef struct {
    VariableSet live_in;  // Variables live at block entry
    VariableSet live_out; // Variables live at block exit
    VariableSet use;      // Variables used before first definition in block
    VariableSet def;      // Variables defined in block
    OperationLiveness **op_liveness; // Array indexed by operation index
} BlockLiveness;

// Liveness information for a function/CFG
typedef struct {
    CFG *cfg;
    BlockLiveness **block_liveness; // Array indexed by block id
    int block_count;
} FunctionLiveness;

// Map variable names to indices for bitset operations
typedef struct {
    char **names;  // Array of variable names indexed by id
    int count;     // Number of variables
    int max_vars;  // Capacity
} VariableMap;

// Create and destroy functions
VariableMap* create_variable_map();
void free_variable_map(VariableMap *map);
int get_variable_id(VariableMap *map, const char *name);
const char* get_variable_name(const VariableMap *map, int id);
int get_variable_count(const VariableMap *map);

// Variable set operations
VariableSet* create_variable_set(int num_variables);
void free_variable_set(VariableSet *set);
void variable_set_add(VariableSet *set, int var_id);
void variable_set_remove(VariableSet *set, int var_id);
bool variable_set_contains(const VariableSet *set, int var_id);
void variable_set_copy(VariableSet *dest, const VariableSet *src);
void variable_set_union(VariableSet *result, const VariableSet *a, const VariableSet *b);
void variable_set_difference(VariableSet *result, const VariableSet *a, const VariableSet *b);
bool variable_set_equals(const VariableSet *a, const VariableSet *b);
void variable_set_clear(VariableSet *set);

// Liveness analysis
FunctionLiveness* compute_liveness(CFG *cfg, VariableMap *var_map);
void free_function_liveness(FunctionLiveness *liveness);

// Utility: extract variables from operands
void collect_operand_variables(Operand *op, VariableMap *var_map, VariableSet *set);

// Linear scan data structures
typedef struct {
    int var_id;           // Variable ID from VariableMap
    int start;            // Start position (operation index)
    int end;              // End position (operation index)
    int reg;              // Assigned register (-1 if none/spilled)
    int stack_offset;     // Stack offset if spilled
    bool is_spilled;      // true if spilled to stack
} LiveInterval;

typedef struct {
    LiveInterval *intervals;
    int count;
    int max_intervals;
} LiveIntervalSet;

// Build live intervals from liveness analysis
LiveIntervalSet* build_live_intervals(CFG *cfg, FunctionLiveness *liveness, const VariableMap *var_map);
void free_live_intervals(LiveIntervalSet *intervals);

#endif