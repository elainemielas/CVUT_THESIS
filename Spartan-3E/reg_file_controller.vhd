----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    01:28:25 03/22/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    reg_file_controller - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.MATH_REAL.ALL;

entity reg_file_controller is
generic(
	reg_num : integer := 32;
	reg_width : integer := 32;
	rf_addr_w : integer := 5;
	data_bits : integer := 8
);
port(
	clk : in std_logic;
	reset : in std_logic;
	rx_din : in std_logic_vector(data_bits - 1 downto 0);
	renew : in std_logic;
	
	--led : out std_logic_vector(7 downto 0);
	
	rx_req : out std_logic;
	tx_req : out std_logic;
	rf_req : out std_logic;
	tx_dout : out std_logic_vector(data_bits - 1 downto 0);
	rf_dout : out std_logic_vector(reg_width - 1 downto 0);
	rf_addr : out std_logic_vector(rf_addr_w - 1 downto 0)
);
end reg_file_controller;

architecture Behavioral of reg_file_controller is

	constant iter : integer := integer(round(real(reg_width/4))) + 1;

	type t_state is (idle, idle2, send_req, last, last2, parse, ignore);
	signal state, state_next : t_state;
	signal req_cnt, req_cnt_next : integer range 0 to 4;
	signal reg_cnt, reg_cnt_next : integer range 0 to 31;
	signal parse_mode, parse_mode_next : integer range 0 to 8; --iter;
	signal tx_data, tx_data_next : std_logic_vector(data_bits - 1 downto 0);
	signal tx_en, tx_en_next : std_logic;
	signal rx_en, rx_en_next : std_logic;
	signal reg_data, reg_data_next : std_logic_vector(15 downto 0);
	signal din: std_logic_vector(7 downto 0);
	signal rf_en, rf_en_next : std_logic;
	signal addr, addr_next : std_logic_vector(rf_addr_w - 1 downto 0);
	
	signal ren_sig : std_logic;
	signal q0, q1, q2 : std_logic;
	signal delay, delay_next : integer;
	signal ig_first, ig_first_next : std_logic;
	

