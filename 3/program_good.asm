; Generated assembly listing
; Program listing for virtual machine

[section name=_code, bank=RAM]
    MOV SP, 0xFFFF
    CALL main
    HALT

main:
    PUSH BP
    MOV BP, SP
    SUB SP, 16
main_entry_0:
    MOV R0, 0
    STORE [BP-4], R0
    JMP main_while_cond_2
main_while_cond_2:
    MOV R0, 1
    CMP R0, 0
    JNE main_while_body_3
    JMP main_while_exit_4
main_while_body_3:
    LOAD R4, [BP-4]
    ADD  R4, 48
    PUSH R4
    CALL writeByte
    ADD SP, 4
    CALL readByte
    STORE [BP-8], R0
    CALL readByte
    STORE [BP-12], R0
    LOAD R0, [BP-12]
    MOV R1, 48
    SUB R0, R1  ; SUB: t2 = digit SUB 
    STORE [BP-16], R0
    LOAD R1, [BP-8]
    MOV R2, 43
    CMP R1, R2  ; EQ (=)
    JE main_then_5
    JMP main_merge_6
main_while_exit_4:
main_then_5:
    LOAD R2, [BP-4]
    LOAD R3, [BP-16]
    ADD R2, R3  ; ADD: t4 = acc ADD operand
    STORE [BP-4], R2
    JMP main_merge_6
main_merge_6:
    LOAD R1, [BP-8]
    mov R4, 45
    CMP R1, R4  ; EQ (=)
    JE main_then_7
    JMP main_merge_8
main_then_7:
    LOAD R2, [BP-4]
    LOAD R3, [BP-16]
    SUB R2, R3  ; SUB: t6 = acc SUB operand
    STORE [BP-4], R2
    JMP main_merge_8
main_merge_8:
    LOAD R1, [BP-8]
    mov R4, 42
    CMP R1, R4  ; EQ (=)
    JE main_then_9
    JMP main_merge_10
main_then_9:
    LOAD R2, [BP-4]
    LOAD R3, [BP-16]
    MUL R2, R3  ; MUL: t8 = acc MUL operand
    STORE [BP-4], R2
    JMP main_merge_10
main_merge_10:
    LOAD R1, [BP-8]
    mov R4, 47
    CMP R1, R4  ; EQ (=)
    JE main_then_11
    JMP main_merge_12
main_then_11:
    LOAD R2, [BP-4]
    LOAD R3, [BP-16]
    DIV R2, R3  ; DIV: t10 = acc DIV operand
    STORE [BP-4], R2
    JMP main_merge_12
main_merge_12:
    LOAD R1, [BP-8]
    mov R4, 102
    CMP R1, R4  ; EQ (=)
    JE main_then_13
    JMP main_merge_14
main_then_13:
    LOAD R4, [BP-16]
    PUSH R4
    CALL fib
    ADD SP, 4
    STORE [BP-4], R0
    JMP main_merge_14
main_merge_14:
    LOAD R1, [BP-8]
    mov R4, 48
    CMP R1, R4  ; EQ (=)
    JE main_exit_1
    JMP main_while_cond_2
main_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
fib:
    PUSH BP
    MOV BP, SP
    SUB SP, 8
fib_entry_0:
    LOAD R0, [BP+8]
    MOV R1, 3
    CMP R0, R1  ; LT (<)
    JL fib_then_2
    JMP fib_else_4
fib_then_2:
    MOV R0, 1
    STORE [BP-4], R0
    JMP fib_exit_1
fib_else_4:
    LOAD R1, [BP+8]
    MOV R2, 2
    SUB R1, R2
    PUSH R1
    CALL fib
    ADD SP, 4
    STORE [BP-4], R0
    LOAD R1, [BP+8]
    MOV R2, 1
    SUB R1, R2
    PUSH R1
    CALL fib
    ADD SP, 4
    LOAD R1, [BP-4]
    ADD R1, R0  ; ADD: t3 = r ADD t2
    STORE [BP-4], R1
    JMP fib_exit_1
fib_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET

;writeByte:
;    PUSH BP
;    MOV BP, SP
;    MOV R0, [BP+8]
;    OUT R0
;    MOV R0, 10
;    OUT R0
;    POP BP
;    RET
;
;readByte:
;    IN R0
;    RET


[section _data, RAM]
str_0: db 48, 0
str_1: db 43, 0
str_2: db 45, 0
str_3: db 42, 0
str_4: db 47, 0
str_5: db 102, 0

