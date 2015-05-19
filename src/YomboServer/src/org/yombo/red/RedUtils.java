package org.yombo.red;

import java.net.URL;
import java.net.HttpURLConnection;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.InputStream;

public class RedUtils {


	public static StringBuffer peticionHttp( String urlString ) {

		try {
			URL url = new URL( urlString );
			HttpURLConnection con = (HttpURLConnection)url.openConnection();
			InputStream is = con.getInputStream();
			InputStreamReader isr = new InputStreamReader( is );
			BufferedReader br = new BufferedReader( isr );

			StringBuffer sb = new StringBuffer();
			for ( String linea = br.readLine(); linea != null; linea = br.readLine() ) {
				sb.append( linea );
				sb.append( '\n' );
			}

			br.close();
			is.close();

			return sb;

		} catch ( IOException e ) {
			return null;
		}
	}

}
