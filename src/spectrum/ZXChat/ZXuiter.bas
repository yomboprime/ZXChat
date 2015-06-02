
#define VERSION "r1"

#include "Yombonet.bas"
#include "entradaSalidaTexto.bas"
#include "variablesGlobales.bas"

function textoActivacion( on as ubyte ) as string
	if on = 0 then
		return "(desactivado)"
	end if
	return "(activado)"
end function

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

dim twitterActivado as ubyte
dim faceBookActivado as ubyte

twitterActivado = 1
faceBookActivado = 0

Menu:

border 2 : paper 0 : ink 7: flash 0 : bright 0 : over 0
cls

imprimirCadenaWrap( "ZXuiter " + VERSION, 0, 0, 0, x1, y1 )
imprimirCadenaWrap( "1) Comenzar", 0, y1 + 2, 0, x1, y1 )
imprimirCadenaWrap( "2) Des/Activar Twitter " + textoActivacion( twitterActivado ), 0, y1 + 2, 0, x1, y1 )
imprimirCadenaWrap( "3) Des/Activar Facebook " + textoActivacion( faceBookActivado ), 0, y1 + 2, 0, x1, y1 )
imprimirCadenaWrap( "4) Cambiar cuenta de Botize (" + strNick + ")", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "5) Configurar WiFi", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "6) Testear Yombonet", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "7) Salir", 0, y1 + 1, 0, x1, y1 )

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
if tecla = "2" then goto ActivarDesactivarTwitter : end if
if tecla = "3" then goto ActivarDesactivarFacebook : end if
if tecla = "4" then goto CambiarNick : end if
if tecla = "5" then goto ConfigurarWiFi : end if
if tecla = "6" then goto TestearYombonet : end if
if tecla = "7" then goto Fin : end if

goto Menu






ActivarDesactivarTwitter:

if twitterActivado = 0 then
	twitterActivado = 1
else
	twitterActivado = 0
end if

goto Menu





ActivarDesactivarFacebook:

if faceBookActivado = 0 then
	faceBookActivado = 1
else
	faceBookActivado = 0
end if

goto Menu





CambiarNick:

imprimirCadenaWrap( "Escriba su nombre de usuario en botize y pulse enter: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strNick = leerCadenaEntrada( strNick, y1 )

imprimirCadenaWrap( "Escriba el 'access token' para el usuario en Botize: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strServerPassword = leerCadenaEntrada( strServerPassword, y1 )

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

imprimirCadenaWrap( "Pulse una tecla...", 0, y1 + 2, 0, x1, y1 )
esperarTecla()


' Bucle de envio de mensajes:
'cadenaEntrada = "Disculpen las molestias. Prueba 10. () Perdonen las disculpas reiteradamente 11 123456789012345678901234"
'cadenaEntrada = "Hola mundo! Tuit enviado desde un ZX Spectrum gomas con WiFi (protot. Yombonet) via @botize"
cadenaEntrada = ""

while cadenaEntrada <> " "

	cls
	y0 = 0
	imprimirCadenaWrap( "Escriba un mensaje para enviar. Escriba un solo espacio para volver al menu.", 0, y0, 0, x1, y0 )
	y0 = y0 + 1

	' TODO sustituir primer parametro por ""
	cadenaEntrada = leerCadenaEntrada( "", y0 )

	border 0

	if cadenaEntrada = " " then
		goto finBucle
	end if
	
	if cadenaEntrada <> "" then
		'codifica url
		dim cadenaEntrada2 as string
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada, $20, "%20" )'espacio
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $21, "%21" )'!
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $22, "%22" )'"
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $28, "%28" )'(
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $29, "%29" )')
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2C, "%2C" )',
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $3A, "%3A" )':
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $3F, "%3F" )'!
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $40, "%40" )'@

		' Envia mensaje
		strUrl =  "botize.com/v1/run?client_id=" + strNick + "&access_token=" + strServerPassword
		if twitterActivado <> 0 then
			strUrl = strUrl + "&keyword=tweet"
		end if
		'TODO facebook
		'if faceBookActivado <> 0 then
		'	strUrl = strUrl + "&keyword=facebook"
		'end if
		strUrl = strUrl + "&data=%7B%22tweet%22%3A%22" + cadenaEntrada2 + "%22%7D"

		tamUrl = copiarCadenaABufer( strUrl )
		codigoError = peticionGetPost( 0, tamUrl, tamRespuesta )
		if codigoError = 0 then
			' Pone borde verde
			border 4
			imprimirCadenaWrap( "Mensaje enviado. Respuesta:", 0, y1 + 1, 0, x1, y1 )

			imprimirCadenaBuferWrap( 0, tamRespuesta, 0, y1 + 1, x1, y1 )
		else
			' Pone borde amarillo (error)
			border 6
			imprimirCadenaWrap( "Codigo de error en envio: " + str( codigoError ), 0, y1 + 1, 0, x1, y1 )
		end if
		y0 = y1 + 1
		
	end if

	if y0 < 0 then
		y0 = 0
	end if

	finBucle:

end while

goto Menu

Fin:
print "FIN"
