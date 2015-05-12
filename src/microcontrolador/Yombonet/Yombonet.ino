/*

Yombonet
*/
#define REVISION "Yombonet firmware r1"
#define DEBUG 1
/*

Interfaz Spectrum con módulo Wifi ESP-07

Microcontrolador: ATMEGA32A-PU @ 16MHz

CPLD Interfaz spectrum:

Bus Datos: D0..D7 = PC0..PC7

RD CPLD: pin 12
WR CPLD: pin 13
REG CPLD: pin 14

SalidaPWMLEDs 15

SenyalRd1: pin 0
SenyalWr1: pin 1
SenyalWr0: pin 2

Reset modulo wifi: pin 3

Entradas analogicas o digitales/Salidas digitales A7..A0

Entradas/salidas digitales B3..B0 corresponden a pines 7, 6, 5, 4

Serial hardware para comunicacion con ESP8266 a 9600 baudios
Serial software para debug con PC a 9600 baudios en los pines
10, 11 (el 10 es dummy, solo OUT en 11)

Funcionamiento de la interfaz Yombonet
--------------------------------------

Yombonet consta de un modulo WiFi Esp-07, un microcontrolador Atmega32A y un CPLD XC9572XL.

El CPLD provee una memoria de doble puerto accesible por el Spectrum y por el microcontrolador,
y es el puente para que se comuniquen.
Esta memoria son cuatro registros de un byte cada uno: dos registros de control y dos de datos.
El registro de control en el que escribe el Spectrum es el registro de control que lee el
microcontrolador, y viceversa. Y lo mismo para los dos registros de datos.
Vea el archivo doc/imagenPlaca.png para una representación gráfica de estos registros.

Los registros de control y datos son accesibles para el Spectrum en los puertos de I/O siguientes:

A7 A6 A0	Registro					Direccion del puerto
----------------------------------	--------------------------------------------
0  0  1		Registro de control/opcode	FF3F	/	65343	/	1111111100111111
0  1  1		Registro de datos			FF7F	/	65407	/	1111111101111111


Instrucciones aceptadas por el interfaz
---------------------------------------

La yombonet espera instrucciones del Spectrum y las ejecuta, devolviendo una respuesta.
Cada instruccion comienza cuando el Spectrum escribe un byte en el registro de control. Despues,
segun la instruccion, puede transmitirse una cadena de bytes en el registro de datos en uno u otro
sentido, o ambos.

Antes de escribir una instruccion, el Spectrum debe comprobar que el bit 0 del registro de
control se lee como 1. De otra forma Yombonet esta ocupada y no puede responder.
Analogamente, antes de empezar a recibir o transmitir una cadena de bytes o un solo byte, el Spectrum
ha de comprobar otros dos bits del registro de control, como se detalla mas adelante.

Cuando se especifica que al ejecutar una instruccion se transmite una cadena de bytes
en uno u otro sentido, significa que se transmite primero el numero de bytes a transmitir (un entero de
16 bits, dos bytes, el más bajo primero) seguido de esa cantidad de bytes, leidos (o escritos)
consecutivamente desde el registro de datos. Los dos bytes de cantidad no se incluyen a si mismos.
La unica excepcion a esto es cuando la instruccion especifica que solo se escribira un byte en el registro
de datos. En ese caso no se envian los dos bytes de longitud.

Cuando se especifica que hay un codigo de resultado disponible en el registro de control al
finalizar una instruccion, significa que los cuatro bits superiores contienen ese codigo.
El codigo estara disponible hasta que se escriba una nueva instruccion. Los cuatro bits inferiores
del registro de control tienen significado fijo, que se detalla a continuacion:

Registro de control leido por Spectrum
bit 0: Uc Listo para recibir instruccion. Antes de que el Spectrum envie una instruccion,
	ha de comprobar que este bit del registro de control este a 1. De lo contrario el
	interfaz esta ocupado.
bit 1: Tras escribir una instruccion, si esta espera que el Spectrum escriba uno o mas bytes, se ha de
	esperar	a que este bit este a 1 antes de comenzar a transmitirlos.
bit 2: Analogamente al bit anterior, si la instruccion espera que el Spectrum reciba uno o mas
	bytes como respuesta, el Spectrum debe esperar a que este bit este a 1 antes de empezar a leerlos.



A continuacion se listan las instrucciones:

Codigo instruccion: 0b00000000
Descripcion: Obtener numero de revision del firmware
	Se devuelve una cadena de bytes con la descripcion del numero de revision.

Codigo instruccion: 0b00000001
Descripcion: Activar/desactivar debug por puerto serie
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica con 00000000 que se quiere desactivar el debug por consola (puerto
	serie por software en el pin TXDB, solo salida). Con 00000001 se activa el debug.
	Codigos de error:
		0	Debug desactivado
		1	Debug activado

Instrucciones de Internet
-------------------------

Codigo instruccion: 0b00000010
Descripcion: Conectar a Wifi
	Se espera una cadena de bytes "SSID PASSW" donde
	SSID es el SSID de la Wifi a la que se quiere conectar, y PASSW el password.
	Se devuelve una cadena de bytes de longitud 0 si hubo error (y el codigo de
	error esta disponible en el registro de control), o la cadena con la IP obtenida
	en la Wifi si hubo exito.
	Codigos de error:
		0	Exito
		1	Error al conectar

Codigo instruccion: 0b00000011
Descripcion: Preguntar si hay conexion
	El microcontrolador responde con un unico byte en el registro de datos, 00000000 si no estamos
	conectados	o 00000001 si lo estamos.

Codigo instruccion: 0b00000100
Descripcion: Hacer peticion HTTP GET
	Se espera una cadena de bytes con la URL de la peticion, sin "http://" y es obligatorio que la
	raiz "/" de la url este presente. El puerto TCP es opcional. Un ejemplo: "www.google.es/".
	El contenido de la respuesta a la peticion GET se devuelve al Spectrum como cadena de bytes, o
	si hubo error, se devuelve la cadena de longitud 0.
	Codigos de error:
		0	Exito
		1	Error: No conectado a Wifi
		2	Error: La URL no contiene la barra "/" de la raiz
		3	Error del modulo Wifi al configurarse
		4	Error al conectar al host
		5	Error al transmitir datos al modulo WiFi
		6	Error al enviar la peticion al host
		7	Error al procesar la respuesta del host

Instrucciones de E/S
--------------------

Codigo instruccion: 0b00010000
Descripcion: Configurar como entradas digitales A0...A7
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica (con sus 1's) cuales de los pines de A0 hasta A7 seran configurados como
	entradas digitales o analogicas (A0 es el LSB) Los bits a 0 no modifican los respectivos pines.
	El estado por defecto (al reinicio) de los pines es como entradas.
	Codigos de error: 0 Exito
	
Codigo instruccion: 0b00010001
Descripcion: Configurar como salidas digitales A0...A7
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica (con sus 1's) cuales de los pines de A0 hasta A7 seran configurados como
	salidas digitales (A0 es el LSB) Los bits a 0 no modifican los respectivos pines.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00010010
Descripcion: Leer valor de una entrada analogica seleccionando entre A0...A7
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica con sus tres bits mas bajos cual de las entradas analogicas (A0..A7) se
	ha de leer. Se devuelve un byte en el registro de datos con el valor leido.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00010011
Descripcion: Leer todas las entradas digitales A0...A7
	Yombonet escribira en el registro de datos un byte con los valores digitales de las ocho entradas A0..A7
	(A0 es LSB) Los pines que han sido configurados como entradas mostraran su valor; los configurados como
	salida mostraran un valor indefinido.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00010100
Descripcion: Escribir todas las salidas A0...A7
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica los valores digitales a escribir en los pines A0..A7 (A0 es LSB). Solo se escribiran
	los pines configurados previamente como salidas, los otros se ignoraran.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00011000
Descripcion: Configurar como entradas digitales B0...B3
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica (con los 1's de los cuatro bits mas bajos) cuales de los pines de B0 hasta B3
	seran configurados como	entradas digitales (B0 es el LSB) Los bits a 0 no modifican sus respectivos pines.
	El estado por defecto de los pines es como entradas.
	Codigos de error: 0 Exito
	
Codigo instruccion: 0b00011001
Descripcion: Configurar como salidas digitales B0...B3
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica (con los 1's de los cuatro bits mas bajos) cuales de los pines de B0 hasta B3
	seran configurados como	salidas digitales (B0 es el LSB) Los bits a 0 no modifican sus respectivos pines.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00011010
Descripcion: Leer todas las entradas B0...B3
	Yombonet escribira en el registro de datos un byte con los valores digitales de las cuatro entradas B0..B3
	(B0 es LSB) Los pines que han sido configurados como entradas mostraran su valor; los configurados como
	salida mostraran un valor indefinido.
	Codigos de error: 0 Exito

Codigo instruccion: 0b00011011
Descripcion: Escribir todas las salidas B0...B3
	Se espera que se haya escrito un byte en el registro de datos antes de escribir el byte de esta
	instruccion. El byte indica con sus cuatro bits mas bajos los valores digitales a escribir en los pines B0..B3
	(A0 es LSB). Solo se escribiran los pines configurados previamente como salidas, los otros se ignoraran.
	Codigos de error: 0 Exito

*/

