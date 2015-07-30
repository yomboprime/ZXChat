/**
 * 
 *  Copyright (C) 2010  Juan Jose Luna Espinosa juanjoluna@gmail.com

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3 of the License.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  
 * Log de mensajes
 */
package org.yombo.log;

import java.util.Vector;
import java.io.PrintStream;

public class Log {

    private static Vector<InterfaceLog> interfacesLog = new Vector<InterfaceLog>();

    public static void addLogger( InterfaceLog logger ) {
        interfacesLog.add( logger );
    }
    
    public static void log( String mensaje ) {
        for ( int i = 0; i < interfacesLog.size(); i ++ ) {
            interfacesLog.elementAt( i ).logMensaje( mensaje );
        }
    }
    
    public static void setStdErrorNull() {
        
        // Hace que System.err no haga nada
        
        OutputStreamNull osNull = new OutputStreamNull();
        System.setErr( new PrintStream( osNull, false ) );
        
    }

}
