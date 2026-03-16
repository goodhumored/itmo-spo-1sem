; Generated assembly listing
; Program listing for virtual machine

[section name=_code, bank=RAM]
    MOV SP, 0xFFFF
    CALL main
    HALT

compute:
    PUSH BP
    MOV BP, SP
    SUB SP, 12
compute_entry_0:
    MOV R0, 0
    STORE [BP-4], R0
    MOV R0, 0
    STORE [BP-8], R0
    JMP compute_while_cond_2
compute_while_cond_2:
    LOAD R0, [BP-4]
    LOAD R1, [BP+8]
    CMP R0, R1  ; LT (<)
    JL compute_while_body_3
    JMP compute_while_exit_4
    JMP compute_while_body_3
compute_while_body_3:
    LOAD R0, [BP-4]
    MOV R2, 5
    CMP R0, R2  ; GT (>)
    JG compute_then_5
    JMP compute_else_7
    JMP compute_then_5
compute_then_5:
    LOAD R2, [BP-8]
    LOAD R0, [BP-4]
    ADD R2, R0  ; ADD: t2 = result ADD i
    STORE [BP-8], R2
    JMP compute_merge_6
compute_merge_6:
    LOAD R0, [BP-4]
    MOV R3, 1
    ADD R0, R3  ; ADD: t6 = i ADD 1
    STORE [BP-4], R0
    JMP compute_while_cond_2
compute_else_7:
    LOAD R0, [BP-4]
    MOV R3, 2
    CMP R0, R3  ; LT (<)
    JL compute_then_8
    JMP compute_else_10
    JMP compute_then_8
compute_then_8:
    LOAD R2, [BP-8]
    MOV R3, 1
    ADD R2, R3  ; ADD: t4 = result ADD 1
    STORE [BP-8], R2
    JMP compute_merge_9
compute_merge_9:
    JMP compute_merge_6
compute_else_10:
    LOAD R2, [BP-8]
    MOV R3, 2
    ADD R2, R3  ; ADD: t5 = result ADD 2
    STORE [BP-8], R2
    JMP compute_merge_9
compute_while_exit_4:
    JMP compute_exit_1
compute_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
main:
    PUSH BP
    MOV BP, SP
    SUB SP, 12
main_entry_0:
    MOV R0, 10
    STORE [BP-4], R0
    LOAD R0, [BP-4]
    PUSH R0
    CALL compute
    ADD SP, 4
    STORE [BP-8], R0
    LOAD R0, [BP-8]
    MOV R1, 20
    CMP R0, R1  ; GT (>)
    JG main_then_2
    JMP main_else_4
    JMP main_then_2
main_then_2:
    LOAD R0, [BP-8]
    MOV R1, 10
    SUB R0, R1  ; SUB: t2 = b SUB 10
    STORE [BP-12], R0
    JMP main_merge_3
main_merge_3:
    JMP main_exit_1
main_else_4:
    LOAD R0, [BP-8]
    MOV R1, 5
    ADD R0, R1  ; ADD: t3 = b ADD 5
    STORE [BP-12], R0
    JMP main_merge_3
main_exit_1:
    LOAD R0, [BP-4]
    MOV SP, BP
    POP BP
    RET
