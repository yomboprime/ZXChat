
#include "ModuloWiFi.h"
#include "Arduino.h"

void ModuloWiFi::configurar( Stream* puertoSerie, int pinReset, uint8_t* bufer, int tamBufer ) {

	this->puertoSerie = puertoSerie;
	this->pinReset = pinReset;
	this->bufer = bufer;
	this->tamBufer = tamBufer;

	pinMode( pinReset, OUTPUT );
	digitalWrite( pinReset, HIGH );
	
}

bool ModuloWiFi::reiniciar() {

	purgarPuertoSerie();

	// Hace reset
	digitalWrite( pinReset, LOW );
	delay( 1000 );
	digitalWrite( pinReset, HIGH );
	
	// Busca "ready"
	return buscarRespuesta( (uint8_t*)"ready", 10000 );
}

bool ModuloWiFi::conectarAWifi( uint8_t* ssid, uint8_t* password ) {

	desconectarWifi();
	
	purgarPuertoSerie();
	
	puertoSerie->write( "AT+CWJAP=\"" );
	puertoSerie->write( (char*)ssid );
	puertoSerie->write( "\",\"" );
	puertoSerie->write( (char*)password );
	puertoSerie->write( "\"\r\n" );

	this->ssidConectada = buscarRespuesta( (uint8_t*)"OK", 25000 );
	return this->ssidConectada;
}

bool ModuloWiFi::getWifiConectada() {
	return this->ssidConectada;
}

bool ModuloWiFi::desconectarWifi() {
	
	purgarPuertoSerie();
	
	puertoSerie->write( "AT+CWQAP\r\n" );

	if ( ! buscarRespuesta( (uint8_t*)"OK", 4000 ) ) {
		return false;
	}

	this->ssidConectada = false;

	return true;
}

int ModuloWiFi::peticionHttpGet( uint8_t* url, int* longitudRespuesta ) {
	
	// El parametro url esta contenido en el bufer y debe estar terminado en 0
	// Al retorno, el parametro url es sobreescrito, el bufer contiene la respuesta.
	// En longitudRespuesta se devuelve el numero de bytes que ocupa la respuesta en el bufer,
	// incluyendo  0 terminador.
	// En la url es obligatorio que este la barra / que indica la raiz. El puerto
	// (:8080 por ejemplo) es opcional (por defecto es el 80)
	// URLs de ejemplo:
	// 192.168.1.100:8080/algun/documento.jsp
	// www.google.com/
	// yombo.org/blog
	
	if ( ! ssidConectada ) {
		return 1;
	}

	// Busca el dominio hasta / o :
	char *pos = NULL;
	char c = 0;	
	char *dominio = (char*)url;
	char *puerto = NULL;
	char *raiz = NULL;
	int tamDominio = 0;
	int tamPuerto = 0;
	for( pos = (char*)url, c = *pos;
		 c != 0 && c != '/' && c != ':';
		 pos++, c = *pos, tamDominio++ ){}
	if ( c == ':' ) {
		// A continuacion parsea el puerto
		pos++;
		puerto = pos;
		for( c = *pos;
			 c != 0 && c != '/';
			 pos++, c = *pos, tamPuerto++ ){}
	}
	if ( c == '/' ) {
		raiz = pos;		
	}
	else {
		return 2;
	}
	
	Serial.println( "\nPETICION:" );
	Serial.print( "\ndominio=" );
	Serial.write( (uint8_t*)dominio, tamDominio );
	Serial.println();
	Serial.print( "\npuerto=" );
	if ( puerto == NULL ) {
		Serial.println( "80" );
	}
	else {
		Serial.write( (uint8_t*)puerto, tamPuerto );
		Serial.println();
	}
	Serial.print( "\nraiz=" );
	Serial.println( raiz );

	int tamRaiz = strlen( raiz );
	
	// Calcula tamanyo peticion GET
	int tamPeticion =	4 + 			// "GET "
						tamRaiz +		// resto url
						11 + 			//" HTTP/1.1\r\n"
						6 +				// "Host: "
						tamDominio + 	// el host
						14 +			// "\r\nUser-Agent: "
						10 +			// "ZXSpectrum"
						4;				// "\r\n\r\n"

	purgarPuertoSerie();

	// Comando de conexion multiple (Aunque solo usamos una, la 4)
	puertoSerie->write( "AT+CIPMUX=1\r\n" );
	if ( ! buscarRespuesta( (uint8_t*)"OK", 2000 ) ) {
		return 3;
	}

	// Inicio conexion TCP
	puertoSerie->write( "AT+CIPSTART=4,\"TCP\",\"" );
	puertoSerie->write( (uint8_t*)dominio, tamDominio );
	puertoSerie->write( "\"," );
	if ( puerto == NULL ) {
		puertoSerie->write( "80" );
	}
	else {
		puertoSerie->write( (uint8_t*)puerto, tamPuerto );
	}
	puertoSerie->write( "\r\n" );
	if ( ! buscarRespuesta( (uint8_t*)"OK", 25000 ) ) {
		return 4;
	}

	// Envia peticion get
	puertoSerie->write( "AT+CIPSEND=4," );
	puertoSerie->print( tamPeticion );
	puertoSerie->write( "\r\n" );
	if ( ! buscarRespuesta( (uint8_t*)">", 5000 ) ) {
		return 5;
	}
	puertoSerie->write( "GET " );
	puertoSerie->write( (uint8_t*)raiz, tamRaiz );
	puertoSerie->write( " HTTP/1.1\r\nHost: " );
	puertoSerie->write( (uint8_t*)dominio, tamDominio );
	puertoSerie->write( "\r\nUser-Agent: ZXSpectrum\r\n\r\n" );
	if ( ! buscarRespuesta( (uint8_t*)"SEND OK", 25000 ) ) {
		return 6;
	}
	if ( ! buscarRespuesta( (uint8_t*)"+IPD", 25000 ) ) {
		return 7;
	}
	if ( ! buscarRespuesta( (uint8_t*)" 200 OK", 25000 ) ) {
		return 8;
	}
	if ( ! buscarRespuesta( (uint8_t*)"Content-Length: ", 25000 ) ) {
		return 9;
	}
	// Lee tamanyo respuesta
	if ( leerCadenaConTerminador( (uint8_t)'\r', 25000 ) == -1 ) {
		return 10;
	}

	int tamRespuesta = atoi( (char*)bufer );

	if ( ! buscarRespuesta( (uint8_t*)"\r\n\r\n", 25000 ) ) {
		return 11;
	}
	if ( leerCadenaLongitud( tamRespuesta, 25000 ) == -1 ) {
		return 12;
	}
	if ( ! buscarRespuesta( (uint8_t*)"OK", 25000 ) ) {
		return 13;
	}
	puertoSerie->write( "AT+CIPCLOSE=4\r\n" );
	buscarRespuesta( (uint8_t*)"OK", 2000 );

	if ( tamRespuesta >= 2 &&
		((char)bufer[ tamRespuesta - 2 ]) == '\r' &&
		((char)bufer[ tamRespuesta - 1 ]) == '\n' ) {

		tamRespuesta -= 2;
	}
	
	if ( tamRespuesta < tamBufer ) {
		bufer[ tamRespuesta++ ] = 0;
	}

	*longitudRespuesta = tamRespuesta;

	return 0;
}

