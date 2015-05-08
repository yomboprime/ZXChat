
#define VERSION "r1"

#include "Yombonet.bas"
#include "entradaSalidaTexto.bas"
#include "variablesGlobales.bas"
#include "menuZXChat.bas"

dim i as uinteger
dim j as uinteger
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
dim intentoConexion as ubyte



Menu:

border 2 : paper 0 : ink 7: flash 0 : bright 0 : over 0
cls


imprimirCadenaWrap( "ZXChat " + VERSION, 0, 0, 0, x1, y1 )
imprimirCadenaWrap( "1) Comenzar", 0, y1 + 2, 0, x1, y1 )
imprimirCadenaWrap( "2) Cambiar nick", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "3) Configurar WiFi", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "4) Cambiar servidor", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "5) Testear Yombonet", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "6) Salir", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaWrap( "Seleccione opcion...", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
tecla = leerCadenaEntrada( "", y1 )

if tecla = "1" then
	if strSSID = "" then
		imprimirCadenaWrap( "Debe especificar una SSID para conectar a WiFi. Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if

	intentoConexion = 1
	goto ConexionWiFi

end if
if tecla = "5" then goto TestearYombonet : end if
if tecla = "6" then goto Fin : end if

goto Menu



TestearYombonet:

imprimirCadenaWrap( "Accediendo a Yombonet para obtener revision del firmware...", 0, y1 + 1, 0, x1, y1 )

tamRespuesta = obtenerRevisionFirmware()

imprimirCadenaWrap( "Respuesta obtenida.", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaWrap( "Firmware: ", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaBuferWrap( 0, tamRespuesta, 0, y1 + 1, x1, y1 )

imprimirCadenaWrap( "Pulse una tecla...", 0, y1 + 1, 0, x1, y1 )
esperarTecla()

goto Menu


ConexionWiFi:

i = copiarCadenaABufer( strSSID + " " + strWifiPassword )
imprimirCadenaWrap( "Conectando a Wifi: " + strSSID + "...", 0, y1 + 1, 0, x1, y1 )
codigoError = conectarAWiFi( i )

if codigoError = 0 then
	imprimirCadenaWrap( "Conectado a Wifi correctamente.", 0, y1 + 1, 0, x1, y1 )
	pause 75
else
	imprimirCadenaWrap( "Error al conectar a WiFi: " + str( codigoError ), 0, y1 + 1, 0, x1, y1 )
	intentoConexion = intentoConexion + 1
	if intentoConexion > 3 then
		imprimirCadenaWrap( "Fin reintentos conexion " + str( codigoError ), 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if
	goto ConexionWiFi
end if

' Bucle de recepcion y envio de mensajes
cls
y0 = 0
timestamp = "0"
cadenaEntrada = ""

imprimirCadenaWrap( "Escriba un mensaje para enviar o pulse solo enter para recibir. Escriba un solo espacio para volver al menu.", 0, y0, 0, x1, y0 )
y0 = y0 + 1

while cadenaEntrada <> " "

	cadenaEntrada = leerCadenaEntrada( "", y0 )

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
		strUrl = strIPServidor + strUrlServidor + "?accion=enviar&nick=YomboZXSpectrum&mensaje=" + cadenaEntrada
		tamUrl = copiarCadenaABufer( strUrl )
		codigoError = peticionGet( tamUrl, tamRespuesta )
		if codigoError <> 0 then
			imprimirCadenaWrap( "Codigo de error en envio: " + str( codigoError ), 0, y0, 0, x1, y1 )
			y0 = y1 + 1
		end if
	end if

	if y0 < 0 then
		y0 = 0
	end if

	' Refresca chat
	strUrl = strIPServidor + strUrlServidor + "?accion=listar&timestamp=" + timestamp
	tamUrl = copiarCadenaABufer( strUrl )
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

	else
		' Pone borde amarillo (error)
		border 6
		imprimirCadenaWrap( "Codigo de error: " + str( codigoError ), 0, y0, 0, x1, y1 )
		y0 = y1 + 1
	end if

	finBucle:

end while

goto Menu

Fin:
print "FIN"
