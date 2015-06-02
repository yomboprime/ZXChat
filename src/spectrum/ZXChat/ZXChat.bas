
#define VERSION "r1"

#include "Yombonet.bas"
#include "entradaSalidaTexto.bas"
#include "variablesGlobales.bas"

dim i as uinteger
dim j as uinteger
dim tamUrl as uinteger
dim strUrl as string
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
imprimirCadenaWrap( "2) Cambiar nick (" + strNick + ")", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "3) Configurar WiFi", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "4) Cambiar servidor", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "5) Testear Yombonet", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "6) Salir", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaWrap( "Pulse opcion y enter...", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
tecla = leerCadenaEntrada( "", y1 )

if tecla = "1" then
	if strSSID = "" then
		imprimirCadenaWrap( "Debe especificar un SSID para conectar a WiFi. Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if

	intentoConexion = 1
	goto ConexionWiFi

end if
if tecla = "2" then goto CambiarNick : end if
if tecla = "3" then goto ConfigurarWiFi : end if
if tecla = "4" then goto CambiarServidor : end if
if tecla = "5" then goto TestearYombonet : end if
if tecla = "6" then goto Fin : end if

goto Menu




CambiarNick:

imprimirCadenaWrap( "Escriba su nick y pulse enter: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strNick = leerCadenaEntrada( strNick, y1 )

goto Menu





ConfigurarWiFi:
cls
y1 = 0
imprimirCadenaWrap( "Escriba el SSID de la WiFi: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strSSID = leerCadenaEntrada( strSSID, y1 )

imprimirCadenaWrap( "Escriba la contrasena de la WiFi: ", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
strWifiPassword = leerCadenaEntrada( strWifiPassword, y1 )

goto Menu





CambiarServidor:


cls

imprimirCadenaWrap( "1) Usar YomboServer", 0, 0, 0, x1, y1 )
imprimirCadenaWrap( "2) Usar servidor manual", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "3) Volver", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaWrap( "Pulse opcion y enter...", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
tecla = leerCadenaEntrada( "", y1 )

if tecla = "1" then 
	ubUsarYomboServer = 1
	goto Menu
end if

if tecla = "3" then 
	goto Menu
end if

if tecla <> "2" then 
	goto CambiarServidor
end if

' Configuracion manual del servidor
ubUsarYomboServer = 0

imprimirCadenaWrap( "Escriba nombre o IP del servidor: ", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
strIPServidor = leerCadenaEntrada( strIPServidor, y1 )

imprimirCadenaWrap( "Escriba URL de ZXChat en el servidor: ", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
strUrlServidor = leerCadenaEntrada( strUrlServidor, y1 )

goto Menu




TestearYombonet:

imprimirCadenaWrap( "Accediendo a Yombonet para obtener revision del firmware...", 0, y1 + 1, 0, x1, y1 )

tamRespuesta = obtenerRevisionFirmware()

imprimirCadenaWrap( "Respuesta obtenida:", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaBuferWrap( 0, tamRespuesta, 0, y1 + 1, x1, y1 )

imprimirCadenaWrap( "Pulse una tecla...", 0, y1 + 1, 0, x1, y1 )
esperarTecla()

goto Menu




ConexionWiFi:

i = copiarCadenaABufer( strSSID + " " + strWifiPassword )
if intentoConexion = 1 then
	imprimirCadenaWrap( "Conectando a Wifi: " + strSSID + "...", 0, y1 + 1, 0, x1, y1 )
else
	imprimirCadenaWrap( "Conectando a Wifi: " + strSSID + ", reintento " + str( intentoConexion ) + " de 3...", 0, y1 + 1, 0, x1, y1 )
end if

codigoError = conectarAWiFi( i )

if codigoError <> 0 then
	imprimirCadenaWrap( "Error al conectar a WiFi, codigo: " + str( codigoError ), 0, y1 + 1, 0, x1, y1 )
	intentoConexion = intentoConexion + 1
	if intentoConexion > 3 then
		imprimirCadenaWrap( "Fin reintentos conexion. Pulse una tecla." + str( codigoError ), 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if
	goto ConexionWiFi
end if

imprimirCadenaWrap( "Conectado a Wifi correctamente.", 0, y1 + 1, 0, x1, y1 )


' Conexion a Yomboserver
if ubUsarYomboServer = 1 then

	imprimirCadenaWrap( "Conectando a YomboServer en yombo.org...", 0, y1 + 1, 0, x1, y1 )

	strUrl = "yombo.org/wp-content/datos/yomboserver/yomboServer.html"
	tamUrl = copiarCadenaABufer( strUrl )
	codigoError = peticionGetPost( 1, tamUrl, tamRespuesta )

	if ( codigoError = 0 ) then
		' Parsea IP
		' Busca //
		strIPServidor = ""
		i = 0
		while ( i < TAM_BUFER - 1 and i < tamRespuesta - 1 and not ( bufer( i ) = code("/") and bufer( i + 1 ) = code("/") ) )
			i = i + 1
		end while

		if i < TAM_BUFER - 1 and i < tamRespuesta - 1 then
		
			i = i + 2
		
			' Busca dos puntos :
			j = i
			while ( j < TAM_BUFER and j < tamRespuesta and bufer( j ) <> code(":") )
				j = j + 1
			end while
			if j < TAM_BUFER and j < tamRespuesta then
				' Copia IP que esta entre caracter i y j:
				while i < j
					strIPServidor = strIPServidor + chr( bufer( i ) )
					i = i + 1
				end while
			end if
		end if
		
		if strIPServidor = "" then
			imprimirCadenaWrap( "YomboServer esta offline :( Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
			esperarTecla()
			goto Menu
		else
			imprimirCadenaWrap( "YomboServer esta online :)", 0, y1 + 1, 0, x1, y1 )
		end if
	else
		imprimirCadenaWrap( "Error al conectar, codigo: " + str( codigoError ) + ". Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if
end if


imprimirCadenaWrap( "Pulse una tecla...", 0, y1 + 2, 0, x1, y1 )
esperarTecla()

' Bucle de recepcion y envio de mensajes (el chat en si):
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
		strUrl = strIPServidor + strUrlServidor + "?accion=enviar&nick=" + strNick + "&mensaje=" + cadenaEntrada
		tamUrl = copiarCadenaABufer( strUrl )
		codigoError = peticionGetPost( 1, tamUrl, tamRespuesta )
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
	codigoError = peticionGetPost( 1, tamUrl, tamRespuesta )

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
