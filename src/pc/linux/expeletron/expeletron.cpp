/*
	expeletron.c

Librerias necesarias:
libxrender-dev libxext-dev mesa-common-dev libglu1-mesa-dev

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/shm.h>
#include <signal.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/extensions/Xrender.h>
#include <X11/extensions/XShm.h>

#include <X11/keysym.h>

#include "timeutils.h"

#include "serialport.h"

using namespace std;

// Defines
#define ZX_SCREEN_RES_X 256
#define ZX_SCREEN_RES_Y 192
#define ZX_SCREEN_NUM_PIXELS ( ZX_SCREEN_RES_X * ZX_SCREEN_RES_Y )
#define ZX_SCREEN_NUM_BYTES_SCREEN ( ZX_SCREEN_NUM_PIXELS / 8 )
#define ZX_SCREEN_BLOCKS_X ( ZX_SCREEN_RES_X / 8 )
#define ZX_SCREEN_BLOCKS_Y ( ZX_SCREEN_RES_Y / 8 )

// Variables

//	int debug1;

// Flag de terminar
bool flagFinalizar;

// Zoom, cuanto mayor mas grande es el area cubierta
float zoom;

// Para acceder al "escritorio", la ventana raiz
Display* dpy;
Window rootwin;
XWindowAttributes rootattr;

// Variables de imagen
XRenderPictFormat* srcformat;
XRenderPictFormat* dstformat;
Picture srcpicture;
Picture dstpicture;
XRenderPictureAttributes pictattr;
XTransform transform;
Pixmap pixmap;
XShmSegmentInfo shmseginfo;
XImage* xim;

// Numero de frame
int numFrame;

// Bitmap
uint8_t *zxBitmap;

// Bufer para calcular el histograma
int *histograma;


// Para grabar SCR (debug)
uint8_t *atributosSCR;

// Puerto serie
std::string STRING_PUERTO_SERIE = "/dev/ttyUSB0";
int BAUDIOS_PUERTO_SERIE = 115200;
CSerialPort puertoSerie;

// Prototipos de funciones
char* inicializar(void);
void finalizar(void);
void actualizarDimensiones();
void capturarFrame();
void procesarFrame();
void senyalHandler( int signum );
void grabarSCR( uint8_t *bitmap );
void grabarPPM();
bool obtenerRaton( int *x, int *y );
int buscarNivelMedio();
void obtenerImagenSpectrum( uint8_t nivel );
bool getBitmapBoolean( int x, int y, uint8_t nivel );
uint8_t getBitmapLuminancia( int x, int y );
void simularTecla( int keycode );
void simularTecla( int keycode, int modificadores );

int main ( void ) {


//debug1 = 0;
	
	printf( "\n\nExpeletron v1.0\n" );

	char *error = inicializar();
	if ( error != NULL ) {
		printf("Error al inicializar: %s", error );
		return 0;
	}

	while ( ! flagFinalizar ) {

		uint8_t b = 0;
		// Espera byte de instruccion
		// Tercer parametro timeout usecs (100000 = 0.1 seg)
		if ( puertoSerie.Read( &b, 1, 100000 ) == 1 ) {

			// Instruccion de capturar frame de escritorio
			if ( b == 34 ) {

				capturarFrame();
				
				procesarFrame();

				numFrame++;

			}
		}

		/*
		if ( numFrame > 3 ) {
			flagFinalizar = true;
		}
		*/
		
		//USleep( 1000000 / 2 );

	}
	
	finalizar();
	
	printf( "\nFin.\n" );
	
	return 0;
} 


