
#ifndef __VARIABLES_GLOBALES__

#define __VARIABLES_GLOBALES__

dim strSSID as string
dim strWifiPassword as string
dim ubUsarYomboServer as ubyte
dim strIPServidor as string
dim strUrlServidor as string
dim strNick as string

dim debugactivado as ubyte

strNick = "sinnombre"
strSSID = "yombo"
strWifiPassword = "ninobravo"

ubUsarYomboServer = 1	' Poner a 0 y especificar IP en la siguiente linea para servidor manual.
strIPServidor = ""
strUrlServidor = ":8080/YomboServer/ZXChat/chat"

debugactivado = 0

#endif