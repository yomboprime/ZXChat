----------------------------------------------------------------------------------
-- Company: Yomboprime Technologies
-- Engineer: Yombo
-- 
-- Create Date:    03:12:24 06/26/2014 
-- Design Name: 
-- Module Name:    InterfazRobotSpectrum - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Incluye manipulacion numerica:
use IEEE.NUMERIC_STD.ALL;

entity InterfazRobotSpectrum is
    Port ( rdSinclair : in STD_LOGIC; -- read del spectrum - NEGADO
			  wrSinclair : in STD_LOGIC; -- write del spectrum - NEGADO
			  iorqSinclair : in STD_LOGIC; -- iorq del spectrum - NEGADO
			  addrSinclair : in  STD_LOGIC_VECTOR (2 DOWNTO 0); -- direccion del spectrum A7, A6, A0
			  busDatosSinclair : inout  STD_LOGIC_VECTOR (7 DOWNTO 0); -- bus datos bidireccional del spectrum
			  rdUc : in STD_LOGIC; -- read del microcontrolador - NEGADO
			  wrUc : in STD_LOGIC; -- write del microcontrolador - NEGADO
			  regUc : in STD_LOGIC; -- selecciona registro 0 o 1
			  busDatosUc : inout  STD_LOGIC_VECTOR (7 DOWNTO 0); -- bus datos bidireccional del microcontrolador
			  --rd0Signal : out STD_LOGIC:= '0'; --senyal de que el spectrum ha leido registro 0 sinclair
			  wr0Signal : out STD_LOGIC:= '0'; --senyal de que el spectrum ha escrito registro 0 sinclair
			  rd1Signal : out STD_LOGIC:= '0'; --senyal de que el spectrum ha leido registro 1 sinclair
			  wr1Signal : out STD_LOGIC:= '0'; --senyal de que el spectrum ha escrito registro 1 sinclair
			  ledActividadSinclair : out STD_LOGIC; -- NEGADO (LED activo a nivel bajo)
			  --ledActividadExterno0 : out STD_LOGIC; -- NEGADO (LED activo a nivel bajo)
			  --ledActividadExterno1 : out STD_LOGIC; -- NEGADO (LED activo a nivel bajo)
			  --actividadExterno0 : in STD_LOGIC;
			  --actividadExterno1 : in STD_LOGIC;
			  clkLed : in STD_LOGIC
			);
end InterfazRobotSpectrum;

architecture Behavioral of InterfazRobotSpectrum is

-- Registros de 8 bits
-- Hay dos del lado que escribe el Spectrum y otros dos del lado que escribe el microcontrolador.
-- Los registros 0 son los de opcode/status, los 1 son los de datos.
-- Los registros Spectrum son los que escribe el Spectrum, los Uc son los que escribe el microcontrolador.
-- El mapeo con la direccion de memoria del Spectrum es A7A6A0=001 para registro 0, A7A6A0=011 para registro 1 (junto con RD/WR y IORQ todos a 0)
signal registro0Spectrum : STD_LOGIC_VECTOR (7 DOWNTO 0) := "00000000";
signal registro1Spectrum : STD_LOGIC_VECTOR (7 DOWNTO 0) := "00000000";
signal registro0Uc : STD_LOGIC_VECTOR (7 DOWNTO 0) := "00000000";
signal registro1Uc : STD_LOGIC_VECTOR (7 DOWNTO 0) := "00000000";

-- Contadores para que los LEDs sigan activos un momento
signal contadorLedSinclair: STD_LOGIC_VECTOR (2 DOWNTO 0) := "000";
--signal contadorLedExterno0: STD_LOGIC_VECTOR (2 DOWNTO 0) := "000";
--signal contadorLedExterno1: STD_LOGIC_VECTOR (2 DOWNTO 0) := "000";

-- Contador para hacer parpadear todos los LEDs (dividir la frecuencia de clkLed)
signal contadorLeds: STD_LOGIC_VECTOR (1 DOWNTO 0) := "00";

begin

process( rdSinclair, wrSinclair, iorqSinclair, addrSinclair, busDatosSinclair,
			rdUc, wrUc, regUc, busDatosUc,
			registro0Spectrum, registro1Spectrum,
			registro0Uc, registro1Uc,
			contadorLeds,
			contadorLedSinclair,
			--contadorLedExterno0,
			--contadorLedExterno1,
			--actividadExterno0,
			--actividadExterno1,
			clkLed )
begin

	-- Lectura desde el Spectrum registro 0
	if ( iorqSinclair = '0' and rdSinclair = '0' and wrSinclair = '1' and addrSinclair = "000" ) then
		busDatosSinclair <= registro0Uc;
