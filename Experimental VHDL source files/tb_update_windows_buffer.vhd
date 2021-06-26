--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   10:18:44 05/06/2021
-- Design Name:   
-- Module Name:   D:/XilinxProjects/binary_conv3x3_tile/tb_update_windows_buffer.vhd
-- Project Name:  binary_conv3x3_tile
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: update_window_buffer
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY tb_update_windows_buffer IS
END tb_update_windows_buffer;
 
ARCHITECTURE behavior OF tb_update_windows_buffer IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT update_window_buffer
    PORT(
         msb_window_buffer_in_1 : IN  std_logic_vector(63 downto 0);
         msb_window_buffer_in_2 : IN  std_logic_vector(63 downto 0);
         msb_window_buffer_in_4 : IN  std_logic_vector(63 downto 0);
         msb_window_buffer_in_5 : IN  std_logic_vector(63 downto 0);
         msb_window_buffer_in_7 : IN  std_logic_vector(63 downto 0);
         msb_window_buffer_in_8 : IN  std_logic_vector(63 downto 0);
         msb_line_buffer_in_0 : IN  std_logic_vector(63 downto 0);
         msb_line_buffer_in_1 : IN  std_logic_vector(63 downto 0);
         msb_inputs : IN  std_logic_vector(15 downto 0);
         msb_window_buffer_out_0 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_1 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_2 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_3 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_4 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_5 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_6 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_7 : OUT  std_logic_vector(63 downto 0);
         msb_window_buffer_out_8 : OUT  std_logic_vector(63 downto 0);
         msb_line_buffer_out_0 : OUT  std_logic_vector(63 downto 0);
         msb_line_buffer_out_1 : OUT  std_logic_vector(63 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal msb_window_buffer_in_1 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_window_buffer_in_2 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_window_buffer_in_4 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_window_buffer_in_5 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_window_buffer_in_7 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_window_buffer_in_8 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_line_buffer_in_0 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_line_buffer_in_1 : std_logic_vector(63 downto 0) := (others => '0');
   signal msb_inputs : std_logic_vector(15 downto 0) := (others => '0');

 	--Outputs
   signal msb_window_buffer_out_0 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_1 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_2 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_3 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_4 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_5 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_6 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_7 : std_logic_vector(63 downto 0);
   signal msb_window_buffer_out_8 : std_logic_vector(63 downto 0);
   signal msb_line_buffer_out_0 : std_logic_vector(63 downto 0);
   signal msb_line_buffer_out_1 : std_logic_vector(63 downto 0);
   -- No clocks detected in port list. Replace <clock> below with 
   -- appropriate port name 

 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: update_window_buffer PORT MAP (
          msb_window_buffer_in_1 => msb_window_buffer_in_1,
          msb_window_buffer_in_2 => msb_window_buffer_in_2,
          msb_window_buffer_in_4 => msb_window_buffer_in_4,
          msb_window_buffer_in_5 => msb_window_buffer_in_5,
          msb_window_buffer_in_7 => msb_window_buffer_in_7,
          msb_window_buffer_in_8 => msb_window_buffer_in_8,
          msb_line_buffer_in_0 => msb_line_buffer_in_0,
          msb_line_buffer_in_1 => msb_line_buffer_in_1,
          msb_inputs => msb_inputs,
          msb_window_buffer_out_0 => msb_window_buffer_out_0,
          msb_window_buffer_out_1 => msb_window_buffer_out_1,
          msb_window_buffer_out_2 => msb_window_buffer_out_2,
          msb_window_buffer_out_3 => msb_window_buffer_out_3,
          msb_window_buffer_out_4 => msb_window_buffer_out_4,
          msb_window_buffer_out_5 => msb_window_buffer_out_5,
          msb_window_buffer_out_6 => msb_window_buffer_out_6,
          msb_window_buffer_out_7 => msb_window_buffer_out_7,
          msb_window_buffer_out_8 => msb_window_buffer_out_8,
          msb_line_buffer_out_0 => msb_line_buffer_out_0,
          msb_line_buffer_out_1 => msb_line_buffer_out_1
        );

   -- Clock process definitions

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
	msb_window_buffer_in_1 <= x"0000000000000001";	
	msb_window_buffer_in_2 <= x"0000000000000002";
	msb_window_buffer_in_4 <= x"0000000000000003";
	msb_window_buffer_in_5 <= x"0000000000000004";
	msb_window_buffer_in_7 <= x"0000000000000005";
	msb_window_buffer_in_8 <= x"0000000000000006";
	msb_line_buffer_in_0 <= x"0000000000000007";
	msb_line_buffer_in_1 <= x"0000000000000008";
	msb_inputs <= x"0009";

      -- insert stimulus here 

      wait;
   end process;

END;