begin

	registers : process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				state <= idle;
				req_cnt <= 0;
				reg_cnt <= 0;
				parse_mode <= 0;
				tx_data <=(others => '0');
				tx_en <= '0';
				rx_en <= '0';
				reg_data <=(others => '0');
				delay <= 0;
				rf_en <= '0';
				addr <=(others => '0');
				ig_first <= '0';
			else
				state <= state_next;
				req_cnt <= req_cnt_next;
				reg_cnt <= reg_cnt_next;
				parse_mode <= parse_mode_next;
				tx_data <= tx_data_next;
				tx_en <= tx_en_next;
				rx_en <= rx_en_next;
				reg_data <= reg_data_next;
				delay <= delay_next;
				rf_en <= rf_en_next;
				addr <= addr_next;
				ig_first <= ig_first_next;
			end if;
		end if; 
	end process;
	
	routing : process (state, req_cnt, tx_data, tx_en, delay, reg_cnt, parse_mode, rx_en, reg_data, din, rf_en, addr, ig_first, ren_sig)
	begin
		state_next <= state;
		req_cnt_next <= req_cnt;
		reg_cnt_next <= reg_cnt;
		parse_mode_next <= parse_mode;
		tx_data_next <= tx_data;
		tx_en_next <= tx_en;
		rx_en_next <= rx_en;
		reg_data_next <= reg_data;
		delay_next <= delay;
		rf_en_next <= rf_en;
		addr_next <= addr;
		ig_first_next <= ig_first;
		
		case state is
			when idle =>
				if delay = 100000 then
					state_next <= send_req;
					delay_next <= 0;
					tx_en_next <= '0';
				else 
					delay_next <= delay + 1;
					tx_en_next <= '0';
				end if;
			when send_req =>
				case req_cnt is
					when 0 =>
						tx_data_next <= "01000000"; -- 40 = @
						req_cnt_next <= 1;
						tx_en_next <= '1';
					when 1 =>
						tx_data_next <= "01010010"; -- 52 = R
						req_cnt_next <= 2;
						tx_en_next <= '1';
					when 2 =>
						case reg_cnt is
							when 0 => 
								tx_data_next <= "00110000"; 
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 1 => 
								tx_data_next <= "00110001";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 2 => 
								tx_data_next <= "00110010"; 
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 3 => 
								tx_data_next <= "00110011";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 4 => 
								tx_data_next <= "00110100";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 5 => 
								tx_data_next <= "00110101";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 6 => 
								tx_data_next <= "00110110";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 7 => 
								tx_data_next <= "00110111";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 8 => 
								tx_data_next <= "00111000";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 9 => 
								tx_data_next <= "00111001";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 10 =>
								tx_data_next <= "01000001"; -- A
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 11 =>
								tx_data_next <= "01000010"; -- B
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 12 =>
								tx_data_next <= "01000011"; -- C
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 13 =>
								tx_data_next <= "01000100"; -- D
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 14 =>
								tx_data_next <= "01000101"; -- E
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 15 =>
								tx_data_next <= "01000110"; -- F
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31 =>
								tx_data_next <= "00110001";
								req_cnt_next <= 3;
								tx_en_next <= '1';
						end case;
					when 3 =>
						case reg_cnt is
							when 16 => 
								tx_data_next <= "00110000"; 
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 17 => 
								tx_data_next <= "00110001";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 18 => 
								tx_data_next <= "00110010"; 
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 19 => 
								tx_data_next <= "00110011";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 20 => 
								tx_data_next <= "00110100";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 21 => 
								tx_data_next <= "00110101";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 22 => 
								tx_data_next <= "00110110";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 23 => 
								tx_data_next <= "00110111";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 24 => 
								tx_data_next <= "00111000";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 25 => 
								tx_data_next <= "00111001";
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 26 =>
								tx_data_next <= "01000001"; -- A
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 27 =>
								tx_data_next <= "01000010"; -- B
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 28 =>
								tx_data_next <= "01000011"; -- C
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 29 =>
								tx_data_next <= "01000100"; -- D
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 30 =>
								tx_data_next <= "01000101"; -- E
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when 31 =>
								tx_data_next <= "01000110"; -- F
								req_cnt_next <= 4;
								tx_en_next <= '1';
							when others => NULL;
						end case;
					when 4 =>
						tx_data_next <= "00001010"; -- 10 = LF (\n)
						req_cnt_next <= 0;
						tx_en_next <= '1';
						if reg_cnt = 31 then
							state_next <= last;
							reg_cnt_next <= 0;
						else 
							state_next <= send_req;
							reg_cnt_next <= reg_cnt + 1;
						end if;
				end case;
			when last =>
				state_next <= last2;
				tx_data_next <= "00000000";
				tx_en_next <= '1';
			when last2 =>
				if delay = 150000000 then
					if ig_first = '1' then
						state_next <= ignore;
					else 
						state_next <= parse;
					end if;
					reg_cnt_next <= 0;
					tx_en_next <= '0';
					delay_next <= 0;
				else delay_next <= delay + 1;
			end if;
			when ignore =>
				if delay = 4 then
					delay_next <= 0;
					rx_en_next <= '0';
					state_next <= parse;
				else
					delay_next <= delay + 1;
					rx_en_next <= '1';
				end if;
			when parse =>
				case parse_mode is
					when 0 =>
						if delay = 1 then
							delay_next <= 0;
							rx_en_next <= '0';
							parse_mode_next <= 1;
							addr_next <= std_logic_vector(to_unsigned(reg_cnt, rf_addr_w));
						else
							rx_en_next <= '1';
							delay_next <= delay + 1;
						end if;
					when 1 =>
						case din is
							when "00110000"|"00110001"|"00110010"|"00110011"|"00110100"|"00110101"|"00110110"|"00110111"|"00111000"|"00111001" =>
								reg_data_next(15 downto 12) <= din(3 downto 0);
								parse_mode_next <= 2;
							when "01000001"|"01000010"|"01000011"|"01000100"|"01000101"|"01000110" =>
								reg_data_next(15 downto 12) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 2;
							when "01100001"|"01100010"|"01100011"|"01100100"|"01100101"|"01100110" =>
								reg_data_next(15 downto 12) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 2;
							when others => parse_mode_next <= 0;
						end case;
					when 2 =>
						if delay = 1 then
							delay_next <= 0;
							rx_en_next <= '0';
							parse_mode_next <= 3;
						else
							rx_en_next <= '1';
							delay_next <= delay + 1;
						end if;
					when 3 =>
						case din is
							when "00110000"|"00110001"|"00110010"|"00110011"|"00110100"|"00110101"|"00110110"|"00110111"|"00111000"|"00111001" =>
								reg_data_next(11 downto 8) <= din(3 downto 0);
								parse_mode_next <= 4;
							when "01000001"|"01000010"|"01000011"|"01000100"|"01000101"|"01000110" =>
								reg_data_next(11 downto 8) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 4;
							when "01100001"|"01100010"|"01100011"|"01100100"|"01100101"|"01100110" =>
								reg_data_next(11 downto 8) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 4;
							when others => parse_mode_next <= 2;
						end case;
					when 4 =>
						if delay = 1 then
							delay_next <= 0;
							rx_en_next <= '0';
							parse_mode_next <= 5;
						else
							rx_en_next <= '1';
							delay_next <= delay + 1;
						end if;
					when 5 =>
						case din is
							when "00110000"|"00110001"|"00110010"|"00110011"|"00110100"|"00110101"|"00110110"|"00110111"|"00111000"|"00111001" =>
								reg_data_next(7 downto 4) <= din(3 downto 0);
								parse_mode_next <= 6;
							when "01000001"|"01000010"|"01000011"|"01000100"|"01000101"|"01000110" =>
								reg_data_next(7 downto 4) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 6;
							when "01100001"|"01100010"|"01100011"|"01100100"|"01100101"|"01100110" =>
								reg_data_next(7 downto 4) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 6;
							when others => parse_mode_next <= 4;
						end case;
					when 6 =>
						if delay = 1 then
							delay_next <= 0;
							rx_en_next <= '0';
							parse_mode_next <= 7;
						else
							rx_en_next <= '1';
							delay_next <= delay + 1;
						end if;
					when 7 =>
						case din is
							when "00110000"|"00110001"|"00110010"|"00110011"|"00110100"|"00110101"|"00110110"|"00110111"|"00111000"|"00111001" =>
								reg_data_next(3 downto 0) <= din(3 downto 0);
								parse_mode_next <= 8;
							when "01000001"|"01000010"|"01000011"|"01000100"|"01000101"|"01000110" =>
								reg_data_next(3 downto 0) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 8;
							when "01100001"|"01100010"|"01100011"|"01100100"|"01100101"|"01100110" =>
								reg_data_next(3 downto 0) <= std_logic_vector(unsigned(din(3 downto 0)) + 9);
								parse_mode_next <= 8;
							when others => parse_mode_next <= 6;
						end case;
					when 8 =>
						if delay = 2 then
							rx_en_next <= '0';
							parse_mode_next <= 0;
							if reg_cnt = 31 then
								state_next <= idle2;
								delay_next <= integer'low;
								reg_cnt_next <= 0;
							else
								state_next <= parse;
								delay_next <= 0;
								reg_cnt_next <= reg_cnt + 1;
							end if;
						else
							rx_en_next <= '1';
							delay_next <= delay + 1;
						end if;
				end case; 
			when idle2 =>
				if delay = integer'high then
					state_next <= idle;
					delay_next <= 0;
				else
					if ren_sig = '1' then
						state_next <= idle;
						delay_next <= 0;
					else
						delay_next <= delay + 1;
					end if;
				end if;
				tx_en_next <= '0';
				rf_en_next <= '0';
				ig_first_next <= '1';
		end case;
	end process;
	
	renew_deb: process(clk)
	begin
		if clk'event and clk='1' then
			if reset = '1' then
				q0 <= '0';
				q1 <= '0';
				q2 <= '0';
			else
				q0 <= renew;
				q1 <= q0;
				q2 <= q1;
			end if;
		end if;
	end process;
	
	ren_sig <= q0 and q1 and (not q2);
	
	tx_dout <= tx_data;
	tx_req <= tx_en;
	rx_req <= rx_en;
	rf_req <= rf_en;
	rf_addr <= addr;
	rf_dout <= reg_data;
	
	din <= rx_din;

end Behavioral;