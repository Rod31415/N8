init:
    ;pshb #32
    ;pshb #54
    ;jsr #fnMultiply8
    
    
    ;jsr #fnPrintHex
    ;add SP#1
    ;jsr #fnPrintHex
    pshsw #0x9c4
    pshsw #0xa
    jsr #fnwMultiply16
    add SP#2
    jsr #fnPrintHex
    add SP#1
    jsr #fnPrintHex



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

fnwAdd16:
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

fnwShiftLeft16:
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

fnwMultiply8:
    sub SP#5
    ; 0.1 res   2.3 add   4 ref   5.6 PC   7 arg1   8 arg0
    
    add FP,SP#0
    ld A#0x0
    st A[FP];0
    add FP#1
    st A[FP];1
    add FP#1
    st A[FP];2
    add FP#1
    st A[FP];3
    add FP#1
    st A[FP];4

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
    jsr #fnwShiftLeft16
    pop A 
    pop B

    add FP,SP#2
    st A[FP]
    add FP,SP#3
    st b[FP]
    
    add FP,SP#4
    jmp #.loop
.add:
    
    jsr #fnwAdd16
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

fnwMultiply16:
    sub SP#2
    ; 0.1 res  2.3 PC  4.5 arg1  6.7 arg0
    mov SP,FP
    add FP,SP#7
    
    ld A[FP]
    psh A
    sub FP#2
    
    ld A[FP]
    psh A
    
    jsr #fnwMultiply8

    pop A
    pop B

    add FP,SP#0 
    st A[FP]
    add FP#1
    st B[FP]

    add FP,SP#7
    
    ld A[FP]
    psh A
    sub FP#3
    ld A[FP]
    psh A

    jsr #fnwMultiply8

    pop A
    pop A
    
    add FP,SP#0
    ld B[FP]
    add A
    st A[FP]
    
    add FP,SP#6
    
    ld A[FP]
    psh A
    sub FP#1
    ld A[FP]
    psh A

    jsr #fnwMultiply8

    pop A
    pop A
    
    add FP,SP#0
    ld B[FP]
    add A
    st A[FP]

    ld A[FP]
    add FP#1
    ld B[FP]

    add FP,SP#6
    st A[FP]
    add FP#1
    st B[FP]

    add SP#2
    rts

fnToDec:
    ; 0.1 res  2.3 0xA  4.5 PC  6.7 arg0
    pshsw #10
    sub SP#2
    add FP,SP#6
.loop:
    ld A[&FP]
    cmp A#0x0
    jnz #.put
    jmp #.loop
.put:
    

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
    jsr #fnwAdd16
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
