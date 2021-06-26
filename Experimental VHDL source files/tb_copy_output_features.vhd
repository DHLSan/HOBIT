--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   09:11:37 05/08/2021
-- Design Name:   
-- Module Name:   D:/XilinxProjects/binary_conv3x3_tile/tb_copy_output_features.vhd
-- Project Name:  binary_conv3x3_tile
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: copy_output_features
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
 
ENTITY tb_copy_output_features IS
END tb_copy_output_features;
 
ARCHITECTURE behavior OF tb_copy_output_features IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT copy_output_features
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         start : IN  std_logic;
         msb_outputs : IN  std_logic_vector(15 downto 0);
         c_in : IN  std_logic_vector(31 downto 0);
         ready : OUT  std_logic;
         msb_partial_out_feature : OUT  std_logic_vector(15 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal start : std_logic := '0';
   signal msb_outputs : std_logic_vector(15 downto 0) := (others => '0');
   signal c_in : std_logic_vector(31 downto 0) := (others => '0');

 	--Outputs
   signal ready : std_logic;
   signal msb_partial_out_feature : std_logic_vector(15 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: copy_output_features PORT MAP (
          clk => clk,
          reset => reset,
          start => start,
          msb_outputs => msb_outputs,
          c_in => c_in,
          ready => ready,
          msb_partial_out_feature => msb_partial_out_feature
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      reset <= '1';
      start <= '0';
      msb_outputs <= x"0008";
      c_in <= x"00000001";
      wait for 10 ns;
      reset <= '0';
      start <= '1';
      
      

      -- insert stimulus here 

      wait;
   end process;

END;