--		rd0Signal <= '1';
	-- Lectura desde el Spectrum registro 1
	elsif ( iorqSinclair = '0' and rdSinclair = '0' and wrSinclair = '1' and addrSinclair = "011" ) then
		busDatosSinclair <= registro1Uc;
		rd1Signal <= '1';
	-- En cualquier otro caso ponemos el bus del Spectrum en alta impedancia
	else
		busDatosSinclair <= ( others => 'Z' );
	end if;


	-- Lectura desde el microcontrolador registro 0
	if ( rdUc = '0' and wrUc = '1' and regUc = '0' ) then
		busDatosUc <= registro0Spectrum;
		wr0Signal <= '0';
	-- Lectura desde el microcontrolador registro 1
	elsif ( rdUc = '0' and wrUc = '1' and regUc = '1' ) then
		busDatosUc <= registro1Spectrum;
		wr1Signal <= '0';
	-- En cualquier otro caso ponemos el bus del microcontrolador en alta impedancia
	else
		busDatosUc <= ( others => 'Z' );
	end if;


	-- Escritura desde el Spectrum registro 0
	if ( iorqSinclair = '0' and wrSinclair = '0' and rdSinclair = '1' and addrSinclair = "001" ) then
		registro0Spectrum <= busDatosSinclair;
		wr0Signal <= '1';
	-- Escritura desde el Spectrum registro 1
	elsif ( iorqSinclair = '0' and wrSinclair = '0' and rdSinclair = '1' and addrSinclair = "011" ) then
		registro1Spectrum <= busDatosSinclair;
		wr1Signal <= '1';
	end if;

	-- Escritura desde el microcontrolador registro 0
	if ( wrUc = '0' and rdUc = '1' and regUc = '0' ) then
		registro0Uc <= busDatosUc;
--		rd0Signal <= '0';
	-- Escritura desde el microcontrolador registro 1
	elsif ( wrUc = '0' and rdUc = '1' and regUc = '1' ) then
		registro1Uc <= busDatosUc;
		rd1Signal <= '0';
	end if;



	-- Incrementa el contador para hacer parpadear los leds
	if ( rising_edge( clkLed ) ) then
		contadorLeds <= std_logic_vector( to_unsigned( to_integer( unsigned( contadorLeds ) ) + 1, 2 ) );
	end if;


	-- Actualizacion del contador de actividad de Spectrum
	-- Si se esta accediendo al cpld...
	if ( ( iorqSinclair = '0' and ( rdSinclair = '0' or wrSinclair = '0' ) ) and 
		  ( ( addrSinclair = "001" ) or ( addrSinclair = "011" ) ) ) then
--	if ( rdUc = '0' or wrUc = '0' ) then
		-- Resetea el contador
		contadorLedSinclair <= "111";
	elsif ( rising_edge( clkLed ) and contadorLedSinclair /= "000" ) then
		-- Decrementa el contador
		contadorLedSinclair <= std_logic_vector( to_unsigned( to_integer( unsigned( contadorLedSinclair ) ) - 1, 3 ) );
	end if;

	-- Actualizacion del led de Spectrum (activo a nivel bajo)
	if ( contadorLedSinclair /= "000" ) then
		ledActividadSinclair <= not contadorLeds( 1 );
	else
		ledActividadSinclair <= '1';
	end if;

--	-- Actualizacion del contador de actividad externo 0
--	if ( actividadExterno0 = '1' ) then
--		-- Resetea el contador
--		contadorLedExterno0 <= "111";
--	elsif ( rising_edge( clkLed ) and contadorLedExterno0 /= "000" ) then
--		-- Decrementa el contador
--		contadorLedExterno0 <= std_logic_vector( to_unsigned( to_integer( unsigned( contadorLedExterno0 ) ) - 1, 3 ) );
--	end if;
--
--	-- Actualizacion del led externo 0
--	if ( contadorLedExterno0 /= "000" ) then
--		ledActividadExterno0 <= not contadorLeds( 1 );
--	else
--		ledActividadExterno0 <= '1';
--	end if;
--
--	-- Actualizacion del contador de actividad externo 1
--	if ( actividadExterno1 = '1' ) then
--		-- Resetea el contador
--		contadorLedExterno1 <= "111";
--	elsif ( rising_edge( clkLed ) and contadorLedExterno1 /= "000" ) then
--		-- Decrementa el contador
--		contadorLedExterno1 <= std_logic_vector( to_unsigned( to_integer( unsigned( contadorLedExterno1 ) ) - 1, 3 ) );
--	end if;
--
--	-- Actualizacion del led externo 1
--	if ( contadorLedExterno1 /= "000" ) then
--		ledActividadExterno1 <= not contadorLeds( 1 );
--	else
--		ledActividadExterno1 <= '1';
--	end if;


end process;

end Behavioral;

