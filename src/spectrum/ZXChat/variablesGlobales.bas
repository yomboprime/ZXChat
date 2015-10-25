
#ifndef __VARIABLES_GLOBALES__

#define __VARIABLES_GLOBALES__

dim strSSID as string
dim strWifiPassword as string
dim ubUsarYomboServer as ubyte
dim strIPServidor as string
dim strUrlServidor as string
dim strNick as string
dim strServerPassword as string
dim avisoSonoroActivado as ubyte
dim guardadoACintaActivado as ubyte

dim debugactivado as ubyte

strNick = "yombo"
strServerPassword = "33424_f6b45622da7575afeb557d81b44d3b15"
strSSID = "yombo"				' Nombre de la red WiFi
strWifiPassword = "ninobravo"	' Password de la WiFi

ubUsarYomboServer = 1	' Poner a 0 y especificar IP en la siguiente linea para servidor manual.
strIPServidor = "83.43.85.77"
strUrlServidor = ":8080/YomboServer/ZXChat/chat"

avisoSonoroActivado = 0
guardadoACintaActivado = 0

debugactivado = 0

#endif