----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:10:31 05/06/2021 
-- Design Name: 
-- Module Name:    copy_output_features - Arch 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity copy_output_features is
    Port ( clk : in  STD_LOGIC;
           reset : in  STD_LOGIC;
           start : in  STD_LOGIC;
           msb_outputs : in  STD_LOGIC_VECTOR (15 downto 0);
           c_in : in  STD_LOGIC_VECTOR (31 downto 0);
           ready : out  STD_LOGIC;
           msb_partial_out_feature : out  STD_LOGIC_VECTOR (15 downto 0));
end copy_output_features;

architecture Arch of copy_output_features is

type state_type is (idle, loopy, done);
signal state_reg, state_next: state_type;
signal c_in_s : unsigned (31 downto 0);
signal OUT_CHANNEL_PARALLELISM : integer := 16;
begin

    c_in_s <= unsigned(c_in);

	process(clk,reset)
	begin
		if(reset = '1') then
			state_reg <= idle;
		elsif(rising_edge(clk)) then
			state_reg <= state_next;
		end if;
	end process;

	process(state_reg, msb_outputs, start)
	begin	
		case state_reg is	
			when idle =>
			    if(start = '1') then
				    state_next <= loopy;
				else 
				    state_next <= idle;
				end if;
				
			when loopy =>
				if(OUT_CHANNEL_PARALLELISM > 0) then
				    if(c_in_s > 0) then
				        msb_partial_out_feature <= msb_outputs;
				    else
				        msb_partial_out_feature <= (others => '0');
				    end if;
				    OUT_CHANNEL_PARALLELISM <= OUT_CHANNEL_PARALLELISM - 1;
                    state_next <= loopy;	
				else
					state_next <= done;
				end if;	
				
			when done =>
			    OUT_CHANNEL_PARALLELISM <= 16;
			    ready <= '1';
				state_next <= idle;			
		end case;
	end process;

	 						
end Arch;




























