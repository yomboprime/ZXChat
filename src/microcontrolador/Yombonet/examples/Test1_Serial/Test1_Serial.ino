/*

Test de Serial


Microcontrolador: ATMEGA32A-PU @ 16MHz

Serial hardware para comunicacion con ESP8266 a 9600 baudios
Serial software para debug con PC a 9600 baudios en los pines
10, 11 (el 10 es dummy, solo OUT en 11)

*/

// Includes

#include <SoftwareSerial.h>

#define debugPrint( x ) debugPrintInterno( x )
#define debugPrintln( x ) debugPrintlnInterno( x )


// Variables globales

// Serial del modulo Wifi
//#define SerialWifi Serial1
#define SerialWifi Serial

// Serial para debug
//SoftwareSerial SerialDebug( 8, 9 ); // 8,9 en ATMEGA1284P
SoftwareSerial SerialDebug( 10, 11 ); //10,11 en Atmega32A

// Funciones varias

void esperar( int segundos ) {
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

// Setup
void setup() {

	// Serial
	SerialDebug.begin( 9600 );
	
	Serial.begin( 9600 );

}

// Loop
void loop() {

	esperar( 2 );

	debugPrintln( "Hola mundo, esto funciona!!!" );

	Serial.println( "Hola mundo, esto funciona!!!" );

}

