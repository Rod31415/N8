main:
    pshw #.str
    jsr #fnPrintStr
    jmp #end

.str: .string "##  N8 CPU  ##",0xa," - hola - ",0x0
fnPrintStr:
    decw SP
    popw &vrPrintStr

.loop:
    ld A%vrPrintStr
    cmp A#0x0
    jpz #.return
    st A&0x2000

    ld A&vrPrintStr
    add A#1
    st A&vrPrintStr

    ld A&vrPrintStr+1
    adc A#0
    st A&vrPrintStr+1

    jmp #.loop
.return:
    incd SP
    rts


end:
    jmp #end

.org 0x4000

vrPrintStr:
    .res 2
