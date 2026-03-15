#include "liveness.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ==================== Variable Map ====================

VariableMap* create_variable_map() {
    VariableMap *map = malloc(sizeof(VariableMap));
    map->names = NULL;
    map->count = 0;
    map->max_vars = 64;
    map->names = malloc(map->max_vars * sizeof(char*));
    for (int i = 0; i < map->max_vars; i++) {
        map->names[i] = NULL;
    }
    return map;
}

void free_variable_map(VariableMap *map) {
    if (!map) return;
    for (int i = 0; i < map->max_vars; i++) {
        free(map->names[i]);
    }
    free(map->names);
    free(map);
}

int get_variable_id(VariableMap *map, const char *name) {
    if (!map || !name) return -1;

    // Search for existing variable
    for (int i = 0; i < map->count; i++) {
        if (map->names[i] && strcmp(map->names[i], name) == 0) {
            return i;
        }
    }

    // Add new variable
    if (map->count >= map->max_vars) {
        map->max_vars *= 2;
        map->names = realloc(map->names, map->max_vars * sizeof(char*));
        for (int i = map->count; i < map->max_vars; i++) {
            map->names[i] = NULL;
        }
    }

    int id = map->count;
    map->names[id] = strdup(name);
    map->count++;
    return id;
}

const char* get_variable_name(const VariableMap *map, int id) {
    if (!map || id < 0 || id >= map->count) return NULL;
    return map->names[id];
}

int get_variable_count(const VariableMap *map) {
    if (!map) return 0;
    return map->count;
}

// ==================== Variable Set ====================

VariableSet* create_variable_set(int num_variables) {
    VariableSet *set = malloc(sizeof(VariableSet));
    int num_words = (num_variables + 31) / 32;
    set->bits = calloc(num_words, sizeof(uint32_t));
    set->size = num_variables;
    return set;
}

void free_variable_set(VariableSet *set) {
    if (!set) return;
    free(set->bits);
    free(set);
}

void variable_set_add(VariableSet *set, int var_id) {
    if (!set || var_id < 0 || var_id >= set->size) return;
    int word_idx = var_id / 32;
    int bit_idx = var_id % 32;
    set->bits[word_idx] |= (1u << bit_idx);
}

void variable_set_remove(VariableSet *set, int var_id) {
    if (!set || var_id < 0 || var_id >= set->size) return;
    int word_idx = var_id / 32;
    int bit_idx = var_id % 32;
    set->bits[word_idx] &= ~(1u << bit_idx);
}

bool variable_set_contains(const VariableSet *set, int var_id) {
    if (!set || var_id < 0 || var_id >= set->size) return false;
    int word_idx = var_id / 32;
    int bit_idx = var_id % 32;
    return (set->bits[word_idx] & (1u << bit_idx)) != 0;
}

void variable_set_copy(VariableSet *dest, const VariableSet *src) {
    if (!dest || !src) return;
    int num_words = (dest->size + 31) / 32;
    for (int i = 0; i < num_words; i++) {
        dest->bits[i] = src->bits[i];
    }
}

void variable_set_union(VariableSet *result, const VariableSet *a, const VariableSet *b) {
    if (!result || !a || !b) return;
    int num_words = (result->size + 31) / 32;
    for (int i = 0; i < num_words; i++) {
        result->bits[i] = a->bits[i] | b->bits[i];
    }
}

void variable_set_difference(VariableSet *result, const VariableSet *a, const VariableSet *b) {
    if (!result || !a || !b) return;
    int num_words = (result->size + 31) / 32;
    for (int i = 0; i < num_words; i++) {
        result->bits[i] = a->bits[i] & ~b->bits[i];
    }
}

bool variable_set_equals(const VariableSet *a, const VariableSet *b) {
    if (!a || !b || a->size != b->size) return false;
    int num_words = (a->size + 31) / 32;
    for (int i = 0; i < num_words; i++) {
        if (a->bits[i] != b->bits[i]) return false;
    }
    return true;
}

void variable_set_clear(VariableSet *set) {
    if (!set) return;
    int num_words = (set->size + 31) / 32;
    memset(set->bits, 0, num_words * sizeof(uint32_t));
}

// ==================== Operand Variable Collection ====================

void collect_operand_variables(Operand *op, VariableMap *var_map, VariableSet *set) {
    if (!op || !var_map || !set) return;

    if (op->kind == OPND_VAR && op->value.name) {
        int var_id = get_variable_id(var_map, op->value.name);
        variable_set_add(set, var_id);
    } else if (op->kind == OPND_TEMP) {
        char temp_name[32];
        snprintf(temp_name, sizeof(temp_name), "temp_%d", op->value.temp_id);
        int var_id = get_variable_id(var_map, temp_name);
        variable_set_add(set, var_id);
    }
}

// ==================== Liveness Analysis ====================

