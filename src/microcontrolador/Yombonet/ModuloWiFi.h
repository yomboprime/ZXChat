
#ifndef __MODULO_WIFI__ 
#define __MODULO_WIFI__

#include <inttypes.h>
#include "Stream.h"

class ModuloWiFi {

public:

	// Primero hay que llamar a esta funcion
	void configurar( Stream* puertoSerie, Stream* puertoSerieDebug, int pinReset, uint8_t* bufer, int tamBufer );
	
	// Luego a esta
	bool reiniciar();

	// Todos los uint8_t* de los parametros estan en el bufer.

	bool conectarAWifi( uint8_t* ssid, uint8_t* password );
	bool getWifiConectada();
	bool desconectarWifi();

	int abrirConexionTCP( uint8_t*dominio, int tamDominio, uint8_t* puerto, int tamPuerto );
	int cerrarConexionTCP();
	bool getConexionTCPAbierta();
	
	int peticionHttpGetPost( bool getNoPost, uint8_t* url, int* longitudRespuesta );

	bool buscarRespuesta( uint8_t* cadenaABuscar, unsigned long timeout );
    
    bool buscarRespuestaRMinuscula( uint8_t* cadenaABuscar, unsigned long timeout );

	void activarDesactivarDebug( bool activar );

private:

	Stream* puertoSerie;
	Stream* puertoSerieDebug;

	bool debugActivado;

	int pinReset;

	uint8_t* bufer;
	int tamBufer;

	bool ssidConectada;

	void purgarPuertoSerie();
	int leerCadenaConTerminador( uint8_t terminador, unsigned long timeout );
	int leerCadenaLongitud( uint8_t* buferAEscribir, int tam, unsigned long timeout );
	
	// Flag de conexion TCP activa
	bool conexionTCPActiva;
	
	int numeroDeDigitos( int i ) {
		// Hasta 999 sin signo
		if ( i < 10 ) {
			return 1;
		}
		if ( i < 100 ) {
			return 2;
		}
		return 3;
	}

};

#endif //__MODULO_WIFI__