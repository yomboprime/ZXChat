
SUB putChars(x AS UBYTE,y AS UBYTE, width AS UBYTE, height AS UBYTE, dataAddress AS UINTEGER)
    ' Copyleft Britlion. Feel free to use as you will. Please attribute me if you use this, however!
 
    ASM
    BLPutChar:
             LD      a,(IX+5)
             ;AND     31
             ld      l,a
             ld      a,(IX+7) ; Y value
             ld      d,a
             AND     24
             add     a,64 ; 256 BYTE "page" FOR screen - 256*64=16384. Change this IF you are working with a screen address elsewhere, such AS a buffer.
             ld      h,a
             ld      a,d
             AND     7
             rrca
             rrca
             rrca
             OR      l
             ld      l,a
 
    PUSH HL ; SAVE our address
 
    LD E,(IX+12) ; data address
    LD D,(IX+13)
    LD B,(IX+9) ; width
    PUSH BC ; SAVE our column count
 
    BLPutCharColumnLoop:
 
    LD B,(IX+11) ; height 
 
    BLPutCharInColumnLoop:
   
    ; gets screen address in HL, AND bytes address in DE. Copies the 8 bytes TO the screen
    ld a,(DE) ; First Row
    LD (HL),a
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; second Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Third Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Fourth Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Fifth Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Sixth Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Seventh Row
    
    INC DE
    INC H
    ld a,(DE)
    LD (HL),a ; Eigth Row
    
    INC DE ; Move TO NEXT data item.
    
    DEC B
    JR Z,BLPutCharNextColumn
    ;The following CODE calculates the address of the NEXT line down below current HL address.
    PUSH DE ; SAVE DE
             ld   a,l   
             AND  224   
             cp   224   
             jp   z,BLPutCharNextThird
 
    BLPutCharSameThird:
             ld   de,-1760
             ;and  a         
             add  hl,de      
             POP DE ; get our data point back.
             jp BLPutCharInColumnLoop
 
    BLPutCharNextThird:
             ld   de,32      
             ;and  a
             add  hl,de   
             POP DE ; get our data point back.
    JP BLPutCharInColumnLoop
 
    BLPutCharNextColumn:
    POP BC
    POP HL
    DEC B
    JP Z, BLPutCharsEnd
 
    INC L   ; Note this would normally be Increase HL - but block painting should never need TO increase H, since that would wrap around.
    PUSH HL
    PUSH BC
    JP BLPutCharColumnLoop
 
BLPutCharsEnd:
    END ASM
 
END SUB
    
    
    
    '*********************************************************************************************************
    
    
SUB paint (x AS UBYTE,y AS UBYTE, width AS UBYTE, height AS UBYTE, attribute AS UBYTE)
    ' Copyleft Britlion. Feel free to use as you will. Please attribute me if you use this, however!
 
    ASM
    ld      a,(IX+7)   ;ypos
    rrca
    rrca
    rrca               ; Multiply by 32
    ld      l,a        ; Pass TO L
    AND     3          ; Mask with 00000011
    add     a,88       ; 88 * 256 = 22528 - start of attributes. Change this IF you are working with a buffer OR somesuch.
    ld      h,a        ; Put it in the High BYTE
    ld      a,l        ; We get y value *32
    AND     224        ; Mask with 11100000
    ld      l,a        ; Put it in L
    ld      a,(IX+5)   ; xpos 
    add     a,l        ; Add it TO the Low BYTE
    ld      l,a        ; Put it back in L, AND we're done. HL=Address.
    
    push HL            ; SAVE address
    LD A, (IX+13)      ; attribute
    LD DE,32
    LD c,(IX+11)       ; height
    
    BLPaintHeightLoop: 
    LD b,(IX+9)        ; width
    
    BLPaintWidthLoop:
    LD (HL),a          ; paint a character
    INC L              ; Move TO the right (Note that we only would have TO inc H IF we are crossing from the right edge TO the left, AND we shouldn't be needing to do that)
    DJNZ BLPaintWidthLoop
    
    BLPaintWidthExitLoop:
    POP HL             ; recover our left edge
    DEC C
    JR Z, BLPaintHeightExitLoop
    
    ADD HL,DE          ; move 32 down
    PUSH HL            ; SAVE it again
    JP BLPaintHeightLoop
 
    BLPaintHeightExitLoop:
    
    END ASM
END SUB


    
    '*********************************************************************************************************
    

    
 
SUB paintData (x AS UBYTE,y AS UBYTE, width AS UBYTE, height AS UBYTE, address AS UINTEGER)
    ' Copyleft Britlion. Feel free to use as you will. Please attribute me if you use this, however!
 
    ASM
    ld      a,(IX+7)   ;ypos
    rrca
    rrca
    rrca               ; Multiply by 32
    ld      l,a        ; Pass TO L
    AND     3          ; Mask with 00000011
    add     a,88       ; 88 * 256 = 22528 - start of attributes. Change this IF you are working with a buffer OR somesuch.
    ld      h,a        ; Put it in the High BYTE
    ld      a,l        ; We get y value *32
    AND     224        ; Mask with 11100000
    ld      l,a        ; Put it in L
    ld      a,(IX+5)   ; xpos 
    add     a,l        ; Add it TO the Low BYTE
    ld      l,a        ; Put it back in L, AND we're done. HL=Address.
    
    push HL            ; SAVE address
    LD D, (IX+13)
    LD E, (IX+12)
    LD c,(IX+11)       ; height
    
    BLPaintDataHeightLoop: 
    LD b,(IX+9)        ; width
    
    BLPaintDataWidthLoop:
    LD a,(DE)
    LD (HL),a          ; paint a character
    INC L              ; Move TO the right (Note that we only would have TO inc H IF we are crossing from the right edge TO the left, AND we shouldn't be needing to do that)
    INC DE
    DJNZ BLPaintDataWidthLoop
                        
    
    BLPaintDataWidthExitLoop:
    POP HL             ; recover our left edge
    DEC C
    JR Z, BLPaintDataHeightExitLoop
    PUSH DE
    LD DE,32
    ADD HL,DE          ; move 32 down
    POP DE
    PUSH HL            ; SAVE it again
    JP BLPaintDataHeightLoop
 
    BLPaintDataHeightExitLoop:
    
    END ASM
END SUB   
 
 
     '*********************************************************************************************************
     
     