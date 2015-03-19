/*

Interfaz Spectrum

Microcontrolador: ATMEGA1284P (Mighty1284) @ 16MHz

CPLD Interfaz spectrum:

Datos: D0..D7 = PC0..PC7

RD CPLD: pin 12
WR CPLD: pin 13
REG CPLD: pin 14

SalidaPWMLEDs 15

SenyalRd1: pin 0
SenyalWr1: pin 1
SenyalWr0: pin 2

Reset modulo wifi: pin 3


Serial para debug con PC a 19200 baudios
Serial1 para comunicacion con ESP8266 a 9600 baudios
*/

#include "ModuloWiFi.h"

/*
Significado del registro de control en direccion uC -> Spectrum:
*/
#define REGISTRO_CONTROL 0
#define REGISTRO_DATOS 1


#define UC_LISTO 1
#define UC_LISTO_RECIBIR 2
#define UC_LISTO_TRANSMITIR 4
/*
b0	0 = uC busy, 1 = uC ready, escuchando nuevo comando.
b1  uC Listo para recibir datos
b2  uC Listo para transmitir datos
b3
b4
b5
b6
b7


Instrucciones
-------------

0x34	El Spectrum envia una cadena de 10000 bytes
0x35	El Spectrum recibe una cadena de 10000 bytes


*/

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

#define nop1() __asm__("nop\n\t")
#define nopn( n ) for ( int i = 0; i < n; i++ ) { __asm__("nop\n\t"); }


// Buferes
#define TAM_BUFER 15000
byte bufer[ TAM_BUFER ];

// Modulo WiFi
ModuloWiFi moduloWiFi;


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

	// Serial
	Serial.begin( 19200 );

	
	// Modulo WiFi
	Serial1.begin( 9600 );
	moduloWiFi.configurar( &Serial1, pinResetModuloWiFi, bufer, TAM_BUFER );

//	Serial.println( "Inicio" );
//	Serial.println();

/*
	while ( Serial.available() == 0 ) {}

	Serial.println( "Empezando" );
	Serial.println();
	Serial.println();

	while ( Serial.available() > 0 ) {
		Serial.read();
	}
*/
//	Serial.println( "Empezando" );



	bool conectadoOk = false;
	while ( ! conectadoOk ) {

		Serial.println( "\nReiniciando WiFi..." );

		if ( moduloWiFi.reiniciar() ) {
			Serial.println( "\nWiFi reiniciada satisfactoriamente." );
		}
		else {
			Serial.println( "\nNo se ha recibido respuesta al reiniciar." );
		}

		esperar( 1 );

		Serial.println( "\nConectando a Wifi..." );
		if ( moduloWiFi.conectarAWifi( (uint8_t*)"JAZZTEL", (uint8_t*)"elcorraldelapacheca" ) ) {
			Serial.println( "\nConectado a Wifi Satisfactoriamente." );
			conectadoOk = true;
		}
		else {
			Serial.println( "\nError al conectar a la wifi." );
		}
	}

	
	// Pone en el registro de control que está listo para atender peticiones
	escribirRegistro( REGISTRO_CONTROL, UC_LISTO );


}

void esperar( unsigned long segundos ) {
	Serial.print( "\nEsperando " );
	Serial.print( segundos );
	Serial.println( " segundos..." );
	delay( segundos * 1000 );
}

// Loop de prueba de alimentacion externa
void loop() {

	Serial.println( "\nPulse tecla..." );
	while ( Serial.available() == 0 ) {}
	Serial.read();

	sprintf( (char*)bufer, "192.168.0.196:8080/YomboServer/ZXChat/chat?accion=listar&timestamp=0" );
	int numBytes = 0;
	int errorPeticion = moduloWiFi.peticionHttpGet( bufer, &numBytes );
	if ( errorPeticion == 0 ) {
		Serial.print( "\nPeticion Http Respondida OK. Longitud datos = " );
		Serial.println( numBytes );
		Serial.print( "DATOS: -->" );
		bufer[numBytes] = 0;
		Serial.print( (char*)bufer );
		Serial.print( "<--FIN DATOS" );
	}
	else {
		Serial.print( "\nPeticion HTTP FALLADA. Error=" );
		Serial.println( errorPeticion );
	}

/*	while ( Serial.available() > 0 ) {
		Serial.read();
	}
*/
}


/*
// Segunda prueba wifi con interface de software
void loop() {
	
	Serial.println( "*********************************************" );
	Serial.println( "\nPulse una tecla..." );
	while ( Serial.available() == 0 ) {}
	while ( Serial.available() > 0 ) {
		Serial.read();
	}

	Serial.println( "\nReiniciando WiFi..." );

	if ( moduloWiFi.reiniciar() ) {
		Serial.println( "\nWiFi reiniciada satisfactoriamente." );
	}
	else {
		Serial.println( "\nNo se ha recibido respuesta al reiniciar." );
	}

	esperar( 1 );
	
	Serial.println( "\nConectando a Wifi..." );
	if ( moduloWiFi.conectarAWifi( (uint8_t*)"JAZZTEL", (uint8_t*)"elcorraldelapacheca" ) ) {
		Serial.println( "\nConectado a Wifi Satisfactoriamente." );
	}
	else {
		Serial.println( "\nError al conectar a la wifi." );
	}
	
	esperar( 1 );

	Serial.println( "\nHaciendo peticion al chat..." );
	int l = 0;
	sprintf( (char*)bufer, "192.168.0.196:8080/ZXChat/chat?accion=listar&timestamp=0" );
	if ( moduloWiFi.peticionHttpGet( bufer, &l ) ) {
		Serial.print( "\n[[[[****]]]]Peticion Http Respondida OK. Longitud datos = " );
		Serial.println( l );
		Serial.print( "DATOS: -->" );
		bufer[l] = 0;
		Serial.print( (char*)bufer );
		Serial.print( "<--FIN DATOS" );
	}
	else {
		Serial.println( "\nPeticion HTTP FALLADA." );
	}
	
	esperar( 1 );
	
	Serial.println( "\nDesconectado de Wifi..." );
	if ( moduloWiFi.desconectarWifi() ) {
		Serial.println( "\nDesconectado de Wifi Satisfactoriamente." );
	}
	else {
		Serial.println( "\nError al desconectar de wifi." );
	}

	while ( Serial.available() > 0 ) {
		Serial.read();
	}
}
*/

