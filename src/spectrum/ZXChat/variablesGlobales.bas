
#ifndef __VARIABLES_GLOBALES__

#define __VARIABLES_GLOBALES__

dim strSSID as string
dim strWifiPassword as string
dim ubUsarYomboServer as ubyte
dim strIPServidor as string
dim strUrlServidor as string
dim strNick as string
dim strServerPassword as string
dim strCodigoTarea1 as string
dim strCodigoTarea2 as string
dim strCodigoTarea3 as string
dim avisoSonoroActivado as ubyte
dim guardadoACintaActivado as ubyte

dim debugactivado as ubyte

strNick = "nick"
strServerPassword = ""
strCodigoTarea1 = ""
strCodigoTarea2 = ""
strCodigoTarea3 = ""

strSSID = "poner_ssid"				' Nombre de la red WiFi
strWifiPassword = "poner_passwd"	' Password de la WiFi

ubUsarYomboServer = 1	' Poner a 0 y especificar IP en la siguiente linea para servidor manual.
strIPServidor = "xx.xx.xx.xx"
strUrlServidor = ":8080/YomboServer/ZXChat/chat"

avisoSonoroActivado = 0
guardadoACintaActivado = 0

debugactivado = 0

#endif