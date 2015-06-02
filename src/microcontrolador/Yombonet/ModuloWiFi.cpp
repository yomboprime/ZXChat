
#include "ModuloWiFi.h"
#include "Arduino.h"

// Comando de debug, por ejemplo SDEBUG->println( "Hola mundo" );
#define SDEBUG if ( debugActivado && puertoSerieDebug ) puertoSerieDebug

void ModuloWiFi::configurar( Stream* puertoSerie, Stream* puertoSerieDebug, int pinReset, uint8_t* bufer, int tamBufer ) {

	pinMode( pinReset, OUTPUT );
	digitalWrite( pinReset, HIGH );

	this->puertoSerie = puertoSerie;
	this->puertoSerieDebug = puertoSerieDebug;
	this->pinReset = pinReset;
	this->bufer = bufer;
	this->tamBufer = tamBufer;

	debugActivado = true;
	
}

bool ModuloWiFi::reiniciar() {

	purgarPuertoSerie();

	// Hace reset
	digitalWrite( pinReset, LOW );
	delay( 1000 );
	digitalWrite( pinReset, HIGH );
	
	// Busca "ready"
	int r = buscarRespuesta( (uint8_t*)"ready", 10000 );

	// Pone modo STA + Soft AP
	purgarPuertoSerie();
	puertoSerie->write( "AT+CWMODE=3\r\n" );
	buscarRespuesta( (uint8_t*)"OK", 10000 );

/*
puertoSerie->write( "AT+CIOBAUD=9600\r\n" );
buscarRespuesta( (uint8_t*)"OK", 10000 );
SDEBUG->println( "\nwDEBUG3" );
*/

	return r;
}

