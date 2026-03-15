#include "utils.h"
#include <stdlib.h>
#include <string.h>

const char* get_instruction_name(VMInstructionType type) {
    switch (type) {
        case VM_MOV: return "MOV";
        case VM_ADD: return "ADD";
        case VM_SUB: return "SUB";
        case VM_MUL: return "MUL";
        case VM_DIV: return "DIV";
        case VM_CMP: return "CMP";
        case VM_JMP: return "JMP";
        case VM_JE: return "JE";
        case VM_JNE: return "JNE";
        case VM_JL: return "JL";
        case VM_JG: return "JG";
        case VM_PUSH: return "PUSH";
        case VM_POP: return "POP";
        case VM_CALL: return "CALL";
        case VM_RET: return "RET";
        case VM_LOAD: return "LOAD";
        case VM_STORE: return "STORE";
        case VM_HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

const char* get_register_name(VMRegister reg) {
    switch (reg) {
        case R0: return "R0";
        case R1: return "R1";
        case R2: return "R2";
        case R3: return "R3";
        case R4: return "R4";
        case R5: return "R5";
        case R6: return "R6";
        case R7: return "R7";
        case IP: return "IP";
        case SP: return "SP";
        case BP: return "BP";
        case FLAGS: return "FLAGS";
        default: return "UNKNOWN";
    }
}

void print_operand(VMOperand op, FILE *out) {
    if (!out) return;

    switch (op.type) {
        case OP_REGISTER:
            fprintf(out, "%s", get_register_name(op.value.reg));
            break;
        case OP_IMMEDIATE:
            fprintf(out, "%d", op.value.immediate);
            break;
        case OP_MEMORY:
            fprintf(out, "[%d]", op.value.address);
            break;
        case OP_BP_OFFSET:
            fprintf(out, "[BP%s%d]", op.value.offset >= 0 ? "+" : "", op.value.offset);
            break;
        case OP_LABEL:
            fprintf(out, "%s", op.value.label);
            break;
        case OP_NONE:
        default:
            break;
    }
}

void print_program_listing(VMProgram *program, FILE *out) {
    if (!program || !out) return;

    fprintf(out, "; Generated assembly listing\n");
    fprintf(out, "; Program listing for virtual machine\n\n");

    // Calculate total data size
    int data_size = 0;
    for (int i = 0; i < program->data_item_count; i++) {
        data_size += program->data_items[i].size;
    }

    // Print data section
    if (program->data_item_count > 0) {
        fprintf(out, "[section name=_data, bank=RAM]\n");
        for (int i = 0; i < program->data_item_count; i++) {
            VMDataItem *item = &program->data_items[i];
            fprintf(out, "%s: ", item->name);

            switch (item->type) {
                case DATA_DWORD:
                    if (item->value_count > 0) {
                        fprintf(out, "dd ");
                        for (int j = 0; j < item->value_count; j++) {
                            if (j > 0) fprintf(out, ", ");
                            fprintf(out, "%d", item->data.dwords[j]);
                        }
                    } else {
                        fprintf(out, "resd %d", item->size / 4);
                    }
                    break;
                case DATA_BYTES:
                    if (item->value_count > 0) {
                        // Check if it's a printable string
                        int is_printable = 1;
                        for (int j = 0; j < item->value_count && is_printable; j++) {
                            unsigned char c = (unsigned char)item->data.bytes[j];
                            if (c < 32 || c > 126) {
                                if (c != '\n' && c != '\r' && c != '\t') {
                                    is_printable = 0;
                                }
                            }
                        }

                        if (is_printable) {
                            // Print as string literal
                            fprintf(out, "db \"");
                            for (int j = 0; j < item->value_count; j++) {
                                char c = item->data.bytes[j];
                                if (c == '\n') fprintf(out, "\\n");
                                else if (c == '\r') fprintf(out, "\\r");
                                else if (c == '\t') fprintf(out, "\\t");
                                else if (c == '\"') fprintf(out, "\\\"");
                                else if (c == '\\') fprintf(out, "\\\\");
                                else fprintf(out, "%c", c);
                            }
                            fprintf(out, "\"");
                        } else {
                            // Print as bytes
                            fprintf(out, "db ");
                            for (int j = 0; j < item->value_count; j++) {
                                if (j > 0) fprintf(out, ", ");
                                fprintf(out, "%d", (unsigned char)(item->data.bytes[j]));
                            }
                        }
                    } else {
                        fprintf(out, "resb %d", item->size);
                    }
                    break;
                case DATA_UNINIT:
                    fprintf(out, "resb %d", item->size);
                    break;
            }
            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    }

    // Print code section with offset address
    fprintf(out, "[section name=_code, bank=RAM, address=%d]\n", data_size);

    // Add CALL main at program start (unified entry point)
    fprintf(out, "    CALL main\n\n");

    // Печатаем метки и инструкции
    int current_address = 0;
    for (int i = 0; i < program->instruction_count; i++) {
        // Проверяем, есть ли метка для текущего адреса
        for (int j = 0; j < program->label_count; j++) {
            if (program->labels[j].address == current_address) {
                fprintf(out, "%s:\n", program->labels[j].name);
            }
        }
        
        // Печатаем инструкцию
        VMInstruction *instr = &program->instructions[i];
        fprintf(out, "    %s", get_instruction_name(instr->type));

        if (instr->operand_count > 0) {
            fprintf(out, " ");
            print_operand(instr->operands[0], out);

            if (instr->operand_count > 1) {
                fprintf(out, ", ");
                print_operand(instr->operands[1], out);
            }
        }

        // Выводим комментарий если есть
        if (instr->comment) {
            fprintf(out, "  ; %s", instr->comment);
        }

        fprintf(out, "\n");
        current_address++;
    }

    // Add HALT at the end of main code section
    fprintf(out, "    HALT\n");

    fprintf(out, "\n");
}

