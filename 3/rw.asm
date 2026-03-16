[section name=_code1, bank=RAM, start=0x0400]

writeByte:
    PUSH BP
    MOV BP, SP
    MOV R0, [BP+8]
    OUT R0
    MOV R0, 10
    OUT R0
    POP BP
    RET

readByte:
    IN R0
    RET
