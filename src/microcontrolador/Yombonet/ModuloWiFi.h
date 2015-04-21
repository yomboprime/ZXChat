
#ifndef __MODULO_WIFI__ 
#define __MODULO_WIFI__

#include <inttypes.h>
#include "Stream.h"

class ModuloWiFi {

public:

	// Primero hay que llamar a esta funcion
	void configurar( Stream* puertoSerie, int pinReset, uint8_t* bufer, int tamBufer );
	
	// Luego a esta
	bool reiniciar();

	// Todos los uint8_t* de los parametros estan en el bufer.

	bool conectarAWifi( uint8_t* ssid, uint8_t* password );
	bool getWifiConectada();
	bool desconectarWifi();

	int peticionHttpGet( uint8_t* url, int* longitudRespuesta );

	
	
	
private:

	Stream* puertoSerie;
	int pinReset;

	uint8_t* bufer;
	int tamBufer;

	bool ssidConectada;

	void purgarPuertoSerie();
	bool buscarRespuesta( uint8_t* cadenaABuscar, unsigned long timeout );
	int leerCadenaConTerminador( uint8_t terminador, unsigned long timeout );
	int leerCadenaLongitud( int tam, unsigned long timeout );

};

#endif //__MODULO_WIFI__