char* inicializar( void ) {
	
	flagFinalizar = false;

	zoom = 1.0f;
	//zoom = 1.7f;

	numFrame = 0;
	zxBitmap = new uint8_t[ ZX_SCREEN_NUM_BYTES_SCREEN ];
	atributosSCR = NULL;
	
	histograma = new int[ 256 ];

	// Engancha handler de senyales
	signal( SIGTERM, senyalHandler );
	signal( SIGINT, senyalHandler );

	
	// Inicializa puerto serie
	if ( ! puertoSerie.Open( STRING_PUERTO_SERIE, BAUDIOS_PUERTO_SERIE ) ) {
		return (char*)"No se pudo abrir el puerto serie.";
	}	
	
	// Inicializa captura de escritorio
	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL) {
		return (char*)"Error opening the display";
	}

	rootwin = RootWindow( dpy, DefaultScreen( dpy ) );
	actualizarDimensiones();

	srcformat = NULL;
	dstformat = NULL;
	pixmap = 0;
	srcpicture = None;
	dstpicture = None;

	memset( &pictattr, 0, sizeof( pictattr ) );
	pictattr.repeat = RepeatNone;

	transform.matrix[0][0] = 0;
	transform.matrix[0][1] = 0;
	transform.matrix[0][2] = 0;
	transform.matrix[1][0] = 0;
	transform.matrix[1][1] = 0;
	transform.matrix[1][2] = 0;
	transform.matrix[2][0] = 0;
	transform.matrix[2][1] = 0;
	transform.matrix[2][2] = 0;

	int eventbase;
	int errorbase;
	int majorver;
	int minorver;
	if ( ! XRenderQueryExtension( dpy, &eventbase, &errorbase) ) {
		return (char*)"Some XRender extension is missing :(";
	}
	if ( !XRenderQueryVersion( dpy, &majorver, &minorver ) ) {
		return (char*)"Insufficient XRender version :(";
	}

	pixmap = XCreatePixmap( dpy, rootwin, ZX_SCREEN_RES_X, ZX_SCREEN_RES_Y, rootattr.depth );

	srcformat = XRenderFindVisualFormat( dpy, rootattr.visual );
	dstformat = XRenderFindVisualFormat( dpy, rootattr.visual );

	srcpicture = XRenderCreatePicture( dpy, rootwin, srcformat, CPRepeat, &pictattr );
	dstpicture = XRenderCreatePicture( dpy, pixmap,  dstformat, CPRepeat, &pictattr );

	// "bilinear" o "nearest":
	XRenderSetPictureFilter( dpy, srcpicture, "nearest", NULL, 0 );

	xim = XShmCreateImage( dpy, rootattr.visual, rootattr.depth, ZPixmap, NULL, &shmseginfo, ZX_SCREEN_RES_X, ZX_SCREEN_RES_Y );
	shmseginfo.shmid = shmget( IPC_PRIVATE, xim->bytes_per_line * xim->height, IPC_CREAT | 0777);
	shmseginfo.shmaddr = reinterpret_cast<char*>( shmat( shmseginfo.shmid, NULL, 0 ) );
	xim->data = shmseginfo.shmaddr;
	shmseginfo.readOnly = False;
	XShmAttach( dpy, &shmseginfo );

	return NULL;
}

void finalizar( void ) {

	puertoSerie.Close();

	XRenderFreePicture( dpy, srcpicture );
	XRenderFreePicture( dpy, dstpicture );

	XFreePixmap( dpy, pixmap );

	if ( dpy ) {
		XCloseDisplay( dpy );
		dpy = NULL;
	}

	
}

void actualizarDimensiones() {
	XGetWindowAttributes( dpy, rootwin, &rootattr );
}

void capturarFrame() {

	actualizarDimensiones();

	// Escala segun zoom
	int escalaX = (int)( zoom * ZX_SCREEN_RES_X );
	int escalaY = (int) ( zoom * ZX_SCREEN_RES_Y * 256.0f / 192.0f );
	int escalaYSinAspecto = (int) ( zoom * ZX_SCREEN_RES_Y );

	// Obtiene coordenadas del raton
	int src_x;
	int src_y;
	if ( obtenerRaton( &src_x, &src_y ) ) {

		int width = rootattr.width;
		int height = rootattr.height;
		int deltaX = escalaX / 2;
		int deltaY = escalaYSinAspecto / 2;

		int x = src_x - deltaX;
		int y = src_y - deltaY;
		
		// Limita la parte capturada a estar dentro de la ventana raiz
		if ( x < 0 ) {
			x = 0;
		}
		if ( x + 2 * deltaX > width ) {
			x = width - 2 * deltaX;
		}
		if ( y < 0 ) {
			y = 0;
		}
		if ( y + 2 * deltaY > height ) {
			y = height - 2 * deltaY;
		}

		src_x = (int)( x / zoom );
		src_y = (int)( y / zoom );

		transform.matrix[0][0] = escalaX;
		transform.matrix[1][1] = escalaY;
		transform.matrix[2][2] = ZX_SCREEN_RES_X;

		//printf( "rootattr.width=%d\n", rootattr.width );
		//printf( "rootattr.height=%d\n", rootattr.height );

		XRenderSetPictureTransform( dpy, srcpicture, &transform );

		int mask_x = 0;
		int mask_y = 0;

		// Renderiza la imagen del escritorio en nuestra imagen
		XRenderComposite( dpy, PictOpSrc, srcpicture, None, dstpicture, src_x, src_y, mask_x, mask_y, 0, 0, ZX_SCREEN_RES_X, ZX_SCREEN_RES_Y );
		XSync( dpy, False );
 
		// Copia el pixmap a la ximage en memoria compartida
		XShmGetImage( dpy, pixmap, xim, 0, 0, AllPlanes );
	}
}

bool getBitmapBoolean( int x, int y, uint8_t nivel ) {

	return getBitmapLuminancia( x, y ) > nivel;

}

uint8_t getBitmapLuminancia( int x, int y ) {

	unsigned long rgb = XGetPixel( xim, x, y );
	
	int r = ( rgb & 0xFF0000 ) >> 16;
	int g = ( rgb & 0xFF00 ) >> 8;
	int b = ( rgb & 0xFF );
	
	int l = ( r + g + b ) / 3;
	
	//uint8_t l = 140;
	//return r > l || g > l || b > l;
	
	return (uint8_t)l;
}

int buscarNivelMedio() {

	for ( int i = 0; i < 256; i++ ) {
		histograma[ i ] = 0;
	}

	for ( int j = 0; j < ZX_SCREEN_RES_Y; j++ ) {
		for ( int i = 0; i < ZX_SCREEN_RES_X; i++ ) {
			histograma[ getBitmapLuminancia( i, j ) ]++;
		}
	}
	
	int nivelBuscado = (int)( ZX_SCREEN_RES_X * ZX_SCREEN_RES_Y * 0.5f );

	int nivel = 0;
	for ( int i = 0; i < 256; i++ ) {
		nivel += histograma[ i ];
		if ( nivel >= nivelBuscado ) {
			return i;
		}
	}
	
	return 255;

}

void procesarFrame() {

	int nivel = buscarNivelMedio();
	
	nivel = max( 10, min( 250, nivel ) );
	
	obtenerImagenSpectrum( nivel );

	// Envia el bitmap por el puerto serie
	if ( puertoSerie.Write( zxBitmap, ZX_SCREEN_NUM_BYTES_SCREEN ) == -1 ) {
		printf("Error al escribir al puerto serie." );
	}
	
	
	printf( "\nFotograma %d procesado. nivel = %d\n", numFrame, nivel );

//	grabarPPM();
//	grabarSCR( zxBitmap );


/*
	if ( debug1 == 0 ) {
		simularTecla( XK_H, ShiftMask );
	}
	else if ( debug1 == 1 ) {
		simularTecla( XK_o, ShiftMask );
	}
	else if ( debug1 == 2 ) {
		simularTecla( XK_l, ShiftMask );
	}
	else if ( debug1 == 3 ) {
		simularTecla( XK_a, ShiftMask );
	}
	else if ( debug1 == 4 ) {
		simularTecla( XK_space );
	}
	else if ( debug1 == 5 ) {
		simularTecla( XK_m );
	}
	else if ( debug1 == 6 ) {
		simularTecla( XK_u );
	}
	else if ( debug1 == 7 ) {
		simularTecla( XK_n );
	}
	else if ( debug1 == 8 ) {
		simularTecla( XK_d );
	}
	else if ( debug1 == 9 ) {
		simularTecla( XK_o );
	}
	else if ( debug1 == 10 ) {
		simularTecla( XK_exclam, ShiftMask );
	}
	else if ( debug1 == 11 ) {
		simularTecla( XK_exclam, ShiftMask );
	}
	else if ( debug1 == 9 ) {
		simularTecla( XK_exclam, ShiftMask );
	}
	else {
		debug1 = -1;
	}
	debug1++;
*/


}

void obtenerImagenSpectrum( uint8_t nivel ) {

	// Obtiene en zxBitmap la imagen entrelazada de spectrum
	int tercioEntrelazado = 0;
	int bloqueEntrelazado = 0;
	int filaEntrelazado = 0;
	int jEntrelazado = 0;
	for (int j = 0; j < ZX_SCREEN_RES_Y; j++) {
		for (int i = 0; i < ZX_SCREEN_RES_X; i+=8) {

			uint8_t b = (uint8_t)( (getBitmapBoolean(i, jEntrelazado, nivel )? 0x80 : 0) |
									(getBitmapBoolean(i+1, jEntrelazado, nivel)? 0x40 : 0) |
									(getBitmapBoolean(i+2, jEntrelazado, nivel)? 0x20 : 0) |
									(getBitmapBoolean(i+3, jEntrelazado, nivel)? 0x10 : 0) |
									(getBitmapBoolean(i+4, jEntrelazado, nivel)? 0x08 : 0) |
									(getBitmapBoolean(i+5, jEntrelazado, nivel)? 0x04 : 0) |
									(getBitmapBoolean(i+6, jEntrelazado, nivel)? 0x02 : 0) |
									(getBitmapBoolean(i+7, jEntrelazado, nivel)? 0x01 : 0) );
			zxBitmap[ j * 32 + i / 8 ] = b;
		}
		
		bloqueEntrelazado++;
		if (bloqueEntrelazado == ZX_SCREEN_RES_Y / 8 / 3) {
			bloqueEntrelazado = 0;
			filaEntrelazado++;
			if ( filaEntrelazado == 8 ) {
				filaEntrelazado = 0;
				tercioEntrelazado++;
			}
		}
		jEntrelazado = tercioEntrelazado * (ZX_SCREEN_RES_Y / 3) + bloqueEntrelazado * 8 + filaEntrelazado;
	}
}

void grabarSCR( uint8_t *bitmap ) {

	FILE *pFile;
	char szFilename[ 1000 ];

	// Open file
	sprintf( szFilename, "/media/datos3/Devel/raspberrypi/Raspbian/pruebascaptura/2/Frame_%d.scr", numFrame );
	pFile = fopen( szFilename, "wb" );
	if( pFile == NULL ) {
		return;
	}

	if ( atributosSCR == NULL ) {
		// Crea atributos. papel negro, tinta blanca, bright
		uint8_t ink = 7;
		uint8_t paper = 0;
		uint8_t atributo = ink | ( paper << 3 ) | 0x04;
		atributosSCR = new uint8_t[ 768 ];
		for ( int i = 0; i < 768; i++ ) {
			atributosSCR[ i ] = atributo;
		}
	}

	fwrite( bitmap, ZX_SCREEN_NUM_BYTES_SCREEN, 1, pFile);
	fwrite( atributosSCR, 768, 1, pFile);

	// Close file
	fclose(pFile);

}


void grabarPPM() {

	FILE *pFile;
	char szFilename[ 1000 ];

	// Open file
	sprintf( szFilename, "/media/datos3/Devel/raspberrypi/Raspbian/pruebascaptura/2/Frame_%d.ppm", numFrame );
	pFile = fopen( szFilename, "wb" );
	if( pFile == NULL ) {
		return;
	}

	// Write header
	fprintf( pFile, "P6\n%d %d\n255\n", ZX_SCREEN_RES_X, ZX_SCREEN_RES_Y );

	// Write pixel data
	for( int y = 0; y < ZX_SCREEN_RES_Y; y++ ) {
		for( int x = 0; x < ZX_SCREEN_RES_X; x++ ) {

			unsigned long rgb = XGetPixel( xim, x, y );	

			uint8_t r = ( rgb & 0xFF0000 ) >> 16;
			uint8_t g = ( rgb & 0xFF00 ) >> 8;
			uint8_t b = ( rgb & 0xFF );

			fwrite( &r, 1, 1, pFile);
			fwrite( &g, 1, 1, pFile);
			fwrite( &b, 1, 1, pFile);
			
		}
	}

	// Close file
	fclose(pFile);

}

bool obtenerRaton( int *x, int *y ) {
	
	Window rootResult;
	Window childResult;
	int winxResult, winyResult;
	unsigned int maskResult;
	return XQueryPointer( dpy, rootwin, &rootResult, &childResult, x, y, &winxResult, &winyResult, &maskResult );

}

XKeyEvent crearKeyEvent( Display *display, Window &win, Window &winRoot, bool press, int keycode, int modifiers ) {
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

void simularTecla( int keycode ) {
	simularTecla( keycode, 0 );
}

void simularTecla( int keycode, int modificadores ) {

	// Los keycodes estan en /usr/include/X11/keysymdef.h
	// Los modificadores posibles son ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask, y Mod5Mask
	
	// Find the window which has the current keyboard focus.
	Window winFocus;
	int revert;
	XGetInputFocus( dpy, &winFocus, &revert );

	// Send a fake key press event to the window.
	XKeyEvent event = crearKeyEvent( dpy, winFocus, rootwin, true, keycode, modificadores );
	XSendEvent( event.display, event.window, True, KeyPressMask, (XEvent *)&event );

	// Send a fake key release event to the window.
	event = crearKeyEvent( dpy, winFocus, rootwin, false, keycode, modificadores );
	XSendEvent( event.display, event.window, True, KeyPressMask, (XEvent *)&event );

}

void senyalHandler( int signum ) {
	flagFinalizar = true;
}
