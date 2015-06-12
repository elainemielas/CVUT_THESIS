----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    15:19:51 03/20/2015
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    uart_rx - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity uart_rx is
generic(
	dbits : integer := 8;
	sb_ticks : integer := 16 -- 16 for 1 stop bit, 24 for 1.5, 32 for 2 
);
port(
	clk : in std_logic;
	reset : in std_logic;
	b_edge : in std_logic;
	rx: in std_logic;
	rx_data : out std_logic_vector((dbits-1) downto 0);
	rx_done : out std_logic
);
end uart_rx;

architecture Behavioral of uart_rx is
	type t_state is (idle, start, data, stop);
	signal state, state_next : t_state;
	signal s, s_next : unsigned(3 downto 0); -- number of sampling ticks; 7 for start; 15 for data
	signal n, n_next : unsigned(2 downto 0); -- number of bits received
	signal d, d_next : std_logic_vector((dbits-1) downto 0); -- data received

begin

	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= idle;
				s <= (others => '0');
				n <= (others => '0');
				d <= (others => '0');
			else
				state <= state_next;
				s <= s_next;
				n <= n_next;
				d <= d_next;
			end if;
		end if;
	end process;
	
	routing : process (state, s, n, d, rx, b_edge)
	begin
		state_next <= state;
		s_next <= s;
		n_next <= n;
		d_next <= d;
		rx_done <= '0';
		
		case state is
			when idle => 
				if rx = '0' then 
					state_next <= start;
					s_next <= (others => '0');
				end if;
			when start =>
				if b_edge = '1' then
					if s = 7 then
						state_next <= data;
						s_next <= (others => '0');
						n_next <= (others => '0');
					else 
						s_next <= s + 1;
					end if;
				end if;
			when data =>
				if b_edge = '1' then
					if s = 15 then
						s_next <= (others => '0');
						d_next <= rx & d((dbits-1) downto 1);
						if n = (dbits - 1) then
							state_next <= stop;
						else 
							n_next <= n + 1;
						end if;
					else 
						s_next <= s + 1;
					end if;
				end if;
			when stop =>
				if b_edge = '1' then
					if s = sb_ticks - 1 then
						state_next <= idle;
						rx_done <= '1';
					else
						s_next <= s + 1;
					end if;
				end if;
		end case;
	end process;
	
	rx_data <= d;			

end Behavioral;