// Compute USE and DEF sets for a single operation
static void compute_operation_use_def(Operation *op, VariableMap *var_map,
                                      VariableSet *use, VariableSet *def) {
    if (!op) return;

    // Add operands to use set
    collect_operand_variables(&op->op1, var_map, use);
    collect_operand_variables(&op->op2, var_map, use);
    collect_operand_variables(&op->dest, var_map, use);

    // Remove dest from use for some operations
    if (op->type == OP_STORE) {
        if (op->op2.kind == OPND_VAR && op->op2.value.name) {
            int var_id = get_variable_id(var_map, op->op2.value.name);
            variable_set_remove(use, var_id);
        }
    }

    // Add destination to def set
    if (op->dest.kind == OPND_VAR || op->dest.kind == OPND_TEMP) {
        int dest_var_id;
        if (op->dest.kind == OPND_VAR) {
            dest_var_id = get_variable_id(var_map, op->dest.value.name);
        } else {
            char temp_name[32];
            snprintf(temp_name, sizeof(temp_name), "temp_%d", op->dest.value.temp_id);
            dest_var_id = get_variable_id(var_map, temp_name);
        }
        variable_set_add(def, dest_var_id);
    }

    // Remove operand variables that are also in def (x = x + 1)
    VariableSet use_minus_def;
    use_minus_def.size = use->size;
    use_minus_def.bits = malloc(((use->size + 31) / 32) * sizeof(uint32_t));
    variable_set_difference(&use_minus_def, use, def);
    variable_set_copy(use, &use_minus_def);
    free(use_minus_def.bits);
}

// Liveness Analysis implementation
FunctionLiveness* compute_liveness(CFG *cfg, VariableMap *var_map) {
    if (!cfg) return NULL;

    FunctionLiveness *liveness = malloc(sizeof(FunctionLiveness));
    liveness->cfg = cfg;
    liveness->block_count = cfg->block_count;
    liveness->block_liveness = malloc(cfg->block_count * sizeof(BlockLiveness*));

    printf("Computing liveness for %s with %d blocks...\n", cfg->function->name, cfg->block_count);

    // First pass: collect all variables and compute USE/DEF for operations
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        if (!block) continue;

        BlockLiveness *blk_liveness = malloc(sizeof(BlockLiveness));
        blk_liveness->use = *(create_variable_set(var_map->count));
        blk_liveness->def = *(create_variable_set(var_map->count));
        blk_liveness->live_in = *(create_variable_set(var_map->count));
        blk_liveness->live_out = *(create_variable_set(var_map->count));

        blk_liveness->op_liveness = malloc(block->op_count * sizeof(OperationLiveness*));
        for (int j = 0; j < block->op_count; j++) {
            blk_liveness->op_liveness[j] = malloc(sizeof(OperationLiveness));

            Operation *op = block->operations[j];
            OperationLiveness *op_live = blk_liveness->op_liveness[j];

            op_live->use = *(create_variable_set(var_map->count));
            op_live->def = *(create_variable_set(var_map->count));
            op_live->live_in = *(create_variable_set(var_map->count));
            op_live->live_out = *(create_variable_set(var_map->count));

            // Initialize operation USE/DEF
            compute_operation_use_def(op, var_map, &op_live->use, &op_live->def);

            // Add to block USE/DEF
            variable_set_union(&blk_liveness->use, &blk_liveness->use, &op_live->use);
            variable_set_union(&blk_liveness->def, &blk_liveness->def, &op_live->def);
        }

        liveness->block_liveness[i] = blk_liveness;
    }

    printf("  Collected %d variables\n", var_map->count);

    // Second pass: iterative dataflow analysis to compute block live_in/live_out
    bool changed = true;
    int iteration = 0;
    while (changed) {
        changed = false;
        iteration++;

        if (iteration > 100) {
            printf("  ERROR: Liveness did not converge after 100 iterations!\n");
            printf("  This likely indicates a bug in the liveness analysis.\n");
            break;
        }

        for (int i = 0; i < cfg->block_count; i++) {
            BasicBlock *block = cfg->blocks[i];
            BlockLiveness *blk = liveness->block_liveness[i];
            if (!block || !blk) continue;

            // Compute live_out: union of successors' live_in
            VariableSet new_live_out = *(create_variable_set(var_map->count));
            variable_set_clear(&new_live_out);

            for (int s = 0; s < block->successor_count; s++) {
                BasicBlock *succ = block->successors[s];
                if (!succ) continue;

                BlockLiveness *succ_live = NULL;
                for (int j = 0; j < cfg->block_count; j++) {
                    if (cfg->blocks[j] == succ) {
                        succ_live = liveness->block_liveness[j];
                        break;
                    }
                }

                if (succ_live) {
                    variable_set_union(&new_live_out, &new_live_out, &succ_live->live_in);
                }
            }

            // Compute live_in: use ∪ (live_out \ def)
            VariableSet live_out_minus_def = *(create_variable_set(var_map->count));
            variable_set_difference(&live_out_minus_def, &new_live_out, &blk->def);

            VariableSet new_live_in = *(create_variable_set(var_map->count));
            variable_set_union(&new_live_in, &blk->use, &live_out_minus_def);

            if (!variable_set_equals(&blk->live_in, &new_live_in) ||
                !variable_set_equals(&blk->live_out, &new_live_out)) {
                changed = true;
                variable_set_copy(&blk->live_in, &new_live_in);
                variable_set_copy(&blk->live_out, &new_live_out);
            }

            free(new_live_out.bits);
            free(live_out_minus_def.bits);
            free(new_live_in.bits);
        }
    }

    if (iteration <= 100) {
        printf("  Liveness converged in %d iterations\n", iteration);
    } else {
        printf("  Liveness FAILED to converge!\n");
    }

    // Third pass: backward pass to compute operation-level live_in/live_out
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        BlockLiveness *blk = liveness->block_liveness[i];
        if (!block || !blk) continue;

        VariableSet live = *create_variable_set(var_map->count);
        variable_set_copy(&live, &blk->live_out);

        for (int j = block->op_count - 1; j >= 0; j--) {
            OperationLiveness *op_live = blk->op_liveness[j];
            if (!op_live) continue;

            variable_set_copy(&op_live->live_out, &live);

            VariableSet live_out_minus_def = *create_variable_set(var_map->count);
            variable_set_difference(&live_out_minus_def, &live, &op_live->def);

            variable_set_union(&op_live->live_in, &op_live->use, &live_out_minus_def);
            variable_set_copy(&live, &op_live->live_in);

            free(live_out_minus_def.bits);
        }

        free(live.bits);
    }

    return liveness;
}

