----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    23:45:33 05/06/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    rotary - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity rotary is
port(
	clk : in std_logic;
	rot_a : in std_logic;
	rot_b : in std_logic;
	
	up : out std_logic;
	down : out std_logic
);
end rotary;

architecture Behavioral of rotary is
	
	signal rotary_a_in : std_logic;
	signal rotary_b_in : std_logic;
	signal rotary_in : std_logic_vector(1 downto 0);
	signal rotary_q1 : std_logic;
	signal rotary_q2 : std_logic;
	signal delay_rotary_q1 : std_logic;
	signal rotary_event : std_logic;
	signal rotary_left : std_logic;
	signal qa0, qa1, qa2, qb0, qb1, qb2 : std_logic;

begin

	rotary_filter: process(clk)
	begin
		if clk'event and clk='1' then
			rotary_a_in <= rot_a;
			rotary_b_in <= rot_b;
			rotary_in <= rotary_b_in & rotary_a_in;
			case rotary_in is
				when "00" => 
					rotary_q1 <= '0';
					rotary_q2 <= rotary_q2;
				when "01" => 
					rotary_q1 <= rotary_q1;
					rotary_q2 <= '0';
				when "10" => 
					rotary_q1 <= rotary_q1;
					rotary_q2 <= '1';
				when "11" => 
					rotary_q1 <= '1';
					rotary_q2 <= rotary_q2;
				when others => 
					rotary_q1 <= rotary_q1;
					rotary_q2 <= rotary_q2;
			end case;
		end if;
	end process;
	
	direction: process(clk)
	begin
		if clk'event and clk='1' then
			delay_rotary_q1 <= rotary_q1;
			if rotary_q1='1' and delay_rotary_q1='0' then
				rotary_event <= '1';
				rotary_left <= rotary_q2;
			else
				rotary_event <= '0';
				rotary_left <= rotary_left;
			end if;
		end if;
	end process;
	
	output: process(clk)
	begin
		if clk'event and clk='1' then
			if rotary_event = '1' and rotary_left = '1' then 
				up <= '0';
				down <= '1';
			elsif rotary_event = '1' and rotary_left = '0' then 
				up <= '1';
				down <= '0';
			else 
				up <= '0';
				down <= '0';
			end if;
		end if;
	end process;
	

end Behavioral;

