----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    14:34:10 05/08/2015  
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    led_controller - Behavioral  
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity led_controller is
generic(
	rf_addr_w : integer := 5;
	reg_width : integer := 32
);
port(
	clk : in std_logic;
	reset : in std_logic;
	rf_data : in std_logic_vector(reg_width - 1 downto 0);
	
	rf_addr : out std_logic_vector(rf_addr_w - 1 downto 0);
	led : out std_logic_vector(3 downto 0) 
);
end led_controller;

architecture Behavioral of led_controller is

	type t_state is (read1, read2, read3, idle);
	signal state, state_next : t_state;
	signal delay, delay_next : integer range 0 to 2147483647;
	signal addr, addr_next : std_logic_vector(rf_addr_w - 1 downto 0);
	signal sum, sum_next : std_logic_vector(reg_width - 1 downto 0);

begin
	
	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= idle;
				delay <= 0;
				addr <= (others => '0');
				sum <= (others => '0');
			else
				state <= state_next;
				delay <= delay_next;
				addr <= addr_next;
				sum <= sum_next;
			end if;
		end if;
	end process;
	
	--reg 5 & 17 or 0
	
	routing : process (state, delay, rf_data, addr, sum)
	begin
		state_next <= state;
		delay_next <= delay;
		addr_next <= addr;
		sum_next <= sum;
		
		case state is
			when read1 =>
				sum_next <= rf_data;
				addr_next <= "10001";
				state_next <= read2;
			when read2 =>
				sum_next <= sum and rf_data;
				addr_next <= "00000";
				state_next <= read3;
			when read3 =>
				sum_next <= sum or rf_data;
				state_next <= idle;
			when idle =>
				if delay = 100 then
					delay_next <= 0;
					state_next <= read1;
					addr_next <= "00101";
				else delay_next <= delay + 1;
				end if;
		end case;
	end process;

	led <= sum(3 downto 0);
	rf_addr <= addr;


end Behavioral;

