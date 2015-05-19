<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>ZX Chat</title>
</head>
<body>

	<form method="post">
	<div>
	<textarea name="textAreaChat" id="textAreaChat" readonly="readonly" cols="80" rows="25"></textarea>
	</div>
	<input type="checkbox" name="chatActivado" id="chatActivado" checked>Refrescar chat<br>
	</form>
	
	<form method="post" action="javascript:enviarMensaje()">
		Nick (3 a 15 caracteres): <input type="text" name="nick" id="nick" size=15 maxlength=15> <br>
		Mensaje: <input type="text" name="mensaje" id="mensaje" size=40 maxlength=255>
		<input type="submit" value="Enviar">
	</form>

	<script>

		var strUltimoTimestamp = "0";
		var taChat = document.getElementById( "textAreaChat" );
		var txtNick = document.getElementById( "nick" );
		var txtMensaje = document.getElementById( "mensaje" );
		var chkChatActivado = document.getElementById( "chatActivado" );
		
		txtNick.focus();

		var enviarMensaje = function() {
			if ( txtNick.value == "" ) {
				alert( 'Por favor, introduce primero tu NICK' );
				return;
			}
			var textoMensaje = txtMensaje.value;
			if ( textoMensaje.length == 0 ) {
				return;
			}
			if ( textoMensaje.length > 255 ) {
				textoMensaje = textoMensaje.substring( 255 );
			}
			peticionGET( 'chat?accion=enviar&nick=' + txtNick.value +
							'&mensaje=' + textoMensaje,
							null, null );
			txtMensaje.value = "";
		}
		
		window.setInterval( "recargarChat();", 2000 );

		function recargarChat() {
			if ( chkChatActivado.checked ) {
				peticionGET( 'chat?accion=listar&timestamp=' + strUltimoTimestamp, callbackRespuesta, callbackError );
			}
		}

		var callbackRespuesta = function( mensaje ) {
			// Parsea timestamp (primera linea)
			var posLinea = mensaje.indexOf( '\n' );
			var t = null;
			if ( posLinea == -1 ) {
				t = mensaje;
			}
			else {
				t = mensaje.substring( 0, posLinea );
			}
			strUltimoTimestamp = t;
			if ( mensaje.length > posLinea ) {
				// Anyade mensajes al chat
				taChat.value += mensaje.substring( posLinea + 1 );
				// Scroll hasta abajo del todo
				taChat.scrollTop = taChat.scrollHeight;
			}
		}

		var callbackError = function( mensaje ) {
			taChat.value += "\nError al acceder al chat: " + mensaje + "\n";
		}

		function peticionGET( url, funcionCallbackExito, funcionCallbackError ) {
			var peticion = new XMLHttpRequest();
			peticion.open( "GET", url, true );
			if ( funcionCallbackExito ) {
				peticion.onload = function ( e ) {
					if ( peticion.readyState === 4 ) {
						if ( peticion.status === 200 ) {
							funcionCallbackExito( peticion.responseText );
						} else {
							funcionCallbackError( peticion.statusText );
						}
					}
				};
			}
			if ( funcionCallbackError ) {
				peticion.onerror = function( e ) {
					funcionCallbackError( peticion.statusText );
				};
			}
			peticion.send( null );
		}
		
	</script>

</body>
</html>