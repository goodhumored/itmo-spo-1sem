#include "register_allocator.h"

VMRegister allocate_register(RegisterAllocator *allocator) {
  for (int i = 0; i < VM_GENERAL_REGISTERS; i++) {
    if (!allocator->used[i]) {
      allocator->used[i] = true;
      return (VMRegister)i;
    }
  }

  return R0;
}

void free_register(RegisterAllocator *allocator, VMRegister reg) {
  if (reg >= R0 && reg <= R7) {
    allocator->used[reg] = false;
  }
}

void free_all_registers(RegisterAllocator *allocator) {
  for (int i = 0; i < VM_GENERAL_REGISTERS; i++) {
    allocator->used[i] = false;
  }
}
