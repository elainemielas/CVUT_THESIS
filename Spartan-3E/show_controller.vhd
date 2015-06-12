----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    01:28:25 03/22/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    show_controller - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity show_controller is
generic(
	addr_width : integer := 5; -- log2(number of regs)
	reg_width : integer := 32 -- width of a reg
);
port(
	clk : in std_logic;
	reset : in std_logic;
	up : in std_logic;
	down : in std_logic;
	rf_data : in std_logic_vector(reg_width - 1 downto 0);
	--get_data : in std_logic;
	
	fifo_wr : out std_logic;
	rf_addr : out std_logic_vector(addr_width - 1 downto 0);
	show_dout : out std_logic_vector(7 downto 0);
	data_count : out std_logic_vector(5 downto 0);
	show_data : out std_logic
);
end show_controller;

architecture Behavioral of show_controller is

	type t_state is (init, idle, take_data, send_data);
	signal state, state_next : t_state;
	signal reg_cnt, reg_cnt_next : integer range 0 to 31;
	signal data_cnt, data_cnt_next : integer range 0 to 10;
	signal delay, delay_next : integer range 0 to 200000000;
	signal dout, dout_next : std_logic_vector(7 downto 0);
	signal addr, addr_next : std_logic_vector(4 downto 0);
	signal show, show_next : std_logic;
	signal wr_en, wr_en_next : std_logic;

