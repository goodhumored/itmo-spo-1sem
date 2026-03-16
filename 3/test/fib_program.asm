; Generated assembly listing
; Program listing for virtual machine

[section name=_code, bank=RAM]
    MOV SP, 0xFFFF
    CALL main
    HALT

fib:
    PUSH BP
    MOV BP, SP
    SUB SP, 8
fib_entry_0:
    LOAD R0, [BP+8]
    MOV R1, 2
    CMP R0, R1  ; LT (<)
    JL fib_then_2
    JMP fib_else_4
    JMP fib_then_2
fib_then_2:
    MOV R1, 1
    STORE [BP-4], R1
    JMP fib_merge_3
fib_merge_3:
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
    JMP fib_merge_3
fib_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
main:
    PUSH BP
    MOV BP, SP
    SUB SP, 4
main_entry_0:
    MOV R0, 10
    PUSH R0
    CALL fib
    ADD SP, 4
    STORE [BP-4], R0
    JMP main_exit_1
main_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
