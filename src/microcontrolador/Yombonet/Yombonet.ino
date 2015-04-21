/*

Yombonet v1.0

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
Esta memoria son cuatro registros de un byte cada uno. Hay visibles dos registros tanto para
el micro como para el Spectrum: Un registro de control y uno de datos.  El registro de control 
en el que escribe el Spectrum es el registro de control que lee el microcontrolador, y viceversa.
Y lo mismo para el registro de datos.
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

Instrucciones de Internet
-------------------------

Codigo instruccion: 0b00000000
Descripcion: Obtener numero de revision del firmware
	Se devuelve una cadena de bytes con la descripcion del numero de revision.

Codigo instruccion: 0b00000001
Descripcion: Conectar a Wifi
	Se espera una cadena de bytes "SSID PASSW" donde
	SSID es el SSID de la Wifi a la que se quiere conectar, y PASSW el password.
	Se devuelve una cadena de bytes de longitud 0 si hubo error (y el codigo de
	error esta disponible en el registro de control), o la cadena con la IP obtenida
	en la Wifi si hubo exito.
	Codigos de error:
		0	Exito
		1	Error al conectar

Codigo instruccion: 0b00000010
Descripcion: Preguntar si hay conexion
	El microcontrolador responde con un unico byte en el registro de datos, 00000000 si no estamos
	conectados	o 00000001 si lo estamos.

Codigo instruccion: 0b00000011
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

#include <SoftwareSerial.h>
#include "ModuloWiFi.h"


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


// Definiciones y variables globales

// Bufer
#define TAM_BUFER 770
byte bufer[ TAM_BUFER ];

// Modulo WiFi
ModuloWiFi moduloWiFi;

// Serial del modulo Wifi
#define SerialWifi Serial1
//#define SerialWifi Serial

// Serial para debug
SoftwareSerial SerialDebug( 8, 9 ); // 8,9 en ATMEGA1284P
//SoftwareSerial SerialDebug( 10, 11 ); //10,11 en Atmega32A


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

void setup() {

	inicializarInterfaz();

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
	moduloWiFi.configurar( &SerialWifi, pinResetModuloWiFi, bufer, TAM_BUFER );

//	SerialDebug.println( "Inicio" );
//	SerialDebug.println();

/*
	while ( SerialDebug.available() == 0 ) {}

	SerialDebug.println( "Empezando" );
	SerialDebug.println();
	SerialDebug.println();

	while ( SerialDebug.available() > 0 ) {
		SerialDebug.read();
	}
*/
//	SerialDebug.println( "Empezando" );



	bool conectadoOk = false;
	while ( ! conectadoOk ) {

		SerialDebug.println( "\nReiniciando WiFi..." );

		if ( moduloWiFi.reiniciar() ) {
			SerialDebug.println( "\nWiFi reiniciada satisfactoriamente." );
		}
		else {
			SerialDebug.println( "\nNo se ha recibido respuesta al reiniciar." );
		}

		esperar( 1 );

		SerialDebug.println( "\n___Conectando a Wifi..." );
		if ( moduloWiFi.conectarAWifi( (uint8_t*)"yombo", (uint8_t*)"ninobravo" ) ) {
			SerialDebug.println( "\nConectado a Wifi Satisfactoriamente." );
			conectadoOk = true;
		}
		else {
			SerialDebug.println( "\nError al conectar a la wifi." );
		}
	}

	
	// Pone en el registro de control que está listo para atender peticiones
	escribirRegistro( REGISTRO_CONTROL, UC_LISTO );


}

void esperar( unsigned long segundos ) {
	SerialDebug.print( "\nEsperando " );
	SerialDebug.print( segundos );
	SerialDebug.println( " segundos..." );
	delay( segundos * 1000 );
}


/*
// Loop de prueba de alimentacion externa
void loop() {

	SerialDebug.println( "\nPulse tecla..." );
	while ( SerialDebug.available() == 0 ) {}
	SerialDebug.read();

	//sprintf( (char*)bufer, "192.168.1.100:8080/YomboServer/ZXChat/chat?accion=listar&timestamp=0" );
	sprintf( (char*)bufer, "88.20.17.84:8080/YomboServer/ZXChat/chat?accion=listar&timestamp=0" );

	int numBytes = 0;
	int errorPeticion = moduloWiFi.peticionHttpGet( bufer, &numBytes );
	if ( errorPeticion == 0 ) {
		SerialDebug.print( "\nPeticion Http Respondida OK. Longitud datos = " );
		SerialDebug.println( numBytes );
		SerialDebug.print( "DATOS: -->" );
		bufer[numBytes] = 0;
		SerialDebug.print( (char*)bufer );
		SerialDebug.print( "<--FIN DATOS" );
	}
	else {
		SerialDebug.print( "\nPeticion HTTP FALLADA. Error=" );
		SerialDebug.println( errorPeticion );
	}

//	while ( SerialDebug.available() > 0 ) {
//		SerialDebug.read();
//	}
}
*/

