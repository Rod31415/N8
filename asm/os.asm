init:
    pshb #0x9c
    pshb #0xdf
    jsr #fnMultiply8
    
    jsr #fnPrintHex
    dec SP
    jsr #fnPrintHex
    
    hlt
    
    jmp #end
.str: 
    .string "##  N8 CPU  ##",0xa
    .string " - hola - ",0x0

fnPrintStr:
    decw SP

.loop:
    ld A[&SP]
    cmp A#0x0
    jpz #.return
    st A&0x2000
    ld A[SP]
    add A#1
    st A[SP]

    dec SP
    
    ld A[SP]
    adc A#0
    st A[SP]

    inc SP
    jmp #.loop
.return:
    incw SP
    rts


end:
    jmp #end

fnAdd16:
    decw SP
    
    ld A[SP]
    decw SP
    ld B[SP]
    add A
    incw SP
    st A[SP]

    dec SP
    
    ld A[SP]
    decw SP
    ld B[SP]
    add A
    incw SP
    st A[SP]
   

    inc SP

    ld A[SP]
    adc A#0x0
    st A[SP]


    incw SP

    rts

fnShiftLeft16:
    decw SP
    
    ld A[SP]
    shl 
    st A[SP]

    
    dec SP
    ld A[SP]
    shl 
    st A[SP]

    inc SP

    ld A[SP]
    adc A#0x0
    st A[SP]

    
    
    incw SP

    rts

fnMultiply8:
    ;pshb #0x0 ;spf + 1 shiftRef
    ;pshw #0x0 ;spf + 3 shiftAdd
    ;pshw #0x0 ;spf + 5 res
    
    ;decd SP   ;spf + 1
    ;dec SP    ;spf + 0
    decw SP   ;spf - 2 [arg 1]
    
    ld A[SP]
    dec SP    ;spf - 3 [arg 0]
    ld B[SP]

    incd SP    ;spf + 1 shiftRef
    
    st B[SP]
    inc SP    ;spf + 2 shiftAdd l
    st A[SP]
    dec SP    ;spf + 1 shiftRef 
    
.loop:
    ld A[SP]
    and A#0x1
    incd SP   ;spf + 5 res
    jnz #.add 
.goback:    
    decd SP   ;spf + 1 shiftRef
    ld A[SP]
    shr
    st A[SP]
    
    cmp A#0x0
    jpz #.return

    incw SP   ;spf + 3 shiftAdd

    ld A[SP]
    shl 
    st A[SP]
    
    dec SP
    ld A[SP]
    shl 
    st A[SP]

    inc SP

    ld A[SP]
    adc A#0x0
    st A[SP]

    decw SP   ;spf + 1 shiftRef 

    jmp #.loop
.add:
    jsr #fnAdd16
    jmp #.goback
.return:
    incd SP   ;spf + 5 res
    ld A[SP]
    dec SP    ;spf + 4 res l
    ld B[SP]
    decd SP   ;spf + 0 
    decw SP   ;spf - 2 [arg 1]
    st A[SP]
    dec SP 
    st B[SP]  ;spf - 3 [arg 0]
    
    inc SP
    incw SP

    rts


fnPrintHex:
    decw SP
    ld A[SP]
    incw SP
    psh A
    
    shr4
    cmp A#0xa
    bcs #.letter
    add A#'0'
    st A&0x2000
    
    pop A
    and A#0xf
    cmp A#0xa
    bcs #.letter
    add A#'0'
    st A&0x2000
    
    rts

.letter:
    add A#0x7
    rts

fnToDec:
    rts

