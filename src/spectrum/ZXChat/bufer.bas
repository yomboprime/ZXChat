#ifndef __BUFER__

#define __BUFER__

#define TAM_BUFER 771
dim bufer( 0 to TAM_BUFER - 1) as ubyte

function copiarCadenaABufer( cadena as string ) as integer
	dim tam as integer
	tam = len( cadena )
	if tam > TAM_BUFER then
		tam = TAM_BUFER
	end if
	dim i as integer
	for i = 0 to tam - 1
		bufer( i ) = code( cadena( i ) )
	next i
	return tam
end function

#endif