bool ModuloWiFi::conectarAWifi( uint8_t* ssid, uint8_t* password ) {

	// Desconecta
	desconectarWifi();

	purgarPuertoSerie();

	puertoSerie->write( "AT+CWJAP=\"" );
	puertoSerie->write( (char*)ssid );
	puertoSerie->write( "\",\"" );
	puertoSerie->write( (char*)password );
	puertoSerie->write( "\"\r\n" );

	this->ssidConectada = buscarRespuesta( (uint8_t*)"OK", 25000 );

	purgarPuertoSerie();
	puertoSerie->write( "AT+CIFSR\r\n" );
	buscarRespuesta( (uint8_t*)"OK", 2000 );

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

int ModuloWiFi::peticionHttpGetPost( bool getNoPost, uint8_t* url, int* longitudRespuesta ) {
	
	// Si getNoPost = 0, se hace peticion POST, si no, GET.
	// El parametro url esta contenido en el bufer y debe estar terminado en el byte nulo (0)
	// Al retorno, el parametro url es sobreescrito, el bufer contiene la respuesta.
	// En longitudRespuesta se devuelve el numero de bytes que ocupa la respuesta en el bufer,
	// incluyendo  0 terminador.
	// En la url es obligatorio que este la barra / que indica la raiz. El puerto es opcional
	// (:8080 por ejemplo) Por defecto es el 80.
	// Para las peticiones post, los parametros se ponen en la URL igual que en GET, y la
	// trama http se construye con POST con los parametros en el body.
	// URLs de ejemplo:
	// 192.168.1.100:8080/algun/documento.jsp?param1=valor1&param2=valor2
	// www.google.com/
	// yombo.org/blog

	*longitudRespuesta = 0;

	if ( ! ssidConectada ) {
		return 1;
	}

	// Busca el dominio hasta / o :
	char *pos = NULL;
	char c = 0;	
	char *dominio = (char*)url;
	char *puerto = NULL;
	char *raiz = NULL;
	char *parametros = NULL;
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

	int tamRaiz = strlen( raiz );
	int tamParametros = tamRaiz;

	// Si es peticion post, parsea parametros
	if ( getNoPost == 0 ) {
		for ( pos = (char*)raiz, c = *pos;
				c != 0 && c != '?';
				pos++, c = *pos, tamParametros-- ){}
		if ( c == '?' && tamParametros > 1 ) {
			parametros = pos + 1;
			*pos = 0;
			tamParametros--;
			tamRaiz -= tamParametros + 1;
		}
		else {
			// No hay parametros
			parametros = "";
			tamParametros = 0;
		}
	}
	
	SDEBUG->println( "\nPETICION:" );
	SDEBUG->print( "\ndominio=" );
	SDEBUG->write( (uint8_t*)dominio, tamDominio );
	SDEBUG->println();
	SDEBUG->print( "\npuerto=" );
	if ( puerto == NULL ) {
		SDEBUG->println( "80" );
	}
	else {
		SDEBUG->write( (uint8_t*)puerto, tamPuerto );
		SDEBUG->println();
	}
	SDEBUG->print( "\nraiz=" );
	SDEBUG->println( raiz );
	if ( parametros != NULL ) {
		SDEBUG->print( "\nparametros=" );
		SDEBUG->println( parametros );
	}

	// Calcula tamanyo peticion GET
	int tamPeticion =	(getNoPost?4:5)+// "GET " o "POST "
						tamRaiz +		// resto url
						11 + 			//" HTTP/1.1\r\n"
						6 +				// "Host: "
						tamDominio + 	// el host
						14 +			// "\r\nUser-Agent: "
						10 +			// "ZXSpectrum"
						4;				// "\r\n\r\n" Linea en blanco entre cabecera y body

	SDEBUG->print( "\nDEBUG1:" );
	SDEBUG->print( tamPeticion );

	
	
	if ( getNoPost == 0 ) {
		// Peticion POST, anyade Content-Type y body con los parametros
		tamPeticion +=  49 + // "\r\nContent-Type: application/x-www-form-urlencoded"
						18 + // "\r\nContent-Length: "
						numeroDeDigitos( tamParametros ) + // Numero de digitos del tamanyo del contenido.
						tamParametros; // Tamanyo del contenido.
	}
	
	SDEBUG->print( "\nDEBUG2:" );
	SDEBUG->print( tamPeticion );
	SDEBUG->print( ", " );
	SDEBUG->print( tamParametros );
	SDEBUG->print( ", " );
	SDEBUG->print( numeroDeDigitos( tamParametros ) );
	SDEBUG->print( ", " );
	SDEBUG->println();

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
	if ( ! buscarRespuesta( (uint8_t*)"OK", 15000 ) ) {
		return 4;
	}

	// Envia peticion get o post
	puertoSerie->write( "AT+CIPSEND=4," );
	puertoSerie->print( tamPeticion );
	puertoSerie->write( "\r\n" );
	if ( ! buscarRespuesta( (uint8_t*)">", 5000 ) ) {
		return 5;
	}
	puertoSerie->write( getNoPost ? "GET " : "POST " );
	puertoSerie->write( (uint8_t*)raiz, tamRaiz );
	puertoSerie->write( " HTTP/1.1\r\nHost: " );
	puertoSerie->write( (uint8_t*)dominio, tamDominio );
	puertoSerie->write( "\r\nUser-Agent: ZXSpectrum" );
	if ( getNoPost == 0 ) {
		puertoSerie->write( "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: " );
		puertoSerie->print( tamParametros );
		puertoSerie->write( "\r\n\r\n" );
		puertoSerie->write( (uint8_t*)parametros, tamParametros );
	}
	else {
		puertoSerie->write( "\r\n\r\n" );
	}

	if ( ! buscarRespuesta( (uint8_t*)"SEND OK", 15000 ) ) {
		return 6;
	}
	if ( ! buscarRespuesta( (uint8_t*)"+IPD", 15000 ) ) {
		return 7;
	}
	if ( ! buscarRespuesta( (uint8_t*)" 200 OK", 15000 ) ) {
		return 8;
	}
	if ( ! buscarRespuesta( (uint8_t*)"Content-Length: ", 15000 ) ) {
		return 9;
	}
	// Lee tamanyo respuesta
	if ( leerCadenaConTerminador( (uint8_t)'\r', 15000 ) == -1 ) {
		return 10;
	}

	int tamRespuesta = atoi( (char*)bufer );

	if ( ! buscarRespuesta( (uint8_t*)"\r\n\r\n", 15000 ) ) {
		return 11;
	}
	if ( leerCadenaLongitud( tamRespuesta, 15000 ) == -1 ) {
		return 12;
	}
	if ( ! buscarRespuesta( (uint8_t*)"OK", 15000 ) ) {
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
		
		// Esto es eco para debug
		SDEBUG->print( (char)b );
		
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

		// Esto es eco para debug
		SDEBUG->print( (char)b );

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

		// Esto es eco para debug
		SDEBUG->print( (char)b );

		bufer[ pos ] = (uint8_t) b;
		pos++;
	}

	return pos;
}

void ModuloWiFi::activarDesactivarDebug( bool activar ) {
	debugActivado = activar;
}
