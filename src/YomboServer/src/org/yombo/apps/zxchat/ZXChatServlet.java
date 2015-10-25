package org.yombo.apps.zxchat;

import java.io.IOException;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class ZXChatServlet
 */
@WebServlet("/ZXChat/chat")
public class ZXChatServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
       
    /**
     * @see HttpServlet#HttpServlet()
     */
    public ZXChatServlet() {
        super();
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet( HttpServletRequest request, HttpServletResponse response ) throws ServletException, IOException {
		doPost( request, response );
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost( HttpServletRequest request, HttpServletResponse response ) throws ServletException, IOException {
		
		/*
		ZXChat.log( "-----------------------");
		ZXChat.log( "Peticion HTTP");
		for ( String param: request.getParameterMap().keySet() ) {
			ZXChat.log( "Param: " + param + ", value = " + request.getParameter( param ) );
		}
		*/

		String accion = request.getParameter( "accion" );

		if ( "enviar".equals( accion ) ) {
			String nick = request.getParameter( "nick" );
			String mensaje = request.getParameter( "mensaje" );
			ZXChat.getInstance().anyadirMensaje( nick, mensaje );
		}
		else if ( "listar".equals( accion ) ) {
			String timestamp = request.getParameter( "timestamp" );
			Date fecha = null; 
			try {
				fecha = new Date( Long.parseLong( timestamp.trim() ) );
			}
			catch ( NumberFormatException e ) {
				fecha = new Date( 0 );
			}
			response.setContentType( "text/plain" );
			ServletOutputStream os = response.getOutputStream();
			ZXChat.getInstance().listarMensajes( fecha, os );
			os.close();
			//ZXChat.log( "Mensajes listados." );
		}
	}

}
