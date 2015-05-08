
#ifndef __ENTRADA_SALIDA_TEXTO__

#define __ENTRADA_SALIDA_TEXTO__

#include "pixelScroll.bas"
#include "Putchars.bas"
#include "bufer.bas"

sub scrollArriba()

	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )
	PixelScrollUp( 1 )

end sub

function esperarTecla() as string
	dim tecla as string
	tecla = ""
	while tecla = ""
		tecla = inkey$
	end while
	while inkey$ <> ""
	end while
	return tecla
end function

function imprimirCadenaWrap( cad as string, x0 as integer, y0 as integer, imprimirSoloUltimoCaracter as ubyte, byref xf as integer, byref yf as integer ) as integer

	' x0 e y0 son caracteres desde esquina sup-izq
	' xf e yf tambien. Se devuelve la posicion del ultimo caracter escrito (normalmente un espacio)
	' devuelve numero de scrolls hechos (lineas)

	dim x as integer
	dim y as integer
	dim numCars as integer
	dim i as integer
	dim numScrolls as integer
	numScrolls = 0
	
	x = x0
	y = y0
	
	while y >= 24
		scrollArriba()
		y = y - 1
		numScrolls = numScrolls + 1
	end while

	numCars = len( cad )
	if numCars > 0 then
		for i = 0 to numCars - 1
			if imprimirSoloUltimoCaracter = 0 or i = numCars - 1 then
				print at y,x; cad( i )
			end if
			x = x + 1
			if x >= 32 then
				x = 0
				y = y + 1
				'if y >= 24 then
				'	i = numCars + 1
				'end if
				while y >= 24
					scrollArriba()
					y = y - 1
					numScrolls = numScrolls + 1
				end while
			end if
		next i
	end if

	if i <= numCars then
		print at y, x; " "
	end if

	xf = x
	yf = y
	
	return numScrolls

end function

function imprimirCadenaBuferWrap( indiceInicio as integer, tamCadena as integer, x0 as integer, y0 as integer, byref xf as integer, byref yf as integer ) as integer

	' x0 e y0 son caracteres desde esquina sup-izq
	' xf e yf tambien. Se devuelve la posicion del ultimo caracter escrito (normalmente un espacio)
	' devuelve numero de scrolls hechos (lineas)

	dim x as integer
	dim y as integer
	dim i as integer
	dim c as ubyte
	dim numScrolls as integer
	numScrolls = 0
	
	x = x0
	y = y0
	
	while y >= 24
		scrollArriba()
		y = y - 1
		numScrolls = numScrolls + 1
	end while

	if tamCadena > 0 then
		for i = 0 to tamCadena - 1
			c = bufer( i + indiceInicio )
			if c = 13 then
				x = 0
				y = y + 1
			else
				if c <> 10 then
					print at y,x; chr( c )
					x = x + 1
					if x >= 32 then
						x = 0
						y = y + 1
					end if
				end if
			end if
			while y >= 24
				scrollArriba()
				y = y - 1
				numScrolls = numScrolls + 1
			end while
		next i
	end if

	xf = x
	yf = y
	
	return numScrolls

end function

sub borrarCadenaWrap( cad as string, x0 as integer, y0 as integer )

	' x0 e y0 son caracteres desde esquina sup-izq
	' xf e yf tambien. Se devuelve la posicion del ultimo caracter escrito (normalmente un espacio)
	' devuelve numero de scrolls hechos (lineas)

	dim x as integer
	dim y as integer
	dim numCars as integer
	dim i as integer
	dim numScrolls as integer
	numScrolls = 0
	
	x = x0
	y = y0

	numCars = len( cad )
	if numCars > 0 then
		for i = 0 to numCars - 1
			print at y,x; " "
			x = x + 1
			if x >= 32 then
				x = 0
				y = y + 1
				'if y >= 24 then
				'	i = numCars + 1
				'end if
				if y >= 24 then
					i = numCars
				end if
			end if
		next i
	end if

end sub

function leerCadenaEntrada( cadena as string, byref posYCadena as integer ) as string

	' Enter termina la cadena, backspc o shift + 0 para borrar.

	dim cad1 as string
	dim car as ubyte
	dim carAnt as ubyte
	dim estado as ubyte
	dim imprimir as ubyte
	dim ultCarX as integer
	dim ultCarY as integer
	dim ultCursorX as integer
	dim ultCursorY as integer

	dim posYCopia as integer
	dim numScrolls as integer

	posYCopia = posYCadena
	ultCarX = 0
	ultCarY = posYCopia
	if cadena <> "" then
		numScrolls = imprimirCadenaWrap( cadena, 0, posYCopia, 0, ultCarX, ultCarY )
		posYCopia = posYCopia - numScrolls
	end if

	' Pone cursor flash
	ultCursorX = ultCarX
	ultCursorY = ultCarY
	paint( ultCarX, ultCarY, 1, 1, 10111000b)

	' Estado: 0 nada, 1 escribir letra, 2 borrar letra
	estado = 0
	
	bucleLeerCaracter:

	imprimir = 0

	cad1 = inkey$

	if cad1 <> "" then

		car = code( cad1( 0 ) )

		if car = 13 then
			goto finBucleLeerCaracter
		end if
		
		if car = 12 then
			estado = 2
		else
			if estado = 0 then
				carAnt = car
				estado = 1
			end if
		end if
	else
		if estado = 1 then
			if carAnt <> 0 then
				cadena = cadena + chr( carAnt )
			end if
			estado = 0
			imprimir = 1
		end if 
		if estado = 2 then
			if len( cadena ) > 1 then
				cadena = cadena(TO len( cadena ) - 2 )
			else
				cadena = ""
			end if
			estado = 0
			imprimir = 1
		end if
	end if

	if imprimir <> 0 then

		numScrolls = imprimirCadenaWrap( cadena, 0, posYCopia, 1, ultCarX, ultCarY )
		posYCopia = posYCopia - numScrolls

		' Pone cursor con flash
		paint( ultCursorX, ultCursorY, 1, 1, 00000111b)
		paint( ultCarX, ultCarY, 1, 1, 10000111b)
		ultCursorX = ultCarX
		ultCursorY = ultCarY

	end if
	
	goto bucleLeerCaracter

	finBucleLeerCaracter:
	
	' Quita cursor flash
	paint( ultCursorX, ultCursorY, 1, 1, 00000111b)

	borrarCadenaWrap( cadena, 0, posYCopia )

	posYCadena = posYCopia

	return cadena

end function
 
function reemplazarCadena( cadena as string, caracterAReemplazar as ubyte, cadenaQueReemplaza as string ) as string

	dim cadenaDevuelta as string
	cadenaDevuelta = ""
	
	dim n as uinteger
	n = len( cadena )
	
	dim c as ubyte
	dim i as uinteger
	for i = 0 to n - 1
		c = code( cadena( i ) )

		if c = caracterAReemplazar then
			cadenaDevuelta = cadenaDevuelta + cadenaQueReemplaza
		else
			cadenaDevuelta = cadenaDevuelta + chr( c )
		end if
	next i
	
	return cadenaDevuelta

end function

#endif