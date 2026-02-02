init:
        jsr #ansi_clearscreen
        jsr #ansi_home
        ld A#<.str
        ld B#>.str
        jsr #printstring
        jmp #slash
.str:
                .string '+========Monitor========+',0xa
                .string '|  0x4000   -  BASIC    |',0x0

slash:
        
        ld A#0xa
        st A&0x2000
        ld A#'\'
        st A&0x2000
        ld A#0xa
        st A&0x2000
        jsr #clearvars
        jmp #maincin
clearvars:
        ld A#<hIn
        ld B#>hIn
        st A&stringPtr
        st B&stringPtr+1
        ld A#0

        st A&xamhex
        st A&xamhex+1
        st A&sthex
        st A&sthex+1
        st A&state
        rts
   
maincin:
        ld A&0x2000
        cmp A#0
        jnz #.key
        jmp #maincin
.key:
        
        st A&0x2000
        st A%stringPtr
        
        cmp A#0xa
        beq #clearvars
        jpz #mainkey

        ld A&stringPtr
        add A#1
        st A&stringPtr
        jmp #maincin

mainkey:
        
        ld A%stringPtr
        
        cmp A#0
        jnz #.put
        
        jmp #mainkey
.put:
        cmp A#0xa
        jpz #mon
        cmp A#'R'
        jpz #runPgm
        cmp A#':'
        jpz #wrtblockf
        cmp A#'.'
        psh A
        beq #setxamblock
        pop A
        jsr #isHex
        cmp A#0x0
        bne #readhexf
        ld A&stringPtr
        add A#1
        st A&stringPtr
        jmp #mainkeyloop
runPgm:
        ld A&xamhex
        st A&runAddr
        ld A&xamhex+1
        st A&runAddr+1
        jsr #clearvars
        jmp &runAddr
setxamblock:
        ld A#0x1
        st A&state
        ld A&xamhex
        st A&sthex
        ld A&xamhex+1
        st A&sthex+1
        ld A#0x0
        st A&xamhex
        st A&xamhex+1
        rts
wrtblockf:
        ld A#0x2
        st A&state
        ld A&xamhex
        st A&sthex
        ld A&xamhex+1
        st A&sthex+1
        ld A#0x0
        st A&xamhex
        st A&xamhex+1
.keyloop:
        ld A&stringPtr
        add A#1
        st A&stringPtr
        ld A%stringPtr
        
        cmp A#0xa
        jpz #slash
        cmp A#' '
        jpz #.writeto
        jsr #isHex
        cmp A#0x0
        bne #readhexf

        jmp #.keyloop
.writeto:
        
        ld A&xamhex
        st A%sthex
        ld A&sthex
        add A#1
        st A&sthex
        ld A&sthex+1
        adc A#0
        st A&sthex+1
        
        ld A#0x0
        st A&xamhex
        st A&xamhex+1
        jmp #.keyloop
mon:
        ld A&state
        cmp A#0x0
        beq #xamf
        cmp A#0x1
        beq #xamblockf
        jmp #slash


xamf:
        ld A&xamhex+1
        jsr #printhexf
        ld A&xamhex
        jsr #printhexf
        ld A#':'
        st A&0x2000
        ld A%xamhex
        jsr #printhexf
        rts

xamblockf:
        jsr #.addr
.loop:
        ld A#' '
        st A&0x2000
        ld A%sthex
        jsr #printhexf
        
        ld A&sthex
        add A#1
        st A&sthex
        ld A&sthex+1
        adc A#0x0
        st A&sthex+1
        cmp A&xamhex+1
        jpz #.nextComp

        ld A&sthex
        and A#0x0f
        
        beq #.addr

        jmp #.loop
.nextComp:
        ld A&sthex
        ;sub A#1

        cmp A&xamhex
        jpz #return
        jmp #.loop
.addr:
        ld A#0xa
        st A&0x2000
        ld A&sthex+1
        jsr #printhexf
        ld A&sthex
        jsr #printhexf
        ld A#':'
        st A&0x2000
        rts
        
isHex:
      cmp A#'0'
      jnc #retFalse
      cmp A#'9'+1
      jnc #retTrue
      cmp A#'A'
      jnc #retFalse
      cmp A#'F'+1
      jnc #retTrue
      jmp #retFalse
