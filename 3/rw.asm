readByte:
    IN R0
    RET 

writeByte:
    MOV R0, [SP+4]
    OUT R0

    MOV R0, 10
    OUT R0

    ADD SP, 4
    RET