void ModuloWiFi::purgarPuertoSerie() {
	while ( puertoSerie->available() > 0 ) {
		puertoSerie->read();
	}
}

bool ModuloWiFi::buscarRespuesta( uint8_t* cadenaABuscar, unsigned long timeout ) {

	/*
	Serial.print( "\nbuscarRespuesta: Buscando: " );
	Serial.print( (char*)cadenaABuscar );
	Serial.print( ", timeout=" );
	Serial.println( timeout );
	Serial.println();
	*/

	int pos = 0;
	int n = strlen( (char*)cadenaABuscar );
	unsigned long t0 = millis();
	
	while ( pos < n ) {
		while ( puertoSerie->available() == 0 ) {
			if ( ( millis() - t0 ) >= timeout ) {
				return false;
			}
		}

		int b = puertoSerie->read();
		if ( b == -1 ) {
			continue;
		}
		
		// Esto es eco para debug (comentarlo)
		Serial.print( (char)b );
		
		if ( ((uint8_t)b) == cadenaABuscar[ pos ] ) {
			pos++;
		}
		else {
			pos = 0;
			if ( ((uint8_t)b) == cadenaABuscar[ pos ] ) {
				pos++;
			}
		}
	}

	return true;
}


int ModuloWiFi::leerCadenaConTerminador( uint8_t terminador, unsigned long timeout ) {

	// Devuelve longitud escrita en el bufer antes del terminador, o -1 si hubo timeout
	// Escribe 0 terminador pero no se incluye en valor devuelto

	int pos = 0;
	unsigned long t0 = millis();

	while ( pos < tamBufer - 1 ) {
		while ( puertoSerie->available() == 0 ) {
			if ( ( millis() - t0 ) >= timeout ) {
				return -1;
			}
		}

		int b = puertoSerie->read();
		if ( b == -1 ) {
			continue;
		}

		// Esto es eco para debug (comentarlo)
		Serial.print( (char)b );

		if ( ((uint8_t)b) == terminador ) {
			bufer[ pos ] = 0;
			return pos;
		}
		else {
			bufer[ pos ] = (uint8_t) b;
			pos++;
		}
	}
	
	bufer[ pos ] = 0;
	return pos;
}


int ModuloWiFi::leerCadenaLongitud( int tam, unsigned long timeout ) {

	// Escribe en el bufer la cadena leida. No pone 0 terminador
	// Devuelve numero de bytes escritos o -1 si timeout

	tam = min( tam, tamBufer - 1 );

	int pos = 0;
	unsigned long t0 = millis();

	while ( pos < tam ) {
		while ( puertoSerie->available() == 0 ) {
			if ( ( millis() - t0 ) >= timeout ) {
				return -1;
			}
		}

		int b = puertoSerie->read();
		if ( b == -1 ) {
			continue;
		}

		// Esto es eco para debug (comentarlo)
		Serial.print( (char)b );

		bufer[ pos ] = (uint8_t) b;
		pos++;
	}

	return pos;
}
