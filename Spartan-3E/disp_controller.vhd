----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    01:28:25 03/22/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    disp_controller - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity disp_controller is
port(
	clk : in std_logic;
	reset : in std_logic;
	din : in std_logic_vector(7 downto 0);
	data_count : in std_logic_vector(5 downto 0);
	show_data : in std_logic;
	
	get_data_o : out std_logic;
	dout : out std_logic_vector(3 downto 0);
	lcd_rs : out std_logic;
	lcd_rw : out std_logic;
	lcd_e : out std_logic
);
end disp_controller;

architecture Behavioral of disp_controller is

	type t_state is (power_on1, power_on2, power_on3, power_on4, send1, send2, send3,
						  func_set, entry_mode, disp_on, disp_clear, clear_wait, addr_set, disp_data, idle);
	signal state, state_next : t_state;
	signal rs, rs_next : std_logic;
	signal en, en_next : std_logic;
	signal data, data_next : std_logic_vector(3 downto 0);
	signal nibble, nibble_next : std_logic; -- 0 if high, 1 if low, high goes first
	signal delay, delay_next : integer range 0 to 800000; -- up to 750000 cycles
	signal send_mode, send_mode_next : integer range 0 to 10;
	signal addr_mode, addr_mode_next : integer range 1 to 2;
	signal data_cnt, data_cnt_next : unsigned(5 downto 0);
	signal target_data_cnt : unsigned(5 downto 0);
	signal get_data, get_data_next : std_logic;
	signal d_low, d_high, d_low_next, d_high_next : std_logic_vector(3 downto 0);

