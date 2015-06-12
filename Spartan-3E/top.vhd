----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    21:50:03 03/21/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    top - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.MATH_REAL.ALL;
use IEEE.std_logic_arith.all;

entity top is
generic(
	reg_num : integer := 32; -- number of registers in register field
	reg_width : integer := 16; -- register width in register field
	baud_rate : integer := 19200;
	data_bits : integer := 8; -- fifo is 8 bit, but a new one can be generated
	stop_bits : real := 1.0
);
port(
	clk : in std_logic;
	reset : in std_logic;
	rx : in std_logic;
	tx : out std_logic ;
	rot_a : in std_logic;
	rot_b : in std_logic;
	sw0 : in std_logic;
	sw1 : in std_logic;
	sw2 : in std_logic;
	sw3 : in std_logic;
	
	lcd_dout : out std_logic_vector(3 downto 0);
	lcd_rs : out std_logic;
	lcd_rw : out std_logic;
	lcd_e : out std_logic;
	renew_reg : in std_logic;
	led : out std_logic_vector(7 downto 0)
);
end top;

	architecture Behavioral of top is

component rate_generator is
generic(
	dvsr : integer := 2604
);
port(
	clk : in std_logic;
	reset : in std_logic;
	b_edge : out std_logic
);
end component;

component uart_rx is
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
end component;

component uart_tx is
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
end component;

COMPONENT fifo_rx
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    data_count : OUT STD_LOGIC_VECTOR(4 DOWNTO 0)
  );
END COMPONENT;

COMPONENT fifo_tx
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
END COMPONENT;

COMPONENT fifo_uart IS
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    data_count : OUT STD_LOGIC_VECTOR(4 DOWNTO 0)
  );
END COMPONENT;

COMPONENT fifo_uart_long IS
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
END COMPONENT;

COMPONENT fifo_uart_long3 IS
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
END COMPONENT;

COMPONENT fifo_uart_long_w_outp IS
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
	 data_count : OUT STD_LOGIC_VECTOR(9 DOWNTO 0)
  );
END COMPONENT;

component disp_controller
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
end component;

component reg_file_controller is
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
	
	rx_req : out std_logic;
	tx_req : out std_logic;
	rf_req : out std_logic;
	tx_dout : out std_logic_vector(data_bits - 1 downto 0);
	rf_dout : out std_logic_vector(reg_width - 1 downto 0);
	rf_addr : out std_logic_vector(rf_addr_w - 1 downto 0)
);
end component;

component register_file is
generic(
	addr_width : integer := 5; -- log2(number of regs)
	reg_width : integer := 32 -- width of a reg
);
port(
	clk : in std_logic;
	reset : in std_logic;
	wr_en : in std_logic;
	wr_addr : in std_logic_vector(addr_width - 1 downto 0);
	wr_data : in std_logic_vector(reg_width - 1 downto 0);
	rd_addr : in std_logic_vector(addr_width - 1 downto 0);
	rd_data : out std_logic_vector(reg_width - 1 downto 0);
	rd_addr_2 : in std_logic_vector(addr_width - 1 downto 0);
	rd_data_2 : out std_logic_vector(reg_width - 1 downto 0);
	rd_addr_3 : in std_logic_vector(addr_width - 1 downto 0);
	rd_data_3 : out std_logic_vector(reg_width - 1 downto 0)
);
end component;

component show_controller is
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
	
	fifo_wr : out std_logic;
	rf_addr : out std_logic_vector(addr_width - 1 downto 0);
	show_dout : out std_logic_vector(7 downto 0);
	data_count : out std_logic_vector(5 downto 0);
	show_data : out std_logic
);
end component;