/*
// Segunda prueba wifi con interface de software
void loop() {
	
	SerialDebug.println( "*********************************************" );
	SerialDebug.println( "\nPulse una tecla..." );
	while ( SerialDebug.available() == 0 ) {}
	while ( SerialDebug.available() > 0 ) {
		SerialDebug.read();
	}

	SerialDebug.println( "\nReiniciando WiFi..." );

	if ( moduloWiFi.reiniciar() ) {
		SerialDebug.println( "\nWiFi reiniciada satisfactoriamente." );
	}
	else {
		SerialDebug.println( "\nNo se ha recibido respuesta al reiniciar." );
	}

	esperar( 1 );
	
	SerialDebug.println( "\nConectando a Wifi..." );
	if ( moduloWiFi.conectarAWifi( (uint8_t*)"JAZZTEL", (uint8_t*)"elcorraldelapacheca" ) ) {
		SerialDebug.println( "\nConectado a Wifi Satisfactoriamente." );
	}
	else {
		SerialDebug.println( "\nError al conectar a la wifi." );
	}
	
	esperar( 1 );

	SerialDebug.println( "\nHaciendo peticion al chat..." );
	int l = 0;
	sprintf( (char*)bufer, "192.168.0.196:8080/ZXChat/chat?accion=listar&timestamp=0" );
	if ( moduloWiFi.peticionHttpGet( bufer, &l ) ) {
		SerialDebug.print( "\n[[[[****]]]]Peticion Http Respondida OK. Longitud datos = " );
		SerialDebug.println( l );
		SerialDebug.print( "DATOS: -->" );
		bufer[l] = 0;
		SerialDebug.print( (char*)bufer );
		SerialDebug.print( "<--FIN DATOS" );
	}
	else {
		SerialDebug.println( "\nPeticion HTTP FALLADA." );
	}
	
	esperar( 1 );
	
	SerialDebug.println( "\nDesconectado de Wifi..." );
	if ( moduloWiFi.desconectarWifi() ) {
		SerialDebug.println( "\nDesconectado de Wifi Satisfactoriamente." );
	}
	else {
		SerialDebug.println( "\nError al desconectar de wifi." );
	}

	while ( SerialDebug.available() > 0 ) {
		SerialDebug.read();
	}
}
*/

/*
// Primera prueba wifi con interface de software
void loop() {

	SerialDebug.println( "Empezando..." );
	
	SerialDebug.println( "Entrando en comunicacion directa..." );

	while ( 1 ) {
		if ( SerialWifi.available() > 0 ) {
			SerialDebug.write( SerialWifi.read() );
		}
		if ( SerialDebug.available() > 0 ) {
			uint8_t b = SerialDebug.read();
			if ( b == '\n' ) {
				SerialWifi.write( '\r' );
				SerialWifi.write( '\n' );
			}
			else {
				SerialWifi.write( b );
			}
		}
	}
}
*/


