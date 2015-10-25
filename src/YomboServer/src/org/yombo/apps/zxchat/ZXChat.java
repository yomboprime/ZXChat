package org.yombo.apps.zxchat;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import javax.servlet.ServletOutputStream;

public class ZXChat {
	
	public static final int MAX_LONGITUD_MENSAJE = 767;

	private static ZXChat zxChatSingleton;

	public static ZXChat getInstance() {
		if ( zxChatSingleton == null ) {
			zxChatSingleton = new ZXChat();
			
			zxChatSingleton.anyadirMensaje( "Server", "Bienvenidos a ZXChat" );
			
			//zxChatSingleton.mensajesDePrueba();
		}

		return zxChatSingleton;
	}

	public class Mensaje {
		public String nick;
		public Date timestamp;
		public String mensaje;
		
		public Mensaje( String nick, String mensaje, Date timestamp ) {
			this.nick = nick;
			this.mensaje = mensaje;
			this.timestamp = timestamp;
		}
	}
	
	public ArrayList<Mensaje> mensajes;
	
	public static int longitudFecha = 12;
	public SimpleDateFormat dateFormat = new SimpleDateFormat( "dd MM HH:mm " );

	public ZXChat() {
		mensajes = new ArrayList<Mensaje>();
	}
	
	public boolean chequearNick( String nick ) {
		
		// Ojo! El nick debe tener tres caracteres minimo y 15 maximo
		
		return nick != null && nick.length() >= 3 && nick.length() <= 15 && ! nick.contains( ":" ) && ! nick.contains( "\n" );
	}
	
	public synchronized boolean anyadirMensaje( String nick, String mensaje ) {

		if ( chequearNick( nick ) && mensaje != null && mensaje.length() <= MAX_LONGITUD_MENSAJE ) {  
		
			mensajes.add( new Mensaje( nick, mensaje, new Date() ) );
			
			log( "Mensaje anyadido. Nick: " + nick );
			
			// Purga mensajes antiguos si se llega a cierto limite
			if ( mensajes.size() > 50 ) {
				for ( int i = 0; i < 25; i++ ) {
					mensajes.remove( 0 );
				}
			}

			return true;
		}
		else {

			log( "Se intento enviar un mensaje no valido o nick no valido. Nick: \"" + nick + "\"");
			
			return false;
		}
	}
	
	public synchronized void listarMensajes( Date desdeFecha, ServletOutputStream os ) {

		// La primera linea de la salida es el timestamp del ultimo mensaje. Despues vienen los mensajes.

		try {

			if ( mensajes.size() == 0 ) {
				os.println( "0" );
				return;
			}

			int indicePrimerMensaje = -1;
			int indiceUltimoMensaje = -1;
			// Obtiene el timestamp del ultimo mensaje
			Date timestampSeleccionado = mensajes.get( mensajes.size() - 1 ).timestamp;
			String stringTimestamp = "" + timestampSeleccionado.getTime();

			if ( desdeFecha.getTime() == 0 ) {

				// Recorremos los mensajes al reves para obtener los ultimos

				int longitudMensajes = 0;
				indiceUltimoMensaje = mensajes.size() - 1;
				int longitudCabecera = stringTimestamp.length() + 2; // 2 bytes de fin de linea
				for ( int i = indiceUltimoMensaje; i >= 0; i-- ) {

					if ( indicePrimerMensaje != -1 && indiceUltimoMensaje != -1 && ( indiceUltimoMensaje - indicePrimerMensaje + 1 ) > 24 ) {
						// Maximo 24 mensajes, 24 lineas
						break;
					}

					Mensaje m = mensajes.get( i ); 

					if ( desdeFecha.compareTo( m.timestamp ) >= 0 ) {
						// A partir de este, todos los mensajes seran anteriores a desdeFecha
						break;
					}
						
					int longitudMensaje = longitudFecha + m.nick.length() + 2 + m.mensaje.length() + 2; // 2 bytes del ": " y 2 bytes de fin de linea
					int longitudMensajesCabecera = longitudMensajes + longitudMensaje + longitudCabecera;
					if ( longitudMensajesCabecera > MAX_LONGITUD_MENSAJE ) {
						// Hemos terminado de procesar mensajes porque excederian la capacidad de visualizacion del Spectrum.
						break;
					}

					// Aceptamos el mensaje
					longitudMensajes += longitudMensaje;
					indicePrimerMensaje = i;
				}
			}
			else {
				int longitudMensajes = 0;
				boolean fechaSobrepasada = false;
				// Bucle para ver la ventana de mensajes que devolvemos
				for ( int i = 0; i < mensajes.size(); i++ ) {

					if ( indicePrimerMensaje != -1 && indiceUltimoMensaje != -1 && ( indiceUltimoMensaje - indicePrimerMensaje + 1 ) > 24 ) {
						// Maximo 24 mensajes, 24 lineas
						break;
					}

					Mensaje m = mensajes.get( i ); 

					if ( desdeFecha.compareTo( m.timestamp ) < 0 ) {
						
						Date timestampTemp = m.timestamp;
						String stringTimestampTemp = "" + timestampTemp.getTime();
						int longitudCabecera = stringTimestampTemp.length() + 2; // 2 bytes de fin de linea
						int longitudMensaje = longitudFecha + m.nick.length() + 2 + m.mensaje.length() + 2; // 2 bytes del ": " y 2 bytes de fin de linea
						int longitudMensajesCabecera = longitudMensajes + longitudMensaje + longitudCabecera;
						if ( longitudMensajesCabecera > MAX_LONGITUD_MENSAJE ) {
							// Hemos terminado de procesar mensajes porque excederian la capacidad de visualizacion del Spectrum.
							break;
						}

						// Aceptamos el mensaje
						if ( ! fechaSobrepasada ) {
							indicePrimerMensaje = i;
							fechaSobrepasada = true;
						}
						longitudMensajes += longitudMensaje;
						indiceUltimoMensaje = i;
						// El timestamp pasa a ser el del ultimo mensaje aceptado (que se va a devolver en la salida)
						timestampSeleccionado = timestampTemp; 
						stringTimestamp = stringTimestampTemp;
					}
				}
			}

			// Envia a la salida la cabecera con timestamp del ultimo mensaje
			//os.println( "\r\n\r\n" );
			os.println( stringTimestamp );

			// Bucle para enviar a la salida los mensajes
			int longitudTotal = 0;
			if ( indicePrimerMensaje != -1 && indiceUltimoMensaje != -1 ) {
				for ( int i = indicePrimerMensaje; i <= indiceUltimoMensaje; i++ ) {
					
					Mensaje m = mensajes.get( i );
					
					String strMensaje = dateFormat.format( m.timestamp ) + m.nick + ": " + m.mensaje;
							
					os.println( strMensaje );

					longitudTotal += strMensaje.length() + 2;
				}
			}
			
			longitudTotal += stringTimestamp.length() + 2; // 2 bytes de fin de linea
			
			log( "Se devuelven " + longitudTotal + " bytes." );

		} catch ( IOException e ) {
			log( "listarMensajes: Error de IO: " + e );
		}
	}
	
	public static void log( String mensaje ) {
		System.out.println( "(" + new Date() + ") " + mensaje );
	}
	
	public void mensajesDePrueba() {
		for ( int i = 0; i < 50; i++ ) {
			anyadirMensaje( "Server", String.format( "%08d", i ) );
			// 3 anyadirMensaje( "Server", "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" );
			
			try {
				Thread.sleep( 10 );
			} catch (InterruptedException e) {
			}
		}
	}
}