// Includes

#include <SoftwareSerial.h>
#include "ModuloWiFi.h"


// Definiciones globales

// Registros del CPLD
#define REGISTRO_CONTROL 0
#define REGISTRO_DATOS 1

// Significado de los bits del registro de control en direccion uC -> Spectrum:
#define UC_LISTO 1
#define UC_LISTO_RECIBIR 2
#define UC_LISTO_TRANSMITIR 4


// Pines

// Salidas

// Read hacia el cpld (salida)
#define pinRD 12

// Write hacia el cpld (salida)
#define pinWR 13

// Registro (0 0 1) (salida)
#define pinREG 14

// Salida PWM hacia el cpld como reloj de los LEDs
#define pinRelojLED 15

// Reset hacia el modulo WiFi
#define pinResetModuloWiFi 3

// Entradas

// Pines de senyalizacion lectura/escritura de registro 1 y de escritura de registro 0
#define pinSenyalRd1 0
#define pinSenyalWr1 1
#define pinSenyalWr0 2
#define mascaraSenyalRd1 1
#define mascaraSenyalWr1 2
#define mascaraSenyalWr0 4


#if ( DEBUG == 1 )
	// Debug habilitado
	#define debugPrint( x ) debugPrintInterno( x )
	#define debugPrintln( x ) debugPrintlnInterno( x )
#else
	// Debug deshabilitado, mejor rendimiento
	#define debugPrint( x )
	#define debugPrintln( x )
#endif


// Variables globales

// Bufer
#define TAM_BUFER 771
byte bufer[ TAM_BUFER ];

// Modulo WiFi
ModuloWiFi moduloWiFi;

// Serial del modulo Wifi
#define SerialWifi Serial1
//#define SerialWifi Serial

// Serial para debug
SoftwareSerial SerialDebug( 8, 9 ); // 8,9 en ATMEGA1284P
//SoftwareSerial SerialDebug( 10, 11 ); //10,11 en Atmega32A

// Indica si esta activado debug por puerto serie
bool debugActivado = true;


// Funciones varias

void esperar( int segundos ) {
	debugPrint( "\nEsperando " );
	debugPrint( segundos );
	debugPrintln( " segundos..." );
	delay( segundos * 1000 );
}

void debugPrintInterno( const char * str ) {
	if ( debugActivado ) {
		SerialDebug.print( str );
	}
}

void debugPrintlnInterno( const char * str ) {
	if ( debugActivado ) {
		SerialDebug.println( str );
	}
}

void debugPrintInterno( int i ) {
	if ( debugActivado ) {
		SerialDebug.print( i );
	}
}

void debugPrintlnInterno( int i ) {
	if ( debugActivado ) {
		SerialDebug.println( i );
	}
}

void debugPrintlnInterno() {
	if ( debugActivado ) {
		SerialDebug.println();
	}
}

// Funciones de la interfaz con Spectrum

