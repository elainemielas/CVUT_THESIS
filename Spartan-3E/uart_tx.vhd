----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    19:54:39 03/20/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    uart_tx - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity uart_tx is
generic(
	dbits : integer := 8;
	sb_ticks : integer := 16 -- 16 for 1 stop bit, 24 for 1.5, 32 for 2 
);
port(
	clk : in std_logic;
	reset : in std_logic;
	b_edge : in std_logic;
	tx_start : in std_logic;
	tx_data : in std_logic_vector((dbits-1) downto 0);
	tx_done : out std_logic;
	tx : out std_logic
);
end uart_tx;

architecture Behavioral of uart_tx is
	type t_state is (idle, start, data, stop);
	signal state, state_next : t_state;
	signal s, s_next : unsigned(3 downto 0); -- number of sampling ticks; 16
	signal n, n_next : unsigned(2 downto 0); -- number of bits sent
	signal d, d_next : std_logic_vector((dbits-1) downto 0); -- data sent
	signal tx_r, tx_next : std_logic;
	
begin
	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= idle;
				s <= (others => '0');
				n <= (others => '0');
				d <= (others => '0');
				tx_r <= '1';
			else
				state <= state_next;
				s <= s_next;
				n <= n_next;
				d <= d_next;
				tx_r <= tx_next;
			end if;
		end if;
	end process;
	
	routing : process (state, s, n, d, b_edge, tx_r, tx_start, tx_data)
	begin
		state_next <= state;
		s_next <= s;
		n_next <= n;
		d_next <= d;
		tx_next <= tx_r;
		tx_done <= '0';
		
		case state is
			when idle =>
				tx_next <= '1';
				if tx_start = '1' then 
					state_next <= start;
					s_next <= (others => '0');
					d_next <= tx_data;
				end if;
			when start =>
				tx_next <= '0';
				if b_edge = '1' then
					if s = 15 then
						state_next <= data;
						s_next <= (others => '0');
						n_next <= (others => '0');
					else 
						s_next <= s + 1;
					end if;
				end if;
			when data =>
				tx_next <= d(0);
				if b_edge = '1' then
					if s = 15 then
						s_next <= (others => '0');
						d_next <= '0' & d((dbits-1) downto 1);
						if n = (dbits - 1) then
							state_next <= stop;
						else 
							n_next <= n + 1;
						end if;
					else 
						s_next <= s+1;
					end if;
				end if;
			when stop =>
				tx_next <= '1';
				if b_edge = '1' then
					if s = sb_ticks - 1 then
						state_next <= idle;
						tx_done <= '1';
					else
						s_next <= s + 1;
					end if;
				end if;
		end case;
	end process;
	
	tx <= tx_r;			
	
end Behavioral;

