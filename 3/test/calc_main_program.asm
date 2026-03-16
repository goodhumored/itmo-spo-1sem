; Generated assembly listing
; Program listing for virtual machine

[section name=_code, bank=RAM]
    MOV SP, 0xFFFF
    CALL main
    HALT

main:
    PUSH BP
    MOV BP, SP
    SUB SP, 20
main_entry_0:
    MOV R0, 0
    STORE [BP-4], R0
    MOV R0, 48
    STORE [BP-20], R0
    JMP main_while_cond_2
main_while_cond_2:
    MOV R0, 1
    CMP R0, 0
    JNE main_while_body_3
    JMP main_while_exit_4
    JMP main_while_body_3
main_while_body_3:
    MOV R0, 12
    LOAD R1, [BP-20]
    ADD R0, R1
    PUSH R0
    CALL writeByte
    ADD SP, 4
    LOAD R0, [BP-4]
    LOAD R1, [BP-20]
    ADD R0, R1
    PUSH R0
    CALL writeByte
    ADD SP, 4
    CALL readByte
    STORE [BP-8], R0
    CALL readByte
    STORE [BP-12], R0
    LOAD R0, [BP-12]
    LOAD R1, [BP-20]
    SUB R0, R1  ; SUB: t4 = digit SUB zero
    STORE [BP-16], R0
    LOAD R2, [BP-8]
    MOV R3, 43
    CMP R2, R3  ; EQ (=)
    JE main_then_5
    JMP main_merge_6
    JMP main_then_5
main_then_5:
    LOAD R3, [BP-4]
    LOAD R4, [BP-16]
    ADD R3, R4  ; ADD: t6 = acc ADD operand
    STORE [BP-4], R3
    JMP main_merge_6
main_merge_6:
    LOAD R2, [BP-8]
    MOV R5, 45
    CMP R2, R5  ; EQ (=)
    JE main_then_7
    JMP main_merge_8
    JMP main_then_7
main_then_7:
    LOAD R3, [BP-4]
    LOAD R4, [BP-16]
    SUB R3, R4  ; SUB: t8 = acc SUB operand
    STORE [BP-4], R3
    JMP main_merge_8
main_merge_8:
    LOAD R2, [BP-8]
    MOV R5, 42
    CMP R2, R5  ; EQ (=)
    JE main_then_9
    JMP main_merge_10
    JMP main_then_9
main_then_9:
    LOAD R3, [BP-4]
    LOAD R4, [BP-16]
    MUL R3, R4  ; MUL: t10 = acc MUL operand
    STORE [BP-4], R3
    JMP main_merge_10
main_merge_10:
    LOAD R2, [BP-8]
    MOV R5, 47
    CMP R2, R5  ; EQ (=)
    JE main_then_11
    JMP main_merge_12
    JMP main_then_11
main_then_11:
    LOAD R3, [BP-4]
    LOAD R4, [BP-16]
    DIV R3, R4  ; DIV: t12 = acc DIV operand
    STORE [BP-4], R3
    JMP main_merge_12
main_merge_12:
    LOAD R2, [BP-8]
    LOAD R1, [BP-20]
    CMP R2, R1  ; EQ (=)
    JE main_then_13
    JMP main_merge_14
    JMP main_then_13
main_then_13:
    JMP main_while_exit_4
main_merge_14:
    JMP main_while_cond_2
main_while_exit_4:
    JMP main_exit_1
main_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
[section _data, RAM]
str_0: db 48, 0
str_1: db 43, 0
str_2: db 45, 0
str_3: db 42, 0
str_4: db 47, 0