void free_function_liveness(FunctionLiveness *liveness) {
    if (!liveness) return;
    // TODO: Free allocated memory
    free(liveness);
}

// ==================== Live Intervals ====================

LiveIntervalSet* build_live_intervals(CFG *cfg, FunctionLiveness *liveness, const VariableMap *var_map) {
    if (!cfg || !liveness || !var_map) return NULL;

    LiveIntervalSet *intervals = malloc(sizeof(LiveIntervalSet));
    intervals->count = 0;
    intervals->max_intervals = var_map->count * 2;
    intervals->intervals = malloc(intervals->max_intervals * sizeof(LiveInterval));

    for (int i = 0; i < intervals->max_intervals; i++) {
        intervals->intervals[i].var_id = -1;
        intervals->intervals[i].start = -1;
        intervals->intervals[i].end = -1;
        intervals->intervals[i].reg = -1;
        intervals->intervals[i].stack_offset = 0;
        intervals->intervals[i].is_spilled = false;
    }

    int inst_index = 0;

    // Assign instruction indices and collect interval starts/ends
    for (int blk_idx = 0; blk_idx < cfg->block_count; blk_idx++) {
        BasicBlock *block = cfg->blocks[blk_idx];
        BlockLiveness *blk = liveness->block_liveness[blk_idx];
        if (!block || !blk) continue;

        for (int op_idx = 0; op_idx < block->op_count; op_idx++) {
            OperationLiveness *op_live = blk->op_liveness[op_idx];
            if (!op_live) continue;

            // Check if any variable becomes live here (interval start)
            for (int var_id = 0; var_id < var_map->count; var_id++) {
                if (variable_set_contains(&op_live->live_in, var_id)) {
                    LiveInterval *interval = NULL;
                    for (int i = 0; i < intervals->count; i++) {
                        if (intervals->intervals[i].var_id == var_id) {
                            if (inst_index < intervals->intervals[i].start) {
                                intervals->intervals[i].start = inst_index;
                            }
                            if (inst_index > intervals->intervals[i].end) {
                                intervals->intervals[i].end = inst_index;
                            }
                            interval = &intervals->intervals[i];
                            break;
                        }
                    }

                    if (!interval) {
                        if (intervals->count >= intervals->max_intervals) {
                            intervals->max_intervals *= 2;
                            intervals->intervals = realloc(intervals->intervals,
                                                          intervals->max_intervals * sizeof(LiveInterval));
                            for (int i = intervals->count; i < intervals->max_intervals; i++) {
                                intervals->intervals[i].var_id = -1;
                                intervals->intervals[i].start = -1;
                                intervals->intervals[i].end = -1;
                                intervals->intervals[i].reg = -1;
                                intervals->intervals[i].stack_offset = 0;
                                intervals->intervals[i].is_spilled = false;
                            }
                        }
                        LiveInterval *new_interval = &intervals->intervals[intervals->count];
                        new_interval->var_id = var_id;
                        new_interval->start = inst_index;
                        new_interval->end = inst_index;
                        new_interval->reg = -1;
                        new_interval->stack_offset = 0;
                        new_interval->is_spilled = false;
                        intervals->count++;
                    }
                }
            }

            inst_index++;
        }
    }

    printf("  Built %d live intervals over %d instructions\n", intervals->count, inst_index);
    return intervals;
}

void free_live_intervals(LiveIntervalSet *intervals) {
    if (!intervals) return;
    free(intervals->intervals);
    free(intervals);
}