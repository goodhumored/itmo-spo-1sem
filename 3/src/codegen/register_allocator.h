#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include "../asm_types.h"
#include <stdbool.h>

typedef struct {
    bool used[VM_GENERAL_REGISTERS];
} RegisterAllocator;

VMRegister allocate_register(RegisterAllocator *allocator);
void free_register(RegisterAllocator *allocator, VMRegister reg);
void free_all_registers(RegisterAllocator *allocator);

#endif