inline void busDatosTriState() {
	DDRC = 0x00;
}

inline void busDatosOutput() {
	DDRC = 0xFF;
}

inline void setRD( byte on ) {
	digitalWrite( pinRD, on? LOW : HIGH );
}

inline void setWR( byte on ) {
	digitalWrite( pinWR, on? LOW : HIGH );
}

inline void setREG( byte on ) {
	digitalWrite( pinREG, on? HIGH : LOW );
}

inline byte getSenyalRd1() {
	return ( PINB & mascaraSenyalRd1 ) != 0 ? 1 : 0;
}

inline byte getSenyalWr1() {
	return ( PINB & mascaraSenyalWr1 ) != 0 ? 1 : 0;
}

inline byte getSenyalWr0() {
	return ( PINB & mascaraSenyalWr0 ) != 0 ? 1 : 0;
}

void inicializarInterfaz() {

	// El estado por defecto es el bus de datos en tristate

	busDatosTriState();

	pinMode( pinRD, OUTPUT );
	pinMode( pinWR, OUTPUT );
	pinMode( pinREG, OUTPUT );
	setRD( 0 );
	setWR( 0 );
	setREG( 0 );
	pinMode( pinSenyalRd1, INPUT );
	pinMode( pinSenyalWr1, INPUT );
	pinMode( pinSenyalWr0, INPUT );

}

inline void escribirRegistro( byte registro, byte valor ) {

	setREG( registro );
	PORTC = valor;
	busDatosOutput();
	setWR( 1 );
	setWR( 0 );
	busDatosTriState();

}

inline byte leerRegistro( byte registro ) {

	setREG( registro );
	setRD( 1 );
	byte valor = PINC;
	setRD( 0 );

	return valor;
}

int recibirCadenaBytes() {

	// Recibe dos bytes que forman un entero N de 16 bits (LSB primero)
	// A continuacion recibe esa cantidad de N bytes en el bufer.
	// Se devuelve N (N >= 0)

	// Lee registro de datos para borrar flags
	leerRegistro( REGISTRO_DATOS );

	byte b0 = 0, b1 = 0;
	
	// Listo para recibir
	escribirRegistro( REGISTRO_CONTROL, UC_LISTO_RECIBIR );

	// Lee primer byte del entero
	while ( ! getSenyalWr1() ) {}
	b0 = leerRegistro( REGISTRO_DATOS );

	// Lee segundo byte del entero
	while ( ! getSenyalWr1() ) {}
	b1 = leerRegistro( REGISTRO_DATOS );

	// Construye entero numBytes con los dos bytes
	int numBytes = ( ( ((int)b1) << 8 ) & 0xFF00 ) | ( ((int)b0) & 0x00FF );
	numBytes = min( numBytes, TAM_BUFER - 3 );

	// Recibe numBytes bytes
	for ( int i = 0; i < numBytes; i++ ) {
		while ( ! getSenyalWr1() ) {}
		bufer[ i ] = leerRegistro( REGISTRO_DATOS );
	}

	debugPrint( "Recibida cadena de bytes: " );
	debugPrintln( numBytes );

	return numBytes;
}

void transmitirCadenaBytes( int numBytes, byte valorRegControl ) {

	// Transmite dos bytes que forman un entero N de 16 bits (LSB primero)
	// A continuacion transmite esa cantidad de N bytes desde el bufer.
	// Durante la transmision se pone el valorRegControl en el registro  de control
	// ( ha de tener los cuatro bits bajos a 0)

	debugPrint( "Se transmitira cadena de bytes: " );
	debugPrintln( numBytes );

	byte b02 = numBytes & 0x00FF;
	byte b12 = ( numBytes >> 8 ) & 0x00FF;

	escribirRegistro( REGISTRO_DATOS, b02 );
	
	// Listo para transmitir
	escribirRegistro( REGISTRO_CONTROL, UC_LISTO_TRANSMITIR | valorRegControl );

	while ( ! getSenyalRd1() ) {}
	escribirRegistro( REGISTRO_DATOS, b12 );

	for ( int i = 0; i < numBytes; i++ ) {
		while ( ! getSenyalRd1() ) {}
		escribirRegistro( REGISTRO_DATOS, bufer[ i ] );
	}
}