/*
// Primera prueba wifi con interface de software
void loop() {

	Serial.println( "Empezando..." );
	
	Serial.println( "Reiniciando WiFi..." );

	moduloWiFi.reiniciar();
	
	Serial.println( "WiFi reiniciada." );

	Serial.println( "Entrando en comunicacion directa..." );

	while ( 1 ) {
		if ( Serial1.available() > 0 ) {
			Serial.write( Serial1.read() );
		}
		if ( Serial.available() > 0 ) {
			uint8_t b = Serial.read();
			if ( b == '\n' ) {
				Serial1.write( '\r' );
				Serial1.write( '\n' );
			}
			else {
				Serial1.write( b );
			}
		}
	}
}
*/

/*
// Prueba comunicacion Spectrum
void loop() {
	
	// Si el Spectrum ha escrito en el registro de control...
	if ( getSenyalWr0() ) {

		// Lee el registro de control
		byte instruccion = leerRegistro( 0 );

		// Decodificacion de la instruccion
		if ( instruccion == 0 ) {

			// Instruccion de transmitir un bitmap de pantalla al spectrum

			Serial.write( (uint8_t)34 );

			for ( int i = 0; i < 6144; i++ ) {
				while ( Serial.available() == 0 ) {}
				bufer[ i ] = Serial.read();
			}

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

			Serial.println( "Recibida instruccion 34." );

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
			
			Serial.print( "Recibidos " );
			Serial.print( n );
			Serial.println( " bytes del Spectrum." );
			Serial.println( "Los 15 primeros son:" );
			
			for ( int i = 0; i < 15 ; i++ ) {
				Serial.println( (int)bufer[ i ] );
			}
			Serial.println();

			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
		else if ( instruccion == 35 ) {
			
			// Instruccion de transmitir 15000 bytes al spectrum

			Serial.println( "Recibida instruccion 35." );

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

			Serial.print( "Transmitidos " );
			Serial.print( n );
			Serial.println( " bytes al Spectrum." );
			unsigned long microsPasados = t1 - t0;
			Serial.print( "Tiempo: " );
			Serial.print( microsPasados );
			Serial.println( " microsegundos." );
			float seg = microsPasados / 1000000.0f;
			Serial.print( "Tiempo: " );
			Serial.print( seg );
			Serial.println( " segundos." );
			float bytesSeg = n / seg;
			Serial.print( "Velocidad de transferencia: " );
			Serial.print( bytesSeg );
			Serial.println( " bytes/segundo." );


			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
		else if ( instruccion == 36 ) {
			
			// Instruccion de recibir un entero N de dos bytes y a continuacion N bytes,
			// y de responder igualmente con otro paquete variable

			Serial.println( "Recibida instruccion 36." );

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

			Serial.print( "Recibida cadena de bytes: " );
			Serial.println( numBytes );

			//Serial.print( "Cadena recibida:" );
			//Serial.println( (char*)bufer );

			//int numBytes2 = sprintf( (char*)bufer,"Hola mundooo!!!" ) + 1;
			int numBytes2 = 0;
			int errorPeticion = moduloWiFi.peticionHttpGet( bufer, &numBytes2 );
			if ( errorPeticion == 0 ) {
				Serial.print( "\nPeticion Http Respondida OK. Longitud datos = " );
				Serial.println( numBytes2 );
				//Serial.print( "DATOS: -->" );
				//bufer[numBytes2] = 0;
				//Serial.print( (char*)bufer );
				//Serial.print( "<--FIN DATOS" );
			}
			else {
				Serial.print( "\nPeticion HTTP FALLADA. Error=" );
				Serial.println( errorPeticion );
				numBytes2 = 0;
			}

			Serial.print( "Se transmitira cadena de bytes: " );
			Serial.println( numBytes2 );

			byte b02 = numBytes2 & 0x00FF;
			byte b12 = ( numBytes2 >> 8 ) & 0x00FF;

			escribirRegistro( REGISTRO_DATOS, b02 );
			
			// Listo para transmitir
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO_TRANSMITIR );

			while ( ! getSenyalRd1() ) {}
			escribirRegistro( REGISTRO_DATOS, b12 );

			for ( int i = 0; i < numBytes2; i++ ) {
				while ( ! getSenyalRd1() ) {}
				escribirRegistro( REGISTRO_DATOS, bufer[ i ] );
			}

			Serial.println( "Transmitido el bufer." );
			
			escribirRegistro( REGISTRO_CONTROL, UC_LISTO );
		}
	}
}
*/