
#define VERSION "r2"

#include <print64.bas>
#include "Yombonet.bas"
#include "entradaSalidaTexto.bas"
#include "variablesGlobales.bas"

function textoTarea( idTarea as string ) as string
	if idTarea = "" then
		return "(desactivada)"
	end if
	return "(activada, id=" + idTarea + ")"
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
imprimirCadenaWrap( "2) Des/Activar Twitter (o 1a tarea) " + textoTarea( strCodigoTarea1 ), 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "3) Des/Activar 2a tarea " + textoTarea( strCodigoTarea2 ), 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "4) Des/Activar 3a tarea " + textoTarea( strCodigoTarea3 ), 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "5) Cambiar cuenta de Botize (" + strNick + ")", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "6) Configurar WiFi", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "7) Testear Yombonet", 0, y1 + 1, 0, x1, y1 )
imprimirCadenaWrap( "8) Salir", 0, y1 + 1, 0, x1, y1 )

imprimirCadenaWrap( "Pulse opcion y enter...", 0, y1 + 2, 0, x1, y1 )
y1 = y1 + 1
tecla = leerCadenaEntrada( "", y1 )

if tecla = "1" then
	if strSSID = "" then
		imprimirCadenaWrap( "Debe especificar un SSID para conectar a WiFi. Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
		esperarTecla()
		goto Menu
	end if
	
	if strCodigoTarea1 = "" and strCodigoTarea2 = "" and strCodigoTarea3 = "" then
        imprimirCadenaWrap( "No hay ninguna tarea activada. Pulse una tecla.", 0, y1 + 1, 0, x1, y1 )
        esperarTecla()
        goto Menu
    end if

	intentoConexion = 1
	goto ConexionWiFi

end if
if tecla = "2" then goto ActivarDesactivarTarea1 : end if
if tecla = "3" then goto ActivarDesactivarTarea2 : end if
if tecla = "4" then goto ActivarDesactivarTarea3 : end if
if tecla = "5" then goto CambiarNick : end if
if tecla = "6" then goto ConfigurarWiFi : end if
if tecla = "7" then goto TestearYombonet : end if
if tecla = "8" then goto Fin : end if

goto Menu






ActivarDesactivarTarea1:

imprimirCadenaWrap( "Escriba el numero (id) de la primera tarea en Botize: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strCodigoTarea1 = leerCadenaEntrada( strCodigoTarea1, y1 )

goto Menu

ActivarDesactivarTarea2:

imprimirCadenaWrap( "Escriba el numero (id) de la segunda tarea en Botize: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strCodigoTarea2 = leerCadenaEntrada( strCodigoTarea2, y1 )

goto Menu

ActivarDesactivarTarea3:

imprimirCadenaWrap( "Escriba el numero (id) de la tercera tarea en Botize: ", 0, y1 + 1, 0, x1, y1 )
y1 = y1 + 1
strCodigoTarea3 = leerCadenaEntrada( strCodigoTarea3, y1 )

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

cls
y1 = 0

while cadenaEntrada <> " "

	imprimirCadenaWrap( "Escriba un mensaje para enviar. Escriba un solo espacio para volver al menu.", 0, y1 + 1, 0, x1, y1 )
	y1 = y1 + 1

	cadenaEntrada = leerCadenaEntrada( "", y1 )

	border 0

	if cadenaEntrada = " " then
		goto finBucle
	end if
	
	if cadenaEntrada <> "" then
	
        imprimirCadenaWrap( "Mensaje: " + cadenaEntrada, 0, y1 + 1, 0, x1, y1 )
	
		'codifica url
		dim cadenaEntrada2 as string
		
		cadenaEntrada2 = cadenaEntrada

		cadenaEntrada2 = reemplazarCadena( cadenaEntrada, $25, "%25" )'%
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $20, "%20" )'espacio
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $21, "%21" )'!
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $22, "%22" )'"
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $28, "%28" )'(
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $29, "%29" )')
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2C, "%2C" )',
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $3A, "%3A" )':
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $3F, "%3F" )'!
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $40, "%40" )'@
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $23, "%23" )'#
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2E, "%2E" )'.
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $24, "%24" )'$
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $26, "%26" )'&
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $27, "%27" )''
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2A, "%2A" )'*
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2B, "%2B" )'+
		cadenaEntrada2 = reemplazarCadena( cadenaEntrada2, $2F, "%2F" )'/
		

		' Envia mensaje a las tres tareas
		
		' Tarea 1
		if strCodigoTarea1 <> "" then
		
            imprimirCadenaWrap( "Enviando mensaje a la tarea 1, id = " + strCodigoTarea1 + " ...", 0, y1 + 1, 0, x1, y1 )
            
            strUrl = "dev.botize.com/request/" + strNick + "/" + strServerPassword + "/" + strCodigoTarea1 + "?mensaje=" + cadenaEntrada2

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
		
		' Tarea 2
        if strCodigoTarea2 <> "" then
        
            imprimirCadenaWrap( "Enviando mensaje a la tarea 2, id = " + strCodigoTarea2 + " ...", 0, y1 + 1, 0, x1, y1 )
            
            strUrl = "dev.botize.com/request/" + strNick + "/" + strServerPassword + "/" + strCodigoTarea2 + "?mensaje=" + cadenaEntrada2

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
		
        ' Tarea 3
        if strCodigoTarea3 <> "" then
        
            imprimirCadenaWrap( "Enviando mensaje a la tarea 3, id = " + strCodigoTarea3 + " ...", 0, y1 + 1, 0, x1, y1 )
            
            strUrl = "dev.botize.com/request/" + strNick + "/" + strServerPassword + "/" + strCodigoTarea3 + "?mensaje=" + cadenaEntrada2

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
		
		
	end if

	if y0 < 0 then
		y0 = 0
	end if

	finBucle:

end while

goto Menu

Fin:
print "FIN"
