----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    15:21:19 03/20/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    rate_generator - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity rate_generator is
generic(
	dvsr : integer := 2604
	-- divisor = 50000000 / (16 * baud)
		-- 1200 -> 2604 +
		-- 2400 -> 1302 +
		-- 4800 -> 651 +
		-- 9600 -> 326
		-- 19200 -> 163
		-- 38400 -> 81
		-- 57600 -> 54 +
		-- 115200 -> 27 +
		-- 230400 -> 14
		-- 460800 -> 7
		-- 500000 -> 6
		-- 576000 -> 5
		-- 921600 -> 3
		-- 1000000 -> 3 +
		-- 1152000 -> 3
		-- 1500000 -> 2 +
		-- 2000000 -> 2
		-- 2500000 -> 1
		-- 3000000 -> 1 +
);
port(
	clk : in std_logic;
	reset : in std_logic;
	b_edge : out std_logic
);
end rate_generator;

architecture Behavioral of rate_generator is
	signal b_count : integer range 1 to dvsr := 1; 
begin
	b_clock : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				b_count <= 1;
				b_edge <= '0';
			elsif b_count = dvsr then
				b_count <= 1;
				b_edge <= '1';
			else 
				b_count <= b_count + 1;
				b_edge <= '0';
			end if;
		end if;
	end process;

end Behavioral;

