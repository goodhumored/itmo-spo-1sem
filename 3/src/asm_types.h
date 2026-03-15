#ifndef ASM_TYPES_H
#define ASM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Константы
#define VM_GENERAL_REGISTERS 8

// Типы регистров
typedef enum {
    R0, R1, R2, R3, R4, R5, R6, R7,
    IP,  // Instruction Pointer
    SP,  // Stack Pointer
    BP,  // Base Pointer
    FLAGS,
    REG_COUNT
} VMRegister;

// Типы операндов
typedef enum {
    OP_NONE,
    OP_REGISTER,
    OP_IMMEDIATE,
    OP_MEMORY,
    OP_BP_OFFSET,  // Смещение относительно BP для локальных переменных
    OP_LABEL
} VMOperandType;

// Структура операнда
typedef struct {
    VMOperandType type;
    union {
        VMRegister reg;
        int32_t immediate;
        uint32_t address;
        int32_t offset;  // Для OP_BP_OFFSET: смещение от BP
        int32_t string_ref;  // Для OP_IMMEDIATE_STRING: индекс в data_items
        char *label;
    } value;
} VMOperand;

// Типы инструкций
typedef enum {
    // Арифметические операции
    VM_MOV,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    
    // Логические операции
    VM_CMP,
    
    // Переходы
    VM_JMP,
    VM_JE,
    VM_JNE,
    VM_JL,
    VM_JG,
    
    // Стековые операции
    VM_PUSH,
    VM_POP,
    
    // Вызов подпрограмм
    VM_CALL,
    VM_RET,
    
    // Работа с памятью
    VM_LOAD,
    VM_STORE,
    
    // Завершение
    VM_HALT,
    
    VM_INSTRUCTION_COUNT
} VMInstructionType;

// Структура инструкции
typedef struct {
    VMInstructionType type;
    VMOperand operands[2];
    int operand_count;
    char *comment;  // Комментарий для отладки (может быть NULL)
} VMInstruction;

// Тип элемента данных
typedef enum {
    DATA_DWORD,      // 32-bit values
    DATA_BYTES,      // Byte array (for strings)
    DATA_UNINIT      // Uninitialized data
} VMDataType;

// Структура элемента данных
typedef struct {
    char *name;
    VMDataType type;
    int size;
    union {
        int32_t *dwords;    // For DATA_DWORD
        char *bytes;        // For DATA_BYTES
    } data;
    int value_count;
} VMDataItem;

// Структура метки
typedef struct {
    char *name;
    int address;
} VMLabel;

// Структура образа программы (только для генерации ассемблера)
typedef struct {
    VMInstruction *instructions;
    int instruction_count;
    int max_instructions;
    
    VMDataItem *data_items;
    int data_item_count;
    int max_data_items;
    
    VMLabel *labels;
    int label_count;
    int max_labels;
} VMProgram;

// Функции для работы с программой
VMProgram* create_vm_program();
void free_vm_program(VMProgram *program);

void add_instruction(VMProgram *program, VMInstructionType type, VMOperand op1, VMOperand op2);
void add_instruction_with_comment(VMProgram *program, VMInstructionType type, VMOperand op1, VMOperand op2, const char *comment);
void add_data_item(VMProgram *program, const char *name, int32_t *values, int value_count);
void add_data_bytes(VMProgram *program, const char *name, const char *bytes, int size);
void add_label(VMProgram *program, const char *name, int address);

VMOperand vm_create_register_operand(VMRegister reg);
VMOperand vm_create_immediate_operand(int32_t value);
VMOperand vm_create_memory_operand(uint32_t address);
VMOperand vm_create_bp_offset_operand(int32_t offset);
VMOperand vm_create_label_operand(const char *label);
VMOperand vm_create_operand();

#endif