begin

	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= init;
				reg_cnt <= 0;
				delay <= 1;
				data_cnt <= 0;
				dout <= (others => '0');
				addr <= (others => '0');
				show <= '0';
				wr_en <= '0';
			else
				state <= state_next;
				reg_cnt <= reg_cnt_next;
				delay <= delay_next;
				data_cnt <= data_cnt_next;
				dout <= dout_next;
				addr <= addr_next;
				show <= show_next;
				wr_en <= wr_en_next;
			end if;
		end if;
	end process;

	routing : process (state, delay, reg_cnt, data_cnt, dout, rf_data, addr, show, up, down, wr_en)
	begin
		state_next <= state;
		reg_cnt_next <= reg_cnt;
		delay_next <= delay;
		data_cnt_next <= data_cnt;
		dout_next <= dout;
		addr_next <= addr;
		show_next <= show;
		wr_en_next <= wr_en;
		
		case state is
			when init =>
				if delay = 200000000 then
					state_next <= take_data;
					delay_next <= 0;
				else
					delay_next <= delay + 1;
				end if;
			when take_data =>
				if delay = 150000 then
					delay_next <= 0;
					state_next <= send_data;
					--show_next <= '1';
				elsif delay = 0 then
					addr_next <= std_logic_vector(to_unsigned(reg_cnt, addr_width));
					delay_next <= delay + 1;
				else
					delay_next <= delay + 1;
				end if;
			when send_data =>
				--show_next <= '0';
				case data_cnt is
					when 0 =>
						dout_next <= "01010010";
						wr_en_next <= '1';
						data_cnt_next <= 1;
					when 1 =>
						dout_next <= "01000101";
						wr_en_next <= '1';
						data_cnt_next <= 2;
					when 2 =>
						dout_next <= "01000111";
						wr_en_next <= '1';
						data_cnt_next <= 3;
					when 3 =>
						case reg_cnt is
							when 0|1|2|3|4|5|6|7|8|9 =>
								dout_next <= "00110000";
							when 10|11|12|13|14|15|16|17|18|19 =>
								dout_next <= "00110001";
							when 20|21|22|23|24|25|26|27|28|29 =>
								dout_next <= "00110010";
							when 30|31 =>
								dout_next <= "00110011";
						end case;
						wr_en_next <= '1';
						data_cnt_next <= 4;
					when 4 =>
						case reg_cnt is
							when 0|10|20|30 =>
								dout_next <= "00110000";
							when 1|11|21|31 =>
								dout_next <= "00110001";
							when 2|12|22 =>
								dout_next <= "00110010";
							when 3|13|23 =>
								dout_next <= "00110011";
							when 4|14|24 =>
								dout_next <= "00110100";
							when 5|15|25 =>
								dout_next <= "00110101";
							when 6|16|26 =>
								dout_next <= "00110110";
							when 7|17|27 =>
								dout_next <= "00110111";
							when 8|18|28 =>
								dout_next <= "00111000";
							when 9|19|29 =>
								dout_next <= "00111001";
						end case;
						wr_en_next <= '1';
						data_cnt_next <= 5;
					when 5 =>
						dout_next <= "00111010";
						wr_en_next <= '1';
						data_cnt_next <= 6;
					when 6 =>
						--if get_data = '1' then
							case rf_data(15 downto 12) is
								when "0000" => dout_next <= "00110000"; -- 30
								when "0001" => dout_next <= "00110001";
								when "0010" => dout_next <= "00110010";
								when "0011" => dout_next <= "00110011";
								when "0100" => dout_next <= "00110100";
								when "0101" => dout_next <= "00110101";
								when "0110" => dout_next <= "00110110";
								when "0111" => dout_next <= "00110111";
								when "1000" => dout_next <= "00111000";
								when "1001" => dout_next <= "00111001"; -- 39
								when "1010" => dout_next <= "01000001"; -- 41
								when "1011" => dout_next <= "01000010";
								when "1100" => dout_next <= "01000011";
								when "1101" => dout_next <= "01000100";
								when "1110" => dout_next <= "01000101";
								when "1111" => dout_next <= "01000110"; -- 46
								when others => dout_next <= "01011111"; -- _
							end case;
							wr_en_next <= '1';
							data_cnt_next <= 7;
						--end if;
					when 7 =>
						--if get_data = '1' then
							case rf_data(11 downto 8) is
								when "0000" => dout_next <= "00110000"; -- 30
								when "0001" => dout_next <= "00110001";
								when "0010" => dout_next <= "00110010";
								when "0011" => dout_next <= "00110011";
								when "0100" => dout_next <= "00110100";
								when "0101" => dout_next <= "00110101";
								when "0110" => dout_next <= "00110110";
								when "0111" => dout_next <= "00110111";
								when "1000" => dout_next <= "00111000";
								when "1001" => dout_next <= "00111001"; -- 39
								when "1010" => dout_next <= "01000001"; -- 41
								when "1011" => dout_next <= "01000010";
								when "1100" => dout_next <= "01000011";
								when "1101" => dout_next <= "01000100";
								when "1110" => dout_next <= "01000101";
								when "1111" => dout_next <= "01000110"; -- 46
								when others => dout_next <= "01011111"; -- _
							end case;
							data_cnt_next <= 8;
							wr_en_next <= '1';
						--end if;
					when 8 =>
						--if get_data = '1' then
							case rf_data(7 downto 4) is
								when "0000" => dout_next <= "00110000"; -- 30
								when "0001" => dout_next <= "00110001";
								when "0010" => dout_next <= "00110010";
								when "0011" => dout_next <= "00110011";
								when "0100" => dout_next <= "00110100";
								when "0101" => dout_next <= "00110101";
								when "0110" => dout_next <= "00110110";
								when "0111" => dout_next <= "00110111";
								when "1000" => dout_next <= "00111000";
								when "1001" => dout_next <= "00111001"; -- 39
								when "1010" => dout_next <= "01000001"; -- 41
								when "1011" => dout_next <= "01000010";
								when "1100" => dout_next <= "01000011";
								when "1101" => dout_next <= "01000100";
								when "1110" => dout_next <= "01000101";
								when "1111" => dout_next <= "01000110"; -- 46
								when others => dout_next <= "01011111"; -- _
							end case;
							wr_en_next <= '1';
							data_cnt_next <= 9;
						--end if;
					when 9 =>
						--if get_data = '1' then
							case rf_data(3 downto 0) is
								when "0000" => dout_next <= "00110000"; -- 30
								when "0001" => dout_next <= "00110001";
								when "0010" => dout_next <= "00110010";
								when "0011" => dout_next <= "00110011";
								when "0100" => dout_next <= "00110100";
								when "0101" => dout_next <= "00110101";
								when "0110" => dout_next <= "00110110";
								when "0111" => dout_next <= "00110111";
								when "1000" => dout_next <= "00111000";
								when "1001" => dout_next <= "00111001"; -- 39
								when "1010" => dout_next <= "01000001"; -- 41
								when "1011" => dout_next <= "01000010";
								when "1100" => dout_next <= "01000011";
								when "1101" => dout_next <= "01000100";
								when "1110" => dout_next <= "01000101";
								when "1111" => dout_next <= "01000110"; -- 46
								when others => dout_next <= "01011111"; -- _
							end case;
							wr_en_next <= '1';
							data_cnt_next <= 10;
						when 10 =>
							--wr_en_next <= '1';
							data_cnt_next <= 0;
							state_next <= idle;
							show_next <= '1';
						--end if;
				end case;
			when idle =>
				show_next <= '0';
				wr_en_next <= '0';
				if up = '1' then
					state_next <= take_data;
					if reg_cnt = 31 then
						reg_cnt_next <= 0;
					else
						reg_cnt_next <= reg_cnt + 1;
					end if;
				elsif down = '1' then
					state_next <= take_data;
					if reg_cnt = 0 then
						reg_cnt_next <= 31;
					else
						reg_cnt_next <= reg_cnt - 1;
					end if;
				end if;
		end case;
	end process;
	
	show_data <= show;
	show_dout <= dout;
	rf_addr <= addr;
	data_count <= "001010";
	fifo_wr <= wr_en;

end Behavioral;
