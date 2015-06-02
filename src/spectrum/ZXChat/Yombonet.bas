#ifndef __YOMBONET__

#define __YOMBONET__

#include "bufer.bas"


' Funciones de instruccion

function obtenerRevisionFirmware() as uinteger

	' Devuelve tam del mensaje recibido en el bufer

	esperarYombonetLista()

	' Escribe instruccion
	escribirRegistroControl( 00000000b )

	return recibirCadena()

end function

function activarDesactivarDebug( activar as ubyte ) as ubyte

	if activar <> 0 then
		activar = 1
	end if

	escribirRegistroDatos( activar )

	esperarYombonetLista()

	' Escribe instruccion
	escribirRegistroControl( 00000001b )

	esperarYombonetLista()

	return leerCodigoError()

end function

function conectarAWiFi( tamPeticion as uinteger ) as ubyte

	' En el bufer se debe haber escrito SSID PASSWORD
	' tamPeticion es el tamanyo de esa cadena

	esperarYombonetLista()

	' Escribe instruccion
	escribirRegistroControl( 00000010b )

	transmitirCadena( tamPeticion )

	esperarYombonetLista()

	return leerCodigoError()

end function


function peticionGetPost( getNoPost as ubyte, tamPeticion as uinteger, byref tamRespuesta as uinteger ) as ubyte

	' Devuelve codigo de error.
	' en tamRespuesta se devuelve el tam de los bytes recibidos en el bufer

	dim instruccion as ubyte
	instruccion = 00000100b ' GET
	if getNoPost = 0 then
		instruccion = 00000101b ' POST
	end if

	esperarYombonetLista()

	' Escribe instruccion
	escribirRegistroControl( instruccion )

	transmitirCadena( tamPeticion )

	tamRespuesta = recibirCadena()

	return leerCodigoError()

end function


' Funciones intermedias

function leerCodigoError() as ubyte
	return ( leerRegistroControl() shr 4 ) band $0F
end function

function esperarYombonetLista()
	while ( ( leerRegistroControl() band 1 ) = 0 )
	end while
end function

function recibirCadena() as uinteger

	dim b0 as ubyte
	dim b1 as ubyte
	dim tamRespuesta as uinteger

	' Espera Yombonet listo para transmitir
	while ( ( leerRegistroControl() band 4 ) = 0 )
	end while

	b0 = leerRegistroDatos()
	b1 = leerRegistroDatos()
	
	tamRespuesta = ( ( cast( uinteger, b1 ) shl 8 ) band $FF00) bor ( cast( uinteger, b0 ) band $00FF)

	if ( tamRespuesta > TAM_BUFER ) then
		tamRespuesta = TAM_BUFER
	end if

	esperarNOP( 100 )

	if ( tamRespuesta > 0 ) then
	
		' No se por que hace falta esta lectura pero si no los bytes se desplazan una posicion.
		leerRegistroDatos()

		for i = 0 to tamRespuesta - 1
			bufer( i ) = leerRegistroDatos()
		next i

	end if
	
	return tamRespuesta

end function

sub transmitirCadena( tamCadena as uinteger )

	dim b0 as ubyte
	dim b1 as ubyte

	b0 = cast( ubyte, tamCadena band $00FF )
	b1 = cast( ubyte, ( tamCadena shr 8 ) band $00FF )

	' Espera Yombonet listo para recibir
	while ( ( leerRegistroControl() band 2 ) = 0 )
	end while

	esperarNOP( 1 )
	escribirRegistroDatos( b0 )
	esperarNOP( 1 )
	escribirRegistroDatos( b1 )
	esperarNOP( 1 )
	for i = 0 to tamCadena - 1
		escribirRegistroDatos( bufer( i ) )
	next i

end sub


' Funciones de bajo nivel

function FASTCALL escribirRegistroControl( byval valor as ubyte )
	' fastcall functions always receive the 1st parameter
	' in accumulator (if byte)
	asm
		out (65343), a
	end asm
end function

function FASTCALL escribirRegistroDatos( byval valor as ubyte )
	' fastcall functions always receive the 1st parameter
	' in accumulator (if byte)
	asm
		out (65407), a
	end asm
end function

function FASTCALL leerRegistroControl as ubyte
	' byte values are returned in accumulator
	asm
		in a, (65343)
	end asm
end function

function FASTCALL leerRegistroDatos( ) as ubyte
	' byte values are returned in accumulator
	asm
		in a, (65407)
	end asm
end function

sub esperarNOP( numNops as ubyte )
	dim i as ubyte
	for i = 1 to numNops
		asm
			nop
		end asm
	next i
end sub

 
#endif 


