
#include "entradaSalidaTexto.bas"

border 2 : paper 0 : ink 7: flash 0 : bright 0 : over 0
cls

dim i as uinteger
dim j as uinteger
dim strIPServidor as string
dim strUrl as string
dim tamUrl as uinteger
dim y0 as integer
dim x1 as integer
dim y1 as integer
dim numScrolls as integer
dim cadenaEntrada as string
dim tamRespuesta as uinteger
dim timestamp as string
dim codigoError as ubyte

dim debugactivado as ubyte
debugactivado = 0

imprimirCadenaWrap( "Pulse tecla...", 0, 0, 0, x1, y1 )
tecla = "": while tecla = "" : tecla = inkey$ : end while : while inkey$ <> "" : end while

tamRespuesta = obtenerRevisionFirmware()

imprimirCadenaWrap( "Firm:", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaBuferWrap( 0, tamRespuesta, 0, y1 + 1, x1, y1 )

imprimirCadenaWrap( "Pulse tecla2...", 0, y1 + 1, 0, x1, y1 )
tecla = "": while tecla = "" : tecla = inkey$ : end while : while inkey$ <> "" : end while

i = copiarCadenaABufer( "yombo ninobravo" )
imprimirCadenaBuferWrap( 0, i, 0, y1 + 1, x1, y1 )
codigoError = conectarAWiFi( i )



imprimirCadenaWrap( "Resultado conexion:", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( str( codigoError ), x1, y1, 0, x1, y1 )

imprimirCadenaWrap( "Pulse tecla3...", 0, y1 + 1, 0, x1, y1 )
tecla = "": while tecla = "" : tecla = inkey$ : end while : while inkey$ <> "" : end while
cls

strIPServidor = "81.35.6.85"
timestamp = "0"
strUrl = ""
y0 = 0
cadenaEntrada = ""
while cadenaEntrada <> " "

	cadenaEntrada = leerCadenaEntrada( y0 )

	border 0

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
		strUrl = strIPServidor + ":8080/YomboServer/ZXChat/chat?accion=enviar&nick=YomboZXSpectrum&mensaje=" + cadenaEntrada
		tamUrl = copiarCadenaABufer( strUrl )
		codigoError = peticionGet( tamUrl, tamRespuesta )
	end if

	if y0 < 0 then
		y0 = 0
	end if

	' Refresca chat
	strUrl = strIPServidor + ":8080/YomboServer/ZXChat/chat?accion=listar&timestamp=" + timestamp
	tamUrl = copiarCadenaABufer( strUrl )
	
	'Hace la peticion
	codigoError = peticionGet( tamUrl, tamRespuesta )

	if ( codigoError = 0 ) then
	
		' Pone borde verde
		border 4

		timestamp = ""
		i = 0
		j = tamRespuesta
		' Parsea timestamp
		while ( i < TAM_BUFER and i < tamRespuesta and bufer( i ) <> 13 )
			timestamp = timestamp + chr( bufer( i ) )
			i = i + 1
			j = j - 1
		end while

		' Avanza el fin de linea
		if ( i < (TAM_BUFER + 1) and i < (tamRespuesta + 1) and bufer( i ) = 13 and bufer( i + 1 ) = 10 ) then
			i = i + 2
			j = j - 2
		end if

		'Imprime mensajes
		if j > 0 then 
			imprimirCadenaBuferWrap( cast(uinteger,i), j, 0, y0, x1, y1 )
			y0 = y1 + 1
		end if

		'print "Recibidos bytes:";tamRespuesta
		'print "Timestamp=";timestamp;", ";len(timestamp)

	else
		' Pone borde amarillo (error)
		border 6
		imprimirCadenaWrap( "Codigo de error: " + str( codigoError ), 0, y0, 0, x1, y1 )
		y0 = y1 + 1
	end if

	finBucle:

end while


fin:
print "FIN"

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
	
	tamRespuesta = ((b1 shl 8 ) band $FF00) bor (b0 band $00FF)

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

	codigoError = leerCodigoError()
	return codigoError

end function


function peticionGet( tamPeticion as uinteger, byref tamRespuesta as uinteger ) as ubyte

	' Devuelve codigo de error.
	' en tamRespuesta se devuelve el tam de los bytes recibidos en el bufer

	esperarYombonetLista()

	' Escribe instruccion
	escribirRegistroControl( 00000100b )

	transmitirCadena( tamPeticion )

	tamRespuesta = recibirCadena()

	return leerCodigoError()

end function

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

