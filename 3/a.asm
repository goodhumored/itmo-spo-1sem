; Generated assembly listing
; Program listing for virtual machine

[section name=_code, bank=RAM]
    ; Инициализация стека ДО любого CALL
    MOV SP, 0xFFFF
    CALL main
    HALT

main:
    ; Инициализация стека
    PUSH BP
    MOV BP, SP
    SUB SP, 16
main_entry_0:
    CALL readByte
    STORE [BP+8], R0
    PUSH R0
    CALL writeByte
    JMP main_entry_0
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
    STORE [BP+4], R0
    JMP fib_exit_1
fib_merge_3:
fib_else_4:
    LOAD R1, [BP+8]
    MOV R2, 2
    SUB R1, R2
    PUSH R1
    CALL fib
    ADD SP, 4
    STORE [BP+4], R0
    LOAD R1, [BP+8]
    MOV R2, 1
    SUB R1, R2
    PUSH R1
    CALL fib
    ADD SP, 4
    LOAD R1, [BP+4]
    ADD R1, R0  ; ADD: t3 = r ADD t2
    STORE [BP+4], R1
    JMP fib_exit_1
fib_exit_1:
    LOAD R0, [BP+4]
    MOV SP, BP
    POP BP
    RET

readByte:
    IN R0
    RET

writeByte:
    MOV R0, [SP+4]
    OUT R0

    MOV R0, 0
    OUT R0
    RET

[section _data, RAM]
str_0: db 48, 0
str_1: db 43, 0
str_2: db 45, 0
str_3: db 42, 0
str_4: db 47, 0
str_5: db 102, 0