// Funciones de instruccion

int activarDesactivarDebug( bool activar ) {
	debugActivado = activar;
	moduloWiFi.activarDesactivarDebug( activar );
	return debugActivado ? 1 : 0;
}

int instruccionConectarAWiFi() {
	
	int numBytes = recibirCadenaBytes();

	// Se espera SSID PASSWORD en el bufer
	
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

		numReintentos--;

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

int instruccionPeticionGet( int* numBytesRespuesta  ) {

	int numBytes = recibirCadenaBytes();

	// Asegura que el bufer termina en byte nulo (0)
	bufer[ numBytes ] = 0;

	int codigoError = moduloWiFi.peticionHttpGet( bufer, numBytesRespuesta );
	
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

	inicializarInterfaz();

	// Inicializa registro de control como no listo
	escribirRegistro( REGISTRO_CONTROL, 0 );

	// Genera reloj para los leds
	pinMode( pinRelojLED, OUTPUT );
	analogWrite( pinRelojLED, 127 );
	
	/*
	Setting 	Divisor 	Frequency
	0x01 	 	1  		31372.55
	0x02 	 	8 	 	3921.16
	0x03  		32  		980.39
	0x04 	 	64 	 	490.20   <--DEFAULT
	0x05 	 	128  		245.10
	0x06  		256  		122.55
	0x07 	 	1024  		30.64
	*/
	byte prescalerSetting = 0x07;
	TCCR2B = ( TCCR2B & 0b11111000 ) | prescalerSetting;
	//TCCR2 = ( TCCR2 & 0b11111000 ) | prescalerSetting;

	// Serial
	SerialDebug.begin( 9600 );

	// Modulo WiFi
	SerialWifi.begin( 9600 );
	moduloWiFi.configurar( &SerialWifi, &SerialDebug, pinResetModuloWiFi, bufer, TAM_BUFER );

	// Mensaje de inicio
	debugPrint( "Yombonet " );
	debugPrintln( REVISION );
	debugPrintln();

	// Pone en el registro de control que está listo para atender peticiones
	escribirRegistro( REGISTRO_CONTROL, UC_LISTO );

}

// Loop
void loop() {
	
	// Si el Spectrum ha escrito en el registro de control...
	if ( getSenyalWr0() ) {

		// Lee la instruccion del registro de control
		byte instruccion = leerRegistro( REGISTRO_CONTROL );

		debugPrint( "Recibida Instruccion: " );
		debugPrintln( instruccion );

		int codigoError = 0;
		bool transmitirRespuesta = false;
		int numBytesRespuesta = 0;

		// Decodificacion de la instruccion
		switch ( instruccion ) {
			case 0b00000000:
				// Obtener numero de revision del firmware
				numBytesRespuesta = sprintf( (char*)bufer, REVISION ) + 1;
				transmitirRespuesta = true;
				break;
			case 0b00000001:
				// Activar/desactivar debug serie
				codigoError = activarDesactivarDebug( leerRegistro( REGISTRO_DATOS ) == 0 ? false: true );
				break;
			case 0b00000010:
				// Peticion conexion a wifi
				codigoError = instruccionConectarAWiFi();
				break;
			case 0b00000100:
				// Peticion HTTP GET
				codigoError = instruccionPeticionGet( &numBytesRespuesta );
				transmitirRespuesta = true;
				break;
			default:
				debugPrint( "Instruccion no reconocida: " );
				debugPrintln( instruccion );
		}

		byte valorRegControl = ( codigoError << 4 ) & 0xF0;

		if ( transmitirRespuesta ) {
			transmitirCadenaBytes( numBytesRespuesta, valorRegControl );
			debugPrintln( "Transmitido el bufer." );
		}

		// En el registro de control escribe bit de que esta listo, y 
		// en los cuatro bits altos pone el resultado de la operacion
		escribirRegistro( REGISTRO_CONTROL, UC_LISTO | valorRegControl );
	}
}