component show_fifo_wide IS
  PORT (
    clk : IN STD_LOGIC;
    rst : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
END component;

component rotary is
port(
	clk : in std_logic;
	rot_a : in std_logic;
	rot_b : in std_logic;
	
	up : out std_logic;
	down : out std_logic
);
end component;

component switch_controller is
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
end component;

component led_controller is
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
end component;

attribute box_type : string; 
attribute box_type of fifo_uart_long : component is "black_box";
attribute box_type of fifo_uart_long3 : component is "black_box";
attribute box_type of show_fifo_wide : component is "black_box";

constant dvsr : integer := integer(round(real(50000000/(baud_rate*16))));
constant stop_bit_ticks : integer := integer(stop_bits*16.0);
constant rf_addr_w : integer := integer(ceil(log2(real(reg_num))));

signal b_edge : std_logic;
signal rx_data : std_logic_vector((data_bits-1) downto 0);
signal tx_data : std_logic_vector((data_bits-1) downto 0);
signal rx_done : std_logic;
signal tx_done : std_logic;
signal got_data : std_logic_vector(7 downto 0);
signal to_send_data : std_logic_vector(7 downto 0);
signal get_data : std_logic;
signal send_data : std_logic;
signal rx_full : std_logic;
signal tx_full : std_logic;
signal rx_empty : std_logic;
signal inv_in : std_logic;
signal inv_out : std_logic;
signal inv2_in : std_logic;
signal inv2_out : std_logic;
signal wr_rf : std_logic;
signal rf_wr_data : std_logic_vector(reg_width - 1 downto 0);
signal rf_wr_addr : std_logic_vector(rf_addr_w - 1 downto 0);
signal rf_rd_data : std_logic_vector(reg_width - 1 downto 0);
signal rf_rd_addr : std_logic_vector(rf_addr_w - 1 downto 0);
signal rf_rd_data2 : std_logic_vector(reg_width - 1 downto 0);
signal rf_rd_addr2 : std_logic_vector(rf_addr_w - 1 downto 0);
signal rf_rd_data3 : std_logic_vector(reg_width - 1 downto 0);
signal rf_rd_addr3 : std_logic_vector(rf_addr_w - 1 downto 0);
signal up : std_logic;
signal down : std_logic;
signal sh_ctrl_data : std_logic_vector(7 downto 0);
signal show_data : std_logic;
signal sh_get_data : std_logic;
signal data_count : std_logic_vector(5 downto 0);
signal show_wr_en : std_logic;
signal sh_ctrl_dout : std_logic_vector(7 downto 0);
signal show_full : std_logic;
signal show_empty : std_logic;

begin

	inv_out <= not inv_in;
	
	rate_gen_unit : rate_generator
		generic map(dvsr => dvsr)
		port map(clk => clk, reset => reset, b_edge => b_edge);
	
	uart_rx_unit : uart_rx
		generic map(dbits => data_bits, sb_ticks => stop_bit_ticks)
		port map(clk => clk, reset => reset, b_edge => b_edge, rx => rx, rx_data => rx_data, rx_done => rx_done);
		
	uart_tx_unit : uart_tx
		generic map(dbits => data_bits, sb_ticks => stop_bit_ticks)
		port map(clk => clk, reset => reset, b_edge => b_edge, tx_start => inv_out, tx_data => tx_data, tx_done => tx_done, tx => tx);
	
	uart_rx_fifo : fifo_uart_long
		port map(clk => clk, rst => reset, din => rx_data, wr_en => rx_done, rd_en => get_data, dout => got_data, 
		full => rx_full, empty => rx_empty);
		
	uart_tx_fifo : fifo_uart_long3
		port map(clk => clk, rst => reset, din => to_send_data, wr_en => send_data, rd_en => tx_done, dout => tx_data,
		full => tx_full, empty => inv_in);
		
	disp_ctrl : disp_controller
		port map(clk => clk, reset => reset, din => sh_ctrl_dout, data_count => data_count, show_data => show_data,
		get_data_o => sh_get_data, dout => lcd_dout, lcd_rs => lcd_rs, lcd_rw => lcd_rw, lcd_e => lcd_e);
		
	rf_ctrl : reg_file_controller
		generic map(reg_num => reg_num, reg_width => reg_width, rf_addr_w => rf_addr_w, data_bits => data_bits)
		port map(clk => clk, reset => reset, rx_din => got_data, rx_req => get_data, renew => renew_reg, 
		tx_req => send_data, rf_req => wr_rf, tx_dout => to_send_data, rf_dout => rf_wr_data, rf_addr => rf_wr_addr
		);
		
	regfile : register_file
		generic map(reg_width => reg_width, addr_width => rf_addr_w)
		port map(clk => clk, reset => reset, wr_en => wr_rf, wr_addr => rf_wr_addr, rd_addr => rf_rd_addr,  
		wr_data => rf_wr_data, rd_data => rf_rd_data, rd_addr_2 => rf_rd_addr2, rd_data_2 => rf_rd_data2,
		rd_addr_3 => rf_rd_addr3, rd_data_3 => rf_rd_data3);
	
	show_ctrl : show_controller
		generic map(reg_width => reg_width, addr_width => rf_addr_w)
		port map(clk => clk, reset => reset, up => up, down => down, rf_data => rf_rd_data,
		rf_addr => rf_rd_addr, show_dout => sh_ctrl_data, data_count => data_count, show_data => show_data, fifo_wr => show_wr_en);
		
	show_ctrl_fifo : show_fifo_wide
		port map(clk => clk, rst => reset, din => sh_ctrl_data, wr_en => show_wr_en, rd_en => sh_get_data, dout => sh_ctrl_dout,
		full => show_full, empty => show_empty);
		
	rot_ctrl : rotary
		port map(clk => clk, rot_a => rot_a, rot_b => rot_b, up => up, down => down);
		
	sw_ctrl : switch_controller
		generic map(reg_width => reg_width, rf_addr_w => rf_addr_w)
		port map(clk => clk, reset => reset, sw0 => sw0, sw1 => sw1, sw2 => sw2, sw3 => sw3,
		rf_data => rf_rd_data2, rf_addr => rf_rd_addr2, led => led(3 downto 0));
		
	l_ctrl : led_controller
		generic map(reg_width => reg_width, rf_addr_w => rf_addr_w)
		port map(clk => clk, reset => reset, rf_data => rf_rd_data3, rf_addr => rf_rd_addr3, led => led(7 downto 4));

end Behavioral;

