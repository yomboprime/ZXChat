
#ifndef __PANTALLA__

#define __PANTALLA__

sub copiarPantalla()

    beep 0.1,10
    beep 0.1,15
    beep 0.1,20

    ' 0EAC = 3756
    ASM

        call 3756

    END ASM
    
    beep 0.1,10

end sub


#endif