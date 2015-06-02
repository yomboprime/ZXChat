package org.yombo.apps.zxchat;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;

import javax.servlet.ServletOutputStream;

public class ZXChat {
	
	public static final int MAX_LONGITUD_MENSAJE = 768; 
	
	private static ZXChat zxChatSingleton;

	public static ZXChat getInstance() {
		if ( zxChatSingleton == null ) {
			zxChatSingleton = new ZXChat();
			
			zxChatSingleton.mensajesDePrueba();
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
			if ( mensajes.size() > 500 ) {
				for ( int i = 0; i < 250; i++ ) {
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
			}
			else {

				int longitudMensajes = 0;
				int indicePrimerMensaje = -1;
				int indiceUltimoMensaje = -1;
				// Obtiene el timestamp del ultimo mensaje
				String stringTimestamp = "" + mensajes.get( mensajes.size() - 1 ).timestamp.getTime();
				boolean fechaSobrepasada = false;
				// Bucle para ver la ventana de mensajes que devolvemos
				for ( int i = 0; i < mensajes.size(); i++ ) {

					if ( indicePrimerMensaje != -1 && indiceUltimoMensaje != -1 && ( indiceUltimoMensaje - indicePrimerMensaje + 1 ) > 24 ) {
						// Maximo 24 mensajes, 24 lineas
						break;
					}

					Mensaje m = mensajes.get( i ); 

					if ( desdeFecha.compareTo( m.timestamp ) < 0 ) {
						
						if ( ! fechaSobrepasada ) {
							indicePrimerMensaje = i;
							indiceUltimoMensaje = i;
							fechaSobrepasada = true;
						}

						String stringTimestampTemp = "" + m.timestamp.getTime();
						int longitudCabecera = stringTimestamp.length() + 2; // 2 bytes de fin de linea
						int longitudMensaje = m.nick.length() + 2 + m.mensaje.length() + 2; // 2 bytes del ": " y 2 bytes de fin de linea
						int longitudMensajesCabecera = longitudMensajes + longitudMensaje + longitudCabecera;
						if ( longitudMensajesCabecera > MAX_LONGITUD_MENSAJE ) {
							// Hemos terminado de procesar mensajes porque excederian la capacidad de visualizacion del Spectrum.
							break;
						}
						// Aceptamos el mensaje
						longitudMensajes += longitudMensaje;
						indiceUltimoMensaje = i;
						// El timestamp pasa a ser el del ultimo mensaje aceptado (que se va a devolver en la salida)
						stringTimestamp = stringTimestampTemp;
					}
				}
				
				// Envia a la salida la cabecera con timestamp del ultimo mensaje
				os.println( stringTimestamp );

				// Bucle para enviar a la salida los mensajes
				int longitudTotal = 0;
				if ( indicePrimerMensaje != -1 && indiceUltimoMensaje != -1 ) {
					for ( int i = indicePrimerMensaje; i <= indiceUltimoMensaje; i++ ) {
						Mensaje m = mensajes.get( i );
						os.println( m.nick + ": " + m.mensaje );
						longitudTotal += ( m.nick + ": " + m.mensaje ).length() + 2;
					}
				}
				
				log( "Se devuelven " + longitudTotal + " bytes." );
				
				
				
				/*
				// Imprime timestamp del ultimo mensaje
				String stringTimestamp = "" + mensajes.get( mensajes.size() - 1 ).timestamp.getTime(); 
				os.println( stringTimestamp );


				// Imprime mensajes posteriores a desdeFecha
				boolean fechaSobrepasada = false;
				for ( Mensaje m: mensajes ) {
					if ( ! fechaSobrepasada && desdeFecha.compareTo( m.timestamp ) < 0 ) {
						fechaSobrepasada = true;
					}
					if ( fechaSobrepasada ) {
						os.println( m.nick + ": " + m.mensaje );
					}
				}
				*/
			}
		} catch ( IOException e ) {
			log( "listarMensajes: Error de IO: " + e );
		}
	}
	
	public static void log( String mensaje ) {
		System.out.println( "(" + new Date() + ") " + mensaje );
	}
	
	public void mensajesDePrueba() {
		for ( int i = 0; i < 100; i++ ) {
			// Cada linea son 20 bytes
			anyadirMensaje( "Server__", String.format( "%08d", i ) );
		}
	}
}
