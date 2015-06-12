----------------------------------------------------------------------------------
-- Company: FIT CTU
-- Engineer: Elena Filipenkova
-- 
-- Create Date:    01:28:25 03/22/2015 
-- Design Name:    FPGA deska rizena procesorem
-- Module Name:    register_file - Behavioral 
-- Target Devices: Spartan-3E Starter Kit

-- Revision 0.01 - File Created
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity register_file is
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
end register_file;

architecture Behavioral of register_file is

	type reg_file_type is array (2**addr_width - 1 downto 0) of std_logic_vector (reg_width - 1 downto 0);
	signal reg_file, reg_file_2, reg_file_3 : reg_file_type;

begin

	process (clk)
	begin
		if clk = '1' and clk'event then
			if reset = '1' then
				reg_file <= (others => (others => '0'));
				reg_file_2 <= (others => (others => '0'));
				reg_file_3 <= (others => (others => '0'));
			else
				if wr_en = '1' then
					reg_file(to_integer(unsigned(wr_addr))) <= wr_data;
					reg_file_2(to_integer(unsigned(wr_addr))) <= wr_data;
					reg_file_3(to_integer(unsigned(wr_addr))) <= wr_data;
				end if;
			end if;
		end if;
	end process;
	
	rd_data <= reg_file(to_integer(unsigned(rd_addr)));
	rd_data_2 <= reg_file_2(to_integer(unsigned(rd_addr_2)));
	rd_data_3 <= reg_file_3(to_integer(unsigned(rd_addr_3)));


end Behavioral;