// Prueba comunicacion Spectrum
void loop() {
	
	// Si el Spectrum ha escrito en el registro de control...
	if ( getSenyalWr0() ) {

		// Lee el registro de control
		byte instruccion = leerRegistro( 0 );

		// Decodificacion de la instruccion
		if ( instruccion == 0 ) {

			// Instruccion de transmitir un bitmap de pantalla al spectrum

			//SerialDebug.write( (uint8_t)34 );

			//for ( int i = 0; i < 6144; i++ ) {
			//	while ( SerialDebug.available() == 0 ) {}
			//	bufer[ i ] = SerialDebug.read();
			//}

			escribirRegistro( REGISTRO_DATOS, bufer[ 0 ] );
			
			// Listo para transmitir
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO_TRANSMITIR );

			for ( int i = 1; i < 6144; i++ ) {
				while ( ! getSenyalRd1() ) {}
				escribirRegistro( REGISTRO_DATOS, bufer[ i ] );
			}

			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
		else if ( instruccion == 34 ) {
			
			// Instruccion de recibir 10 bytes

			SerialDebug.println( "Recibida instruccion 34." );

			// Lee registro de datos para borrar flags
			leerRegistro( REGISTRO_DATOS );

			// Recibe una cadena
			int n = 15000;

			// Listo para recibir
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO_RECIBIR );
			
			for ( int i = 0; i < n; i++ ) {
				while ( ! getSenyalWr1() ) {}
				bufer[ i ] = leerRegistro( REGISTRO_DATOS );
			}
			
			SerialDebug.print( "Recibidos " );
			SerialDebug.print( n );
			SerialDebug.println( " bytes del Spectrum." );
			SerialDebug.println( "Los 15 primeros son:" );
			
			for ( int i = 0; i < 15 ; i++ ) {
				SerialDebug.println( (int)bufer[ i ] );
			}
			SerialDebug.println();

			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
		else if ( instruccion == 35 ) {
			
			// Instruccion de transmitir 15000 bytes al spectrum

			SerialDebug.println( "Recibida instruccion 35." );

			// Transmite una cadena de bytes
			int n = 15000;
			for ( int i = 0; i < n; i++ ) {
				bufer[ i ] = i;
			}

			unsigned long t0 = micros();
			
			escribirRegistro( REGISTRO_DATOS, bufer[ 0 ] );
			
			// Listo para transmitir
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO_TRANSMITIR );

			for ( int i = 1; i < n; i++ ) {
				while ( ! getSenyalRd1() ) {}
				escribirRegistro( REGISTRO_DATOS, bufer[ i ] );
			}
			
			unsigned long t1 = micros();

//			SerialDebug.print( "Transmitidos " );
//			SerialDebug.print( n );
//			SerialDebug.println( " bytes al Spectrum." );
//			unsigned long microsPasados = t1 - t0;
//			SerialDebug.print( "Tiempo: " );
//			SerialDebug.print( microsPasados );
//			SerialDebug.println( " microsegundos." );
//			float seg = microsPasados / 1000000.0f;
//			SerialDebug.print( "Tiempo: " );
//			SerialDebug.print( seg );
//			SerialDebug.println( " segundos." );
//			float bytesSeg = n / seg;
//			SerialDebug.print( "Velocidad de transferencia: " );
//			SerialDebug.print( bytesSeg );
//			SerialDebug.println( " bytes/segundo." );


			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
		else if ( instruccion == 36 ) {
			
			// Instruccion de recibir un entero N de dos bytes y a continuacion N bytes,
			// y de responder igualmente con otro paquete variable

			SerialDebug.println( "Recibida instruccion 36." );

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
			numBytes = min( numBytes, TAM_BUFER );

			// Recibe numBytes bytes
			for ( int i = 0; i < numBytes; i++ ) {
				while ( ! getSenyalWr1() ) {}
				bufer[ i ] = leerRegistro( REGISTRO_DATOS );
			}

			SerialDebug.print( "Recibida cadena de bytes: " );
			SerialDebug.println( numBytes );

			//SerialDebug.print( "Cadena recibida:" );
			//SerialDebug.println( (char*)bufer );

			//int numBytes2 = sprintf( (char*)bufer,"Hola mundooo!!!" ) + 1;
			int numBytes2 = 0;
			int errorPeticion = moduloWiFi.peticionHttpGet( bufer, &numBytes2 );
			if ( errorPeticion == 0 ) {
				SerialDebug.print( "\nPeticion Http Respondida OK. Longitud datos = " );
				SerialDebug.println( numBytes2 );
				//SerialDebug.print( "DATOS: -->" );
				//bufer[numBytes2] = 0;
				//SerialDebug.print( (char*)bufer );
				//SerialDebug.print( "<--FIN DATOS" );
			}
			else {
				SerialDebug.print( "\nPeticion HTTP FALLADA. Error=" );
				SerialDebug.println( errorPeticion );
				numBytes2 = 0;
			}

			SerialDebug.print( "Se transmitira cadena de bytes: " );
			SerialDebug.println( numBytes2 );

			byte b02 = numBytes2 & 0x00FF;
			byte b12 = ( numBytes2 >> 8 ) & 0x00FF;

			escribirRegistro( REGISTRO_DATOS, b02 );
			
			// Listo para transmitir
			byte valorRegControl = ( errorPeticion << 4 ) & 0xF0;
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO_TRANSMITIR | valorRegControl );

			while ( ! getSenyalRd1() ) {}
			escribirRegistro( REGISTRO_DATOS, b12 );

			for ( int i = 0; i < numBytes2; i++ ) {
				while ( ! getSenyalRd1() ) {}
				escribirRegistro( REGISTRO_DATOS, bufer[ i ] );
			}

			SerialDebug.println( "Transmitido el bufer." );
			
			// En el registro de control escribe bit de que esta listo, y 
			// en los cuatro bits altos pone el resultado de la operacion
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO | valorRegControl );
		}
	}
}
