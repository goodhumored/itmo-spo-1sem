#include "asm_types.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

VMProgram* create_vm_program() {
    VMProgram *program = malloc(sizeof(VMProgram));
    if (!program) {
        fprintf(stderr, "Error: Failed to allocate VMProgram\n");
        return NULL;
    }
    
    program->instructions = NULL;
    program->instruction_count = 0;
    program->max_instructions = 0;
    
    program->data_items = NULL;
    program->data_item_count = 0;
    program->max_data_items = 0;
    
    program->labels = NULL;
    program->label_count = 0;
    program->max_labels = 0;
    
    return program;
}

void free_vm_program(VMProgram *program) {
    if (!program) return;

    for (int i = 0; i < program->instruction_count; i++) {
        for (int j = 0; j < program->instructions[i].operand_count; j++) {
            if (program->instructions[i].operands[j].type == OP_LABEL &&
                program->instructions[i].operands[j].value.label) {
                free(program->instructions[i].operands[j].value.label);
            }
        }
    }
    free(program->instructions);

    for (int i = 0; i < program->data_item_count; i++) {
        free(program->data_items[i].name);
        if (program->data_items[i].type == DATA_DWORD) {
            free(program->data_items[i].data.dwords);
        } else if (program->data_items[i].type == DATA_BYTES) {
            free(program->data_items[i].data.bytes);
        }
    }
    free(program->data_items);

    for (int i = 0; i < program->label_count; i++) {
        free(program->labels[i].name);
    }
    free(program->labels);

    free(program);
}

void add_instruction(VMProgram *program, VMInstructionType type, VMOperand op1, VMOperand op2) {
    if (!program) return;
    
    if (program->instruction_count >= program->max_instructions) {
        program->max_instructions = program->max_instructions == 0 ? 16 : program->max_instructions * 2;
        program->instructions = realloc(program->instructions,
                                       program->max_instructions * sizeof(VMInstruction));
    }
    
    VMInstruction *instr = &program->instructions[program->instruction_count++];
    instr->type = type;
    
    if (op1.type != OP_NONE) {
        instr->operands[0] = op1;
        instr->operand_count = 1;
        
        if (op2.type != OP_NONE) {
            instr->operands[1] = op2;
            instr->operand_count = 2;
        }
    } else {
        instr->operand_count = 0;
    }
    printf("added %d-th instr %s\n", program->instruction_count, get_instruction_name(instr->type));
}

void add_data_item(VMProgram *program, const char *name, int32_t *values, int value_count) {
    if (!program) return;

    if (program->data_item_count >= program->max_data_items) {
        program->max_data_items = program->max_data_items == 0 ? 8 : program->max_data_items * 2;
        program->data_items = realloc(program->data_items,
                                     program->max_data_items * sizeof(VMDataItem));
    }

    VMDataItem *item = &program->data_items[program->data_item_count++];
    item->name = strdup(name);
    item->type = DATA_DWORD;
    item->size = value_count * sizeof(int32_t);
    item->data.dwords = NULL;
    item->value_count = 0;

    if (values && value_count > 0) {
        item->data.dwords = malloc(value_count * sizeof(int32_t));
        memcpy(item->data.dwords, values, value_count * sizeof(int32_t));
        item->value_count = value_count;
    }
}

void add_data_bytes(VMProgram *program, const char *name, const char *bytes, int size) {
    if (!program) return;

    if (program->data_item_count >= program->max_data_items) {
        program->max_data_items = program->max_data_items == 0 ? 8 : program->max_data_items * 2;
        program->data_items = realloc(program->data_items,
                                     program->max_data_items * sizeof(VMDataItem));
    }

    VMDataItem *item = &program->data_items[program->data_item_count++];
    item->name = strdup(name);
    item->type = DATA_BYTES;
    item->size = size;
    item->data.bytes = NULL;
    item->value_count = 0;

    if (bytes && size > 0) {
        item->data.bytes = malloc(size);
        memcpy(item->data.bytes, bytes, size);
        item->value_count = size;
    }
}

void add_label(VMProgram *program, const char *name, int address) {
    if (!program) return;
    
    if (program->label_count >= program->max_labels) {
        program->max_labels = program->max_labels == 0 ? 8 : program->max_labels * 2;
        program->labels = realloc(program->labels,
                                 program->max_labels * sizeof(VMLabel));
    }
    
    VMLabel *label = &program->labels[program->label_count++];
    label->name = strdup(name);
    label->address = address;
}

VMOperand vm_create_register_operand(VMRegister reg) {
    VMOperand op;
    op.type = OP_REGISTER;
    op.value.reg = reg;
    return op;
}

VMOperand vm_create_immediate_operand(int32_t value) {
    VMOperand op;
    op.type = OP_IMMEDIATE;
    op.value.immediate = value;
    return op;
}

VMOperand vm_create_memory_operand(uint32_t address) {
    VMOperand op;
    op.type = OP_MEMORY;
    op.value.address = address;
    return op;
}

VMOperand vm_create_label_operand(const char *label) {
    VMOperand op;
    op.type = OP_LABEL;
    op.value.label = strdup(label);
    return op;
}

VMOperand vm_create_bp_offset_operand(int32_t offset) {
    VMOperand op;
    op.type = OP_BP_OFFSET;
    op.value.offset = offset;
    return op;
}

VMOperand vm_create_operand() {
    VMOperand op;
    op.type = OP_NONE;
    return op;
}
