----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 06.06.2021 13:08:46
-- Design Name: 
-- Module Name: binary_conv_3x3_tile - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity binary_conv_3x3_tile is
    Port ( 
           clk : in  STD_LOGIC ;
           reset : in  STD_LOGIC ;
           en : in  STD_LOGIC ;
           row : in  STD_LOGIC_VECTOR (31 downto 0);
           col : in  STD_LOGIC_VECTOR (31 downto 0);
           H_fmap_out : in  STD_LOGIC_VECTOR (31 downto 0);
           in_channels : in  STD_LOGIC_VECTOR (31 downto 0);
           switch_on : in  STD_LOGIC;
           threshold : in  STD_LOGIC;
	   msb_window_buffer : in  STD_LOGIC_VECTOR (63 downto 0);
           weights : in  STD_LOGIC_VECTOR (63 downto 0);
           done : out  STD_LOGIC;
           msb_accumulation : out  STD_LOGIC_VECTOR (15 downto 0)
    );
end binary_conv_3x3_tile;

architecture Behavioral of binary_conv_3x3_tile is

type state_type is (idle, condition, loop_1, loop_2, load_row_col, inter_calc, condition_2, load_a_w, wait_cmp_eng, calc_1, calc_2, calc_3, accumulation_done);
signal s_msb_accumulation_reg, s_msb_accumulation_next: unsigned (15 downto 0) := (others => '0'); 
-- channel_pt: unsigned (4 downto 0) := "10000"; 
signal s_row_reg, s_row_next: unsigned (31 downto 0); 
signal s_col_reg, s_col_next: unsigned (31 downto 0);
signal k_row_reg, k_row_next: unsigned (1 downto 0) := "11"; 
signal k_col_reg, k_col_next: unsigned (1 downto 0) := "11";
signal row_idx_pad_reg, row_idx_pad_next: unsigned (31 downto 0); 
signal col_idx_pad_reg, col_idx_pad_next: unsigned (31 downto 0);
signal msb_a_reg, msb_a_next, w_reg, w_next: STD_LOGIC_VECTOR (63 downto 0);
signal tmp_calcs_reg, tmp_calcs_next: unsigned (31 downto 0);
signal cmp_eng_result_reg, cmp_eng_result_next, cmp_eng_result: STD_LOGIC_VECTOR (7 downto 0);
signal en_cmp_eng: STD_LOGIC := '0';
signal done_cmp_eng: STD_LOGIC := '0';
signal state_reg, state_next: state_type;



component compute_engine_64
	port(
		b: in std_logic_vector(63 downto 0);
		w: in std_logic_vector(63 downto 0);
		clk: in std_logic;
		reset: in std_logic;
		en: in std_logic;
	    	done: out std_logic; 
		output: out std_logic_vector(7 downto 0)
	);
end component;

begin
process(clk,reset)
	begin
		if(reset = '1') then
			state_reg <= idle;
			k_row_reg <= "11";
			k_col_reg <= "11";
			s_row_reg <= (others => '0');
			s_col_reg <= (others => '0');
			row_idx_pad_reg <= (others => '0');
			col_idx_pad_reg <= (others => '0');
			msb_a_reg <= (others => '0');
			w_reg <= (others => '0');
			s_msb_accumulation_reg  <= (others => '0');
			cmp_eng_result_reg <= (others => '0');
			tmp_calcs_reg <= (others => '0');
		elsif(rising_edge(clk)) then
			state_reg <= state_next;
			k_row_reg <= k_row_next;
			k_col_reg <= k_col_next;
			s_row_reg <= s_row_next;
			s_col_reg <= s_col_next;
			row_idx_pad_reg <= row_idx_pad_next;
			col_idx_pad_reg <= col_idx_pad_next;
			msb_a_reg <= msb_a_next;
			w_reg <= w_next;
			s_msb_accumulation_reg  <= s_msb_accumulation_next;
			cmp_eng_result_reg <= cmp_eng_result_next;
			tmp_calcs_reg <= tmp_calcs_next;
        	end if;
end process;


process(state_reg, msb_window_buffer, weights, done_cmp_eng)
	begin
		done <= '0';
		msb_accumulation <= (others => '0');
		en_cmp_eng <= '0';
		k_row_next <= k_row_reg;
	    	k_col_next <= k_col_reg;
	    	s_row_next <= s_row_reg;
	    	s_col_next <= s_col_reg;
	    	row_idx_pad_next <= row_idx_pad_reg;
	    	col_idx_pad_next <= col_idx_pad_reg;
	    	msb_a_next <= msb_a_reg;
	    	w_next <= w_reg;
	    	s_msb_accumulation_next <= s_msb_accumulation_reg;
	    	cmp_eng_result_next <= cmp_eng_result_reg;
	    	tmp_calcs_next <= tmp_calcs_reg;
		case state_reg is	
		when idle =>
			if(en = '1') then                  
			    state_next <= condition;
			else
			    state_next <= idle;
			end if;
		when condition =>
		    if(switch_on = '1' or threshold = '1') then
			state_next <= loop_1;    
		    else
			state_next <= accumulation_done;
		    end if;           
		when loop_1 =>
			if(k_row_reg > 0) then
			    k_row_next <= k_row_reg - 1;
			    state_next <= loop_2;
			else
			    state_next <= accumulation_done;
			end if;
		when loop_2 =>
		    if(k_col_reg > 0) then
		    k_col_next <= k_col_reg - 1;
		    state_next <= load_row_col;
		    else
		    state_next <= loop_1;
		    k_col_next <= "11";
		end if;
	    when load_row_col =>
		s_row_next <= unsigned(row);
		s_col_next <= unsigned(col);
		state_next <= inter_calc;
			when inter_calc =>
		row_idx_pad_next <= s_row_reg - 2 + k_row_reg;
		col_idx_pad_next <= s_col_reg - 2 + k_col_reg;
		state_next <= condition_2;
	    when condition_2 =>
		if(row_idx_pad_reg >= 0 and row_idx_pad_reg < unsigned(H_fmap_out) and col_idx_pad_reg >= 0 and col_idx_pad_reg < unsigned(H_fmap_out)) then
		    state_next <= load_a_w;
		else
		    state_next <= loop_2;
		end if;
	   when load_a_w =>
		msb_a_next <= msb_window_buffer;
		w_next <= weights;
		en_cmp_eng <= '1';
		state_next <= wait_cmp_eng;
	   when wait_cmp_eng =>
	    	if(done_cmp_eng = '1') then
		    cmp_eng_result_next <= cmp_eng_result;
		    state_next <= calc_1;
		else
		    state_next <= wait_cmp_eng;
		end if;
	    when calc_1 =>
		tmp_calcs_next <= unsigned(x"000000" & cmp_eng_result_reg(6 downto 0) & '0');
		state_next <= calc_2;
	    when calc_2 =>
		tmp_calcs_next <= unsigned(in_channels) - tmp_calcs_reg;
		state_next <= calc_3;
	    when calc_3 =>
		s_msb_accumulation_next <= s_msb_accumulation_reg + tmp_calcs_reg(15 downto 0);
		state_next <= loop_2;
	    when accumulation_done =>
		done <= '1';
		msb_accumulation <= std_logic_vector(s_msb_accumulation_reg);
		state_next <= idle;			    		
		end case;
	end process;


cmp_eng: compute_engine_64 port map(  b =>       msb_a_reg,
                                      w =>       w_reg,
                                      clk =>     clk,
                                      reset =>   reset,
                                      en =>      en_cmp_eng,
                                      done =>    done_cmp_eng,
                                      output =>  cmp_eng_result
                                      );

end Behavioral;
