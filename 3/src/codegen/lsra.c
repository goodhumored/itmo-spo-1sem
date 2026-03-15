#include "lsra.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Active interval (currently allocated interval)
typedef struct ActiveInterval {
    int interval_idx;  // Index in intervals array
    int end;           // End position
    struct ActiveInterval *next;
} ActiveInterval;

// Comparison function for sorting intervals by start position
static int compare_intervals(const void *a, const void *b) {
    const LiveInterval *ia = (const LiveInterval*)a;
    const LiveInterval *ib = (const LiveInterval*)b;
    return ia->start - ib->start;
}

// Add interval to active list (sorted by end)
static void add_active(ActiveInterval **active, int interval_idx, int end) {
    ActiveInterval *new_node = malloc(sizeof(ActiveInterval));
    new_node->interval_idx = interval_idx;
    new_node->end = end;

    // Insert in sorted order by end position
    if (*active == NULL || (*active)->end >= end) {
        new_node->next = *active;
        *active = new_node;
    } else {
        ActiveInterval *curr = *active;
        while (curr->next != NULL && curr->next->end < end) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
    }
}

// Remove interval from active list
static void remove_active(ActiveInterval **active, int interval_idx) {
    ActiveInterval *prev = NULL;
    ActiveInterval *curr = *active;

    while (curr != NULL) {
        if (curr->interval_idx == interval_idx) {
            if (prev == NULL) {
                *active = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Count active intervals
static int count_active(ActiveInterval *active) {
    int count = 0;
    while (active) {
        count++;
        active = active->next;
    }
    return count;
}

// Remove finished intervals
static void expire_old_intervals(ActiveInterval **active, LiveInterval *current,
                                 LiveInterval *intervals) {
    if (!active || !current) return;

    while (*active != NULL && (*active)->end < current->start) {
        ActiveInterval *to_remove = *active;
        *active = (*active)->next;
        free(to_remove);
    }
}

// Find interval to spill (one with furthest end)
static int find_spill_candidate(ActiveInterval *active, LiveInterval *intervals) {
    int interval_idx = -1;
    int furthest_end = -1;
    ActiveInterval *curr = active;

    while (curr != NULL) {
        if (curr->end > furthest_end) {
            furthest_end = curr->end;
            interval_idx = curr->interval_idx;
        }
        curr = curr->next;
    }

    return interval_idx;
}

// Linear Scan Register Allocation
LSRA_Result* lsra_allocate(CFG *cfg, VariableMap *var_map, int num_physical_regs) {
    if (!cfg || !var_map) return NULL;

    printf("\n=== Linear Scan Register Allocation ===\n");
    printf("Variables: %d, Physical registers: %d\n", var_map->count, num_physical_regs);

    // Step 1: Compute liveness
    FunctionLiveness *liveness = compute_liveness(cfg, var_map);

    // Step 2: Build live intervals
    LiveIntervalSet *intervals = build_live_intervals(cfg, liveness, var_map);

    if (!intervals || intervals->count == 0) {
        printf("No intervals to allocate\n");
        return NULL;
    }

    // Step 3: Sort intervals by start position
    qsort(intervals->intervals, intervals->count, sizeof(LiveInterval), compare_intervals);

    // Step 4: Linear scan
    ActiveInterval *active = NULL;
    int next_reg = 0;
    int next_stack_offset = 0;

    for (int i = 0; i < intervals->count; i++) {
        LiveInterval *current = &intervals->intervals[i];

        // Remove expired intervals
        expire_old_intervals(&active, current, intervals);

        // Check if we have free register
        int active_count = count_active(active);

        if (active_count < num_physical_regs) {
            // Allocate a register
            current->reg = next_reg++;
            current->is_spilled = false;
            add_active(&active, i, current->end);

            printf("Allocated R%d to var %d [%d, %d]\n",
                   current->reg, current->var_id, current->start, current->end);
        } else {
            // Need to spill
            int spill_idx = find_spill_candidate(active, intervals);
            LiveInterval *spill = &intervals->intervals[spill_idx];

            if (spill->end > current->end) {
                // Spill the old interval
                spill->reg = -1;
                spill->stack_offset = next_stack_offset;
                spill->is_spilled = true;
                next_stack_offset += 4; // 4 bytes per variable

                remove_active(&active, spill_idx);

                // Allocate register to current interval
                current->reg = spill->reg; // Will be assigned below
                current->is_spilled = false;
                next_reg = spill->reg; // Reuse the register
                add_active(&active, i, current->end);

                printf("Spilled var %d to stack offset %d, allocated R%d to var %d [%d, %d]\n",
                       spill->var_id, spill->stack_offset, next_reg,
                       current->var_id, current->start, current->end);
                next_reg++;
            } else {
                // Current interval should be spilled
                current->reg = -1;
                current->stack_offset = next_stack_offset;
                current->is_spilled = true;
                next_stack_offset += 4;

                printf("Spilled var %d to stack offset %d\n",
                       current->var_id, current->stack_offset);
            }
        }
    }

    // Clean up active list
    while (active != NULL) {
        ActiveInterval *to_remove = active;
        active = active->next;
        free(to_remove);
    }

    // Step 5: Create result structure
    LSRA_Result *result = malloc(sizeof(LSRA_Result));
    result->count = var_map->count;
    result->assignments = calloc(var_map->count, sizeof(RegisterAssignment));
    result->liveness = liveness;
    result->intervals = intervals;

    // Map intervals to assignments
    for (int i = 0; i < intervals->count; i++) {
        LiveInterval *interval = &intervals->intervals[i];
        if (interval->var_id < var_map->count) {
            result->assignments[interval->var_id].var_id = interval->var_id;
            result->assignments[interval->var_id].reg = interval->reg;
            result->assignments[interval->var_id].stack_offset = interval->stack_offset;
            result->assignments[interval->var_id].in_register = !interval->is_spilled;
        }
    }

    printf("\n=== Allocation Complete ===\n");
    lsra_print_allocation(result, var_map);

    return result;
}

int lsra_get_register(LSRA_Result *ra, int var_id) {
    if (!ra || var_id < 0 || var_id >= ra->count) return -1;
    return ra->assignments[var_id].reg;
}

bool lsra_is_spilled(LSRA_Result *ra, int var_id) {
    if (!ra || var_id < 0 || var_id >= ra->count) return false;
    return !ra->assignments[var_id].in_register;
}

int lsra_get_stack_offset(LSRA_Result *ra, int var_id) {
    if (!ra || var_id < 0 || var_id >= ra->count) return 0;
    return ra->assignments[var_id].stack_offset;
}

void lsra_free_result(LSRA_Result *ra) {
    if (!ra) return;

    free(ra->assignments);
    if (ra->liveness) {
        free_function_liveness(ra->liveness);
    }
    if (ra->intervals) {
        free_live_intervals(ra->intervals);
    }
    free(ra);
}

void lsra_print_allocation(LSRA_Result *ra, const VariableMap *var_map) {
    if (!ra || !var_map) return;

    printf("Register assignments:\n");
    for (int i = 0; i < var_map->count; i++) {
        const char *name = get_variable_name(var_map, i);
        RegisterAssignment *assign = &ra->assignments[i];

        if (assign->in_register) {
            printf("  %-20s -> R%d\n", name, assign->reg);
        } else {
            printf("  %-20s -> SPILLED @ BP+%d\n", name, assign->stack_offset);
        }
    }
}