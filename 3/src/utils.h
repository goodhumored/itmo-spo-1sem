#ifndef UTILS_H
#define UTILS_H

#include "asm_types.h"
#include <stdio.h>

// Функции для работы с программой
void print_program_listing(VMProgram *program, FILE *out);

// Функции для работы с инструкциями
const char* get_instruction_name(VMInstructionType type);
const char* get_register_name(VMRegister reg);
void print_operand(VMOperand op, FILE *out);

#endif