begin

	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= power_on1;
				rs <= '0';
				data <= "0000";
				delay <= 0;
				nibble <= '0';
				en <= '0';
				send_mode <= 0;
				addr_mode <= 1;
				data_cnt <= (others => '0');
				get_data <= '0';
				d_low <= (others => '0');
				d_high <= (others => '0');
			else
				state <= state_next;
				rs <= rs_next;
				data <= data_next;
				delay <= delay_next;
				nibble <= nibble_next;
				en <= en_next;
				send_mode <= send_mode_next;
				addr_mode <= addr_mode_next;
				data_cnt <= data_cnt_next;
				get_data <= get_data_next;
				d_low <= d_low_next;
				d_high <= d_high_next;
			end if;
		end if;
	end process;
	
	routing : process (state, rs, data, delay, nibble, show_data, en, send_mode, addr_mode, data_cnt, get_data, d_low, d_high, din, data_count, target_data_cnt)
	begin
		state_next <= state;
		rs_next <= rs;
		data_next <= data;
		delay_next <= delay;
		nibble_next <= nibble;
		en_next <= en;
		send_mode_next <= send_mode;
		addr_mode_next <= addr_mode;
		data_cnt_next <= data_cnt;
		get_data_next <= get_data;
		d_low_next <= d_low;
		d_high_next <= d_high;
		
		case state is
			
			when send1 =>
				if delay = 3 then
					en_next <= '1';
					state_next <= send2;
					delay_next <= 0;
				else delay_next <= delay + 1;
				end if;
				
			when send2 =>
				if delay = 12 then
					en_next <= '0';
					state_next <= send3;
					delay_next <= 0;
				else delay_next <= delay + 1;
				end if;

			when send3 =>
				if delay = 3 then
					case send_mode is
						when 0 => state_next <= power_on2;
						when 1 => state_next <= power_on3;
						when 2 => state_next <= power_on4;
						when 3 => state_next <= func_set;
						when 4 => state_next <= entry_mode;
						when 5 => state_next <= disp_on;
						when 6 => state_next <= disp_clear;
						when 7 => state_next <= addr_set;
						when 8 => state_next <= disp_data;
						when 9 => state_next <= idle;
						when 10 => state_next <= clear_wait;
					end case;
					delay_next <= 0;
				else delay_next <= delay + 1;
				end if;
			
			---INITIALIZATION---	
			when power_on1 =>
				if delay = 770000 then
					data_next <= "0011";
					rs_next <= '0';
					delay_next <= 0;
					state_next <= send1;
				else delay_next <= delay + 1;
				end if;
			when power_on2 =>
				if delay = 210000 then
					data_next <= "0011";
					rs_next <= '0';
					delay_next <= 0;
					state_next <= send1;
					send_mode_next <= 1;
				else delay_next <= delay + 1;
				end if;
			when power_on3 =>
				if delay = 7000 then
					data_next <= "0011";
					rs_next <= '0';
					delay_next <= 0;
					state_next <= send1;
					send_mode_next <= 2;
				else delay_next <= delay + 1;
				end if;
			when power_on4 =>
				if delay = 3000 then
					data_next <= "0010";
					rs_next <= '0';
					delay_next <= 0;
					state_next <= send1;
					send_mode_next <= 3;
				else delay_next <= delay + 1;
				end if;
			---CONFIGURATION---
			when func_set =>
				if nibble = '0' then
					if delay = 3000 then
						data_next <= "0010";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
					else delay_next <= delay + 1;
					end if;
				else
					if delay = 100 then
						data_next <= "1000";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						send_mode_next <= 4;
						nibble_next <= '0';
					else delay_next <= delay + 1;
					end if;
				end if;
			when entry_mode =>
				if nibble = '0' then
					if delay = 3000 then
						data_next <= "0000";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
					else delay_next <= delay + 1;
					end if;
				else
					if delay = 100 then
						data_next <= "0100";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						send_mode_next <= 5;
						nibble_next <= '0';
					else delay_next <= delay + 1;
					end if;
				end if;
			when disp_on =>
				if nibble = '0' then
					if delay = 3000 then
						data_next <= "0000";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
					else delay_next <= delay + 1;
					end if;
				else
					if delay = 100 then
						data_next <= "1110";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						send_mode_next <= 6;
						nibble_next <= '0';
					else delay_next <= delay + 1;
					end if;
				end if;
			when disp_clear =>
				if nibble = '0' then
					if delay = 3000 then
						data_next <= "0000";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
					else delay_next <= delay + 1;
					end if;
				else
					if delay = 100 then
						data_next <= "0001";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						send_mode_next <= 10;
						nibble_next <= '0';
					else delay_next <= delay + 1;
					end if;
				end if;
			when clear_wait => 
				if delay = 90000 then
					state_next <= idle;
					delay_next <= 0;
				else 
					delay_next <= delay + 1;
				end if;
			---WRITING DATA---
			when addr_set =>
				if nibble = '0' then
					if delay = 0 then
						case addr_mode is
							when 1 => data_next <= "1000";
							when 2 => data_next <= "1100";
						end case;
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
						get_data_next <= '0';
					else 
						delay_next <= delay + 1;
						get_data_next <= '0';
					end if;
				else
					if delay = 98 then
						delay_next <= delay + 1;
						if addr_mode = 1 then
							get_data_next <= '1';
						end if;
					elsif delay = 100 then
						data_next <= "0000";
						rs_next <= '0';
						state_next <= send1;
						delay_next <= 0;
						send_mode_next <= 8;
						nibble_next <= '0';
						get_data_next <= '0';
						if addr_mode = 1 then
							d_high_next <= din(7 downto 4);
							d_low_next <= din(3 downto 0);
						end if;
					else 
						delay_next <= delay + 1;
						get_data_next <= '0';
					end if;
				end if;
			when disp_data =>
				-- change address after 16 letters
				if nibble = '0' then
					if delay = 3000 then
						data_next <= d_high;
						rs_next <= '1';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '1';
						get_data_next <= '0';
					else 
						delay_next <= delay + 1;
						get_data_next <= '0';
					end if;
				else
					if delay = 98 then
						delay_next <= delay + 1;
						get_data_next <= '1';
					elsif delay = 100 then
						data_next <= d_low;
						rs_next <= '1';
						state_next <= send1;
						delay_next <= 0;
						nibble_next <= '0';
						get_data_next <= '0';
						d_high_next <= din(7 downto 4);
						d_low_next <= din(3 downto 0);
						if data_cnt = target_data_cnt - 1 then
							send_mode_next <= 9;
						elsif data_cnt = 15 then
							send_mode_next <= 7;
							addr_mode_next <= 2;
						else
							send_mode_next <= 8;
						end if;
						data_cnt_next <= data_cnt + 1;
					else 
						delay_next <= delay + 1;
						get_data_next <= '0';
					end if;
				end if;
			when idle =>
				if show_data = '1' then
					--state_next <= disp_clear;
					state_next <= addr_set;
					addr_mode_next <= 1;
					data_cnt_next <= "000000";
				end if;
		end case;
		
	end process;
	
	process(clk)
	begin
		if clk'event and clk='1' then
			if reset='1' then
				target_data_cnt <= "000000";
			elsif show_data = '1' then
				target_data_cnt <= unsigned(data_count);
			else
				target_data_cnt <= target_data_cnt;
			end if;
		end if;
	end process;

	lcd_rw <= '0';
	lcd_rs <= rs;
	lcd_e <= en;
	dout <= data;
	get_data_o <= get_data;
 
end Behavioral;

