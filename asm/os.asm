init:
    ;pshb #32
    ;pshb #54
    ;jsr #fnMultiply8
    
    
    ;jsr #fnPrintHex
    ;add SP#1
    ;jsr #fnPrintHex
    pshw #0x4000
    jsr #fnReadTillEnter
    
    pshw #0x4000
    jsr #fnToDec

    add SP#2
    hlt
    
    jmp #end
.str: 
    .string "##  N8 CPU  ##",0xa
    .string " - hola - ",0x0

fnPrintStr:
    mov SP,FP
    add FP#2

.loop:
    ld A[&FP]
    cmp A#0x0
    jpz #.return
    st A&0x2000
    ld A[FP]
    add A#1
    st A[FP]

    add FP#1
    
    ld A[SP]
    adc A#0
    st A[SP]

    sub FP#1
    
    jmp #.loop
.return:
    
    rts

fnPrintInvStr:
    mov SP,FP
    sub FP#1
.loop:
    ld A[FP]
    cmp A#0x0
    jpz #.return
    st A&0x2000
    sub FP#1
    jmp #.loop
.return:
    
    rts


fnPrintHex:
    add FP,SP#2
    ld A[FP]
    
    shr4
    cmp A#0xa
    bcs #.letter
    add A#'0'
    st A&0x2000
    
    ld A[FP]
    and A#0xf
    cmp A#0xa
    bcs #.letter
    add A#'0'
    st A&0x2000
    
    ;mov SP,FP
    rts

.letter:
    add A#0x7
    rts



end:
    jmp #end

fnAdd16:
    add FP,SP#2

    ld A[FP]
    add FP,SP#4
    ld B[FP]
    add A
    add FP,SP#2
    st A[FP]

    add FP,SP#3
    
    ld A[FP]
    add FP,SP#5
    ld B[FP]
    add A
    add FP,SP#3
    st A[FP]
   

    add FP,SP#2

    ld A[FP]
    adc A#0x0
    st A[FP]

    rts

fnShiftLeft16:
    add FP,SP#2
    
    ld A[FP]
    shl 
    st A[FP]

    add FP,SP#3
    
    ld A[FP]
    shl 
    st A[FP]

    add FP,SP#2

    ld A[FP]
    adc A#0x0
    st A[FP]

    rts

fnMultiply8:
    sub SP#5
    ; 0.1 res   2.3 add   4 ref   5.6 PC   7 arg1   8 arg0
    add FP,SP#7
    
    ld A[FP]
    add FP,SP#8
    ld B[FP]

    add FP,SP#4
    
    st B[FP]
    add FP,SP#3
    st A[FP]
    add FP,SP#4
    
.loop:
    ld A[FP]
    and A#0x1
    jnz #.add
.goback:
    add FP,SP#4

    ld A[FP]
    shr
    st A[FP]
    
    cmp A#0x0
    jpz #.return

    add FP,SP#2
    ld A[FP]
    add FP,SP#3
    ld b[FP]

    psh B
    psh A
    jsr #fnShiftLeft16
    pop A 
    pop B

    add FP,SP#2
    st A[FP]
    add FP,SP#3
    st b[FP]
    
    add FP,SP#4
    jmp #.loop
.add:
    
    jsr #fnAdd16
    jmp #.goback
.return:
    
    mov SP,FP
    ld A[FP]
    add FP#1
    ld B[FP]

    add FP,SP#7
    st A[FP]
    add FP,SP#8
    st B[FP]

    add SP#5

    rts



fnToDec:
    sub 
    add FP,SP#4
.loop:
    jmp #.loop
.return:
    rts

fnReadTillEnter:
    add FP,SP#2
.loop:
    ld A&0x2000
    cmp A#0x0
    jnz #.put
    jmp #.loop
.put:
    cmp A#0xd
    jpz #.return
    st A[&FP]
    
    ld A[FP]
    add FP,SP#3
    ld B[FP]
    
    pshsw #1
    psh A
    psh B
    jsr #fnAdd16
    pop B
    pop A
    add SP#2

    add FP,SP#2
    st A[FP]
    add FP,SP#3
    st B[FP]

    add FP,SP#2

    jmp #.loop
.return:
    
    rts

fnNormalizeString:
    
    rts
