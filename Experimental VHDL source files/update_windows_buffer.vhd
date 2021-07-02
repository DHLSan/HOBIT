----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    09:55:15 05/06/2021 
-- Design Name: 
-- Module Name:    update_window_buffer - Arch 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity update_window_buffer is
    Port ( msb_window_buffer_in_1 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_in_2 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_in_4 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_in_5 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_in_7 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_in_8 : in  STD_LOGIC_VECTOR (63 downto 0);
		   msb_line_buffer_in_0 : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_line_buffer_in_1 : in  STD_LOGIC_VECTOR (63 downto 0);
		   msb_inputs : in  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_0 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_1 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_2 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_3 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_4 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_5 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_6 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_7 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_window_buffer_out_8 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_line_buffer_out_0 : out  STD_LOGIC_VECTOR (63 downto 0);
           msb_line_buffer_out_1 : out  STD_LOGIC_VECTOR (63 downto 0));
end update_window_buffer;

architecture Arch of update_window_buffer is

begin

msb_window_buffer_out_0 <= msb_window_buffer_in_1;
msb_window_buffer_out_1 <= msb_window_buffer_in_2;
msb_window_buffer_out_2 <= msb_line_buffer_in_0;
msb_window_buffer_out_3 <= msb_window_buffer_in_4;
msb_window_buffer_out_4 <= msb_window_buffer_in_5;
msb_window_buffer_out_5 <= msb_line_buffer_in_1;
msb_line_buffer_out_0 <= msb_line_buffer_in_1;
msb_window_buffer_out_6 <= msb_window_buffer_in_7;
msb_window_buffer_out_7 <= msb_window_buffer_in_8;
msb_window_buffer_out_8 <=  msb_inputs;
msb_line_buffer_out_1 <= msb_inputs;

end Arch;

