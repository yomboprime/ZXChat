/*

	Test2_ModuloWiFi
	
	Prueba del modulo WiFi
	
	Cambia aqui el SSID y el password de la wifi (separados por un espacio):
*/
#define SSID_PASSWORD "yombo ninobravo"
/*	
Microcontrolador: ATMEGA32A-PU @ 16MHz

Reset modulo wifi: pin 3

Serial hardware para comunicacion con ESP8266 a 9600 baudios
Serial software para debug con PC a 9600 baudios en los pines
10, 11 (el 10 es dummy, solo OUT en 11)

*/

// Includes

#include <SoftwareSerial.h>
#include "ModuloWiFi.h"


// Definiciones globales


// Pines

// Salidas

// Reset hacia el modulo WiFi
#define pinResetModuloWiFi 3




#define debugPrint( x ) debugPrintInterno( x )
#define debugPrintln( x ) debugPrintlnInterno( x )


// Variables globales

// Bufer
#define TAM_BUFER 771
byte bufer[ TAM_BUFER ];

// Modulo WiFi
ModuloWiFi moduloWiFi;

// Serial del modulo Wifi
#define SerialWifi Serial

// Serial para debug
SoftwareSerial SerialDebug( 10, 11 ); //10,11 en Atmega32A


// Funciones varias

void esperar( int segundos ) {
	debugPrint( "\nEsperando " );
	debugPrint( segundos );
	debugPrintln( " segundos..." );
	delay( segundos * 1000 );
}

void debugPrintInterno( const char * str ) {
	SerialDebug.print( str );
}

void debugPrintlnInterno( const char * str ) {
	SerialDebug.println( str );
}

void debugPrintInterno( int i ) {
	SerialDebug.print( i );
}

void debugPrintlnInterno( int i ) {
	SerialDebug.println( i );
}

void debugPrintlnInterno() {
	SerialDebug.println();
}

int conectarAWiFi() {

	// SSID y PASSWORD definidos al principio de este fichero
	int numBytes = sprintf( (char*)bufer, SSID_PASSWORD ) + 1;

	// Asegura que el bufer termina en byte nulo (0)
	bufer[ numBytes ] = 0;
	
	// Busca primer espacio
	int pos = 0;
	for ( pos = 0; pos < numBytes && bufer[ pos ] != 32; pos++ );
	
	if ( pos >= numBytes ) {
		return 1;
	}
	bufer[ pos ] = 0;

	// Reinicia la Wifi
	bool reiniciadoOk = false;
	int numReintentos = 3;
	while ( ! reiniciadoOk ) {

		numReintentos--;

		debugPrintln( "\nReiniciando WiFi..." );

		if ( moduloWiFi.reiniciar() ) {
			debugPrintln( "\nWiFi reiniciada satisfactoriamente." );
			reiniciadoOk = true;
		}
		else {
			debugPrintln( "\nNo se ha recibido respuesta al reiniciar." );
			if ( numReintentos <= 0 ) {
				debugPrintln( "\nFin de reintentos de reset." );
				return 2;
			}
		}

		esperar( 1 );

	}

	// Conecta a la Wifi
	debugPrintln( "\nConectando a Wifi..." );
	if ( moduloWiFi.conectarAWifi( (uint8_t*)bufer, (uint8_t*)(bufer + pos + 1) ) ) {
		debugPrintln( "\nConectado a Wifi Satisfactoriamente." );
	}
	else {
		debugPrintln( "\nError al conectar a la wifi." );
		return 3;
	}

	return 0;
}

int peticionGetPost( bool getNoPost, int numBytes, int* numBytesRespuesta  ) {

	// Asegura que el bufer termina en byte nulo (0)
	bufer[ numBytes ] = 0;

	int codigoError = moduloWiFi.peticionHttpGetPost( getNoPost, bufer, numBytesRespuesta );
	
	if ( codigoError == 0 ) {
		debugPrint( "\nPeticion HTTP Respondida OK. Longitud datos = " );
		debugPrintln( *numBytesRespuesta );
	}
	else {
		debugPrint( "\nPeticion HTTP FALLADA. Error=" );
		debugPrintln( codigoError );
	}

	return codigoError;
}


// Setup
void setup() {

	// Serial
	SerialDebug.begin( 19200 );

	// Modulo WiFi
	SerialWifi.begin( 9600 );
	moduloWiFi.configurar( &SerialWifi, &SerialDebug, pinResetModuloWiFi, bufer, TAM_BUFER );

	// Mensaje de inicio
	debugPrintln( "Empezando...espera 5 seg" );
	debugPrintln();

	esperar( 1 );

	conectarAWiFi();

	debugPrintln( "Conectando a servidor en 5 seg..." );

	esperar( 5 );

	char *cad = "www.google.es/";
	int cadLen = sprintf( (char*)bufer, cad ) + 1;	
	int numBytesRespuesta = 0;
	int result = peticionGetPost( true, cadLen, &numBytesRespuesta  );
	
	debugPrint( "Resultado: " );
	debugPrint( result );
	debugPrint( ", num bytes respuesta = " );
	debugPrintln( numBytesRespuesta );

}

// Loop
void loop() {

	// Nada que hacer

}

