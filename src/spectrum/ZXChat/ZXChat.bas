
#include "Yombonet.bas"
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
dim tecla as string

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