retTrue:

        rts
retFalse:
        ld A#0x0
        rts

readhexf:
        
        ld A%stringPtr
        cmp A#'A'-1
        bcs #readhex_letter
        sub A#'0'
        st A&hexRef
        ld B#4
        st B&index
        jsr #passfour
        ld A&hexRef
        ld B&xamhex
        
        or &axamhex
        rts
readhex_letter:
        sub A#7
        rts

passfour:
        ld A&xamhex+1
        shl
        st A&xamhex+1
        ld A&xamhex
        shl
        st A&xamhex
        ld A&xamhex+1
        adc A#0
        st A&xamhex+1
        ld A&index
        sub A#1
        st A&index
        
        ;cmp A#0x0
        jpz #return
        
        jmp #passfour
        

printhexf:
        st A&hexNumberPrint
        and A#0xf ; LSB Nibble 
        cmp A#0xa ; is letter
        bcs #printhex_letter ; yes

        add A#'0' ;add number ascii offset
        st A&hexNumberPrint+1

        ld A&hexNumberPrint
        shr4
        cmp A#0xa ; is letter
        bcs #printhex_letter ; yes

        add A#'0' ;add number ascii offset
        st A&0x2000
        ld A&hexNumberPrint+1
        st A&0x2000
        rts

printhex_letter:
        add A#7 ;add letter ascii offset
        rts


printstring:
        st A&stringPtr
        st B&stringPtr+1
string_loop:
        ld A%stringPtr
        cmp A#0
        jpz #return
        st A&0x2000
        ld A&stringPtr
        add A#1
        st A&stringPtr
        jmp #string_loop


return:
        rts

ansi_control:
        ld A#0x1b
        st A&0x2000
        ld A#0x5b
        st A&0x2000
        rts

ansi_move:
        jsr #ansi_control
        ld A&ansi_aux
        add A#'0'
        st A&0x2000
        rts
ansi_right:
        st A&ansi_aux
        jsr #ansi_move
        ld A#'G'
        st A&0x2000

ansi_clearscreen:
        jsr #ansi_control
        ld A#'2'
        st A&0x2000
        ld A#'J'
        st A&0x2000
        rts
ansi_home:
        jsr #ansi_control
        ld A#'H'
        st A&0x2000
        rts

.org 0x4000
basicf:

        ld A#<.str
        ld B#>.str
        jsr #printstring
.keyloop:
        ld A&0x2000
        cmp A#0x0
        bne #.recvchar
        jmp #.keyloop
.recvchar:
        psh A
        cmp A#'9'+1
        bcc #.below9
        pop A
        cmp A#0xA
        beq #.enter
        rts
.below9:
        cmp A#'0'
        bcs #.isnum
        rts
.isnum:
        st A&0x2000
        jsr #built_read_dec
        rts
.enter:
.asda:
        jmp .asda
        ld A#' '
        st A&0x2000
        
        ld A&dec_linebuffer1
        cmp A#0x0
        jpz #.zero1
        jmp #.normal
.zero1:
        ld A&dec_linebuffer1+1
        cmp A#0x0
        jpz #.zero2
        jmp #.normal
.zero2:
        ld A#0x0
        st A&basic_num
        st A&basic_num+1
.normal:
        ;ld A&dec_linebuffer1+1
        ;jsr #printhexf

        ;ld A&dec_linebuffer1
        ;jsr #printhexf
        
        
        
        pshb &basic_num
        pshb &dec_linebuffer1

        jsr #built_multiply8

        popw &basic_num

        ;ld A&basic_num+1
        ;jsr #printhexf

        ;ld A&basic_num
        ;jsr #printhexf


        ld A#0x0
        st A&dec_linebuffer1
        st A&dec_linebuffer1+1

        


        rts


.end:
        jmp #slash
.str:
                .string '  Eneido BASIC ',0xA,0x0


built_shift16:

        popw &stackshift16
        popw &numbershift16

        ld A&numbershift16+1
        shl 
        st A&numbershift16+1

        ld A&numbershift16
        shl
        st A&numbershift16

        ld A&numbershift16+1
        adc A#0x0
        st A&numbershift16+1

        pshw &numbershift16
        
        pshw &stackshift16
        rts
        

