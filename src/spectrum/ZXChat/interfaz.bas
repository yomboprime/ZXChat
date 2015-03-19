

' TODO:
' parsear tramas IPD
' Ver que pasa cuando hay network error

#include "entradaSalidaTexto.bas"

border 2 : paper 0 : ink 7: flash 0 : bright 0 : over 0
cls

dim i as uinteger
dim j as uinteger


print "Pulse tecla..."

tecla = ""
while tecla = ""
	tecla = inkey$
end while
while inkey$ <> ""
end while

cls 

dim strUrl as string
dim tamUrl as uinteger
dim y0 as integer
dim x1 as integer
dim y1 as integer
dim numScrolls as integer
dim cadenaEntrada as string
dim tamRespuesta as uinteger
dim timestamp as string

timestamp = "0"
strUrl = ""
y0 = 0
cadenaEntrada = ""
while cadenaEntrada <> " "

	cadenaEntrada = leerCadenaEntrada( y0 )
	
	if cadenaEntrada = " " then
		goto finBucle
	end if
	
	if cadenaEntrada <> "" then
		'codifica url
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $20, "%20" )'espacio
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $21, "%21" )'!
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $28, "%28" )'(
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $29, "%29" )')
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $2C, "%2C" )',
		cadenaEntrada = reemplazarCadena( cadenaEntrada, $3F, "%3F" )'!

		' Envia mensaje
		strUrl = "192.168.0.196:8080/YomboServer/ZXChat/chat?accion=enviar&nick=YomboZXSpectrum&mensaje=" + cadenaEntrada
		tamUrl = copiarCadenaABufer( strUrl )
		tamRespuesta = peticionGet( tamUrl )
	end if

	if y0 < 0 then
		y0 = 0
	end if

	' Refresca chat

	while ( ( leerRegistroControl() band 1 ) = 0 )
	end while

	strUrl = "192.168.0.196:8080/YomboServer/ZXChat/chat?accion=listar&timestamp=" + timestamp
	tamUrl = copiarCadenaABufer( strUrl )
	tamRespuesta = peticionGet( tamUrl )

	if ( tamRespuesta > 0 ) then

		timestamp = ""
		i = 0
		j = tamRespuesta
		' Parsea timestamp
		while ( i < TAM_BUFER and i < tamRespuesta and bufer( i ) <> 13 )
			timestamp = timestamp + chr( bufer( i ) )
			i = i + 1
			j = j - 1
		end while
		
		imprimirCadenaBuferWrap( cast(uinteger,i), j, 0, y0, x1, y1 )
		y0 = y1 + 1

		'print "Recibidos bytes:";tamRespuesta
		'print "Timestamp=";timestamp;", ";len(timestamp)

	else
		imprimirCadenaWrap( "Network error.", 0, y0, 0, x1, y1 )
		y0 = y1 + 1
	end if

	finBucle:
	
end while


fin:
print "FIN"

function copiarCadenaABufer( cadena as string ) as integer
	dim tam as integer
	tam = len( cadena )
	dim i as integer
	for i = 0 to tam - 1
		bufer( i ) = code( cadena( i ) )
	next i
	' La cadena ha de estar terminada en 0
	bufer( tam ) = 0
	tam = tam + 1
	return tam
end function

function peticionGet( tam as uinteger ) as uinteger

	' Devuelve tam de los bytes recibidos en el bufer

	dim b0 as ubyte
	dim b1 as ubyte
	dim b02 as ubyte
	dim b12 as ubyte
	dim tam2 as uinteger

	b0 = cast( ubyte, tam band $00FF )
	b1 = cast( ubyte, ( tam shr 8 ) band $00FF )
	
	' Escribe instruccion
	escribirRegistroControl( 36 )

	while ( ( leerRegistroControl() band 2 ) = 0 )
	end while

	esperarNOP( 1 )
	escribirRegistroDatos( b0 )
	esperarNOP( 1 )
	escribirRegistroDatos( b1 )

	esperarNOP( 1 )
	for i = 0 to tam - 1
		escribirRegistroDatos( bufer( i ) )
	next i

	while ( ( leerRegistroControl() band 4 ) = 0 )
	end while

	b02 = leerRegistroDatos()
	b12 = leerRegistroDatos()
	
	tam2 = ((b12 shl 8 ) band $FF00) bor (b02 band $00FF)

	if ( tam2 > TAM_BUFER ) then
		tam2 = TAM_BUFER
	end if

	esperarNOP( 100 )

	if ( tam2 > 0 ) then
	
		' No se por que hace falta esta lectura pero si no los bytes se desplazan una posicion.
		leerRegistroDatos()

		for i = 0 to tam2 - 1
			bufer( i ) = leerRegistroDatos()
		next i

	end if

	return tam2

end function
	

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

