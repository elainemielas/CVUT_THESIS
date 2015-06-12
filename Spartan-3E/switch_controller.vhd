----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    12:57:00 05/08/2015  
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    switch_controller - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity switch_controller is
generic(
	rf_addr_w : integer := 5;
	reg_width : integer := 32
);
port(
	clk : in std_logic;
	reset : in std_logic;
	sw0 : in std_logic;
	sw1 : in std_logic;
	sw2 : in std_logic;
	sw3 : in std_logic;
	rf_data : in std_logic_vector(reg_width - 1 downto 0);
	
	rf_addr : out std_logic_vector(rf_addr_w - 1 downto 0);
	led : out std_logic_vector(3 downto 0) 
);
end switch_controller;

architecture Behavioral of switch_controller is

begin
	rf_addr <= "01010";
	
	process(clk)
	begin
		if clk'event and clk = '1' then
			if reset = '1' then
				led(3 downto 0) <= "0000";
			else
				if rf_data(3) = '1' then
					led(0) <= sw0;
				else 
					led(0) <= '0';
				end if;
				if rf_data(7) = '1' then
					led(1) <= sw1;
				else 
					led(1) <= '0';
				end if;
				if rf_data(11) = '1' then
					led(2) <= sw2;
				else 
					led(2) <= '0';
				end if;
				if rf_data(15) = '1' then
					led(3) <= sw3;
				else 
					led(3) <= '0';
				end if;
			end if;
		end if;
	end process;


end Behavioral;

