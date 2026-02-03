init:
    pshsw #0xff
    jsr #fnShiftLeft16
       
    jsr #fnPrintHex
    dec SP
    jsr #fnPrintHex
    
    hlt
    ;incw SP

    
    jsr #fnPrintHex
    
    ;dec SP
    ;ld A[SP]
    ;inc SP
    ;psh A
    ;jsr #fnPrintHex
    ;pop A
    ;jsr #fnPrintHex
    
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
    pshb #0x0
    pshw #0x0
    pshw #0x0
    
    decd SP 
    dec SP
    decw SP
    
    ld A[SP]
    dec SP
    ld B[SP]

    inc SP
    incw SP
    inc SP
    
    st B[SP]
    inc SP
    st A[SP]
    
    decw SP
    

    ;.loop:
    
    ;jmp #.loop

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