built_add16:

        popw &stackadd16
        popw &number1add16
        popw &number2add16

        ld A&number2add16+1
        add A&number1add16+1
        st A&resadd16+1
        ld A#0x0
        adc A#0x0
        st A&resadd16+2

        ld A&number2add16
        add A&number1add16
        st A&resadd16
        ld A&resadd16+1
        adc A#0x0
        st A&resadd16+1


        pshw &resadd16
        pshb &resadd16+2
        

        pshw &stackadd16

        rts

built_multiply8:

        ld A#0x0
        st A&resmultiply8
        st A&resmultiply8+1
        st A&copiedmultiply8+1
        ld A#0x8
        st A&itermultiply8
        
        popw &stackmultiply8
        
        popb &copiedmultiply8
        popb &refmultiply8


        ld A#0xA
        st A&0x2000



.loop:
        nop

        ld A&itermultiply8
        cmp A#0x0
        jpz #.end


        sub A#0x1
        st A&itermultiply8
        

        ld A&refmultiply8
        and A#0x1
        cmp A#0x1
        beq #.sum

        ld A&refmultiply8
        shr
        st A&refmultiply8

        pshw &copiedmultiply8
        jsr #built_shift16
        popw &copiedmultiply8


        jmp #.loop
.sum:   
        pshw &copiedmultiply8
        pshw &resmultiply8
        jsr #built_add16
        pop A
        popw &resmultiply8

        rts
        
.end:

        pshw &resmultiply8

        

        pshb &stackmultiply8+1
        pshb &stackmultiply8


        rts

built_multiply16:
        rts


built_read_dec:
        psh A
        ld A#4
        st A&dec_iteration
        jsr #.shiftloop

        ld A&dec_linebuffer1+1
        add A&dec_linebuffer2+1
        st A&dec_linebuffer1+1

        ld A&dec_linebuffer1
        add A&dec_linebuffer2
        st A&dec_linebuffer1

        ld A&dec_linebuffer1+1
        adc A#0x0
        st A&dec_linebuffer1+1

        pop A
        sub A#'0'
        st A&dec_iteration
        ld A&dec_linebuffer1
        
        add A&dec_iteration
        st A&dec_linebuffer1
        
        ld A&dec_linebuffer1+1
        adc A#0x0
        st A&dec_linebuffer1+1
        rts

.shiftloop:
        sub A#0x1
        st A&dec_iteration
        jpz #return

        cmp A#0x2
        beq #.oneshift

        shl A&dec_linebuffer1+1
        st A&dec_linebuffer1+1

        shl A&dec_linebuffer1
        st A&dec_linebuffer1

        ld A&dec_linebuffer1+1
        adc A#0x0
        st A&dec_linebuffer1+1

        
        ld A&dec_iteration
        jmp #.shiftloop
.oneshift:
        ld A&dec_linebuffer1
        st A&dec_linebuffer2
        ld A&dec_linebuffer1+1
        st A&dec_linebuffer2+1
        rts

.org 0x8000

;;;;;;; BUILT-DEC TO BIN ;;;;;;;
dec_linebuffer1:
        .res 2
dec_linebuffer2:
        .res 2
dec_iteration:
        .res 1

;;;;;;; BUILT-SHIFT16 ;;;;;;;;;

stackshift16:
        .res 2
numbershift16:
        .res 2

;;;;;;; BUILT-ADD16 ;;;;;;;;;

stackadd16:
        .res 2
number1add16:
        .res 2
number2add16:
        .res 2
resadd16:
        .res 3


;;;;;;; BUILT-MULTIPLY8 ;;;;;;;;;

stackmultiply8:
        .res 2
refmultiply8:
        .res 1
copiedmultiply8:
        .res 2
resmultiply8:
        .res 2
itermultiply8:
        .res 1

;;;;;;; BASIC ;;;;;;;;;
basic_num:
        .res 2
basic_state:
        .res 1
basic_write_buffer:
        .res 1

.org 0x8100
ansi_aux:
        .res 1
runAddr:
        .res 2
xamhex:
        .res 2
sthex:
        .res 2
hexNumberPrint:
        .res 2
hexRef:
        .res 1
stringPtr:
        .res 2
index:
        .res 1
state:
        .res 1

.org 0x8120
hIn:
        .res 1
end:


        
