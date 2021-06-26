library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity compute_engine_64 is
	port(
		b: in std_logic_vector(63 downto 0);
		w: in std_logic_vector(63 downto 0);
		clk: in std_logic;
		reset: in std_logic;
		en: in std_logic;
	    done: out std_logic; 
		output: out std_logic_vector(31 downto 0)
	);
end compute_engine_64;

architecture arch of compute_engine_64 is
	constant m1: std_logic_vector(63 downto 0):= "0101010101010101010101010101010101010101010101010101010101010101"; --m1 = 6148914691236517205	
	constant m2: std_logic_vector(63 downto 0):= "0011001100110011001100110011001100110011001100110011001100110011"; --m2 = 3689348814741910323
	constant m4: std_logic_vector(63 downto 0):= "0000111100001111000011110000111100001111000011110000111100001111"; --m4 = 1085102592571150095

	signal x0: std_logic_vector(63 downto 0); --x0 = b ^ w
	signal x1: std_logic_vector(63 downto 0); --x1 = (x0 >> 1) & m1 
	signal x2: std_logic_vector(63 downto 0); --x2 = x0 - x1
	signal x3: std_logic_vector(63 downto 0); --x3 = (x2 & m2) + ((x2 >> 2) & m2);
	signal x4: std_logic_vector(63 downto 0); --x4 = (x3 + (x3 >> 4)) & m4
	signal x5: std_logic_vector(63 downto 0); --x5 = x4 >> 8
	signal x6: std_logic_vector(63 downto 0); --x6 = x4 + x5
	signal x7: std_logic_vector(63 downto 0); --x7 = x6 >> 16
	signal x8: std_logic_vector(63 downto 0); --x8 = x6 + x7
	signal x9: std_logic_vector(63 downto 0); --x9 = x8 >> 32
	signal x10: std_logic_vector(63 downto 0); --x10 = x8 + x9
	signal x11: std_logic_vector(63 downto 0); --x11 = x10 & 0x7f
	type state_type is (idle, op_done);
	signal state_reg, state_next: state_type;
begin
	x0 <= b xor w;
	x1 <= ('0' & x0(63 downto 1)) and m1;
	x2 <= x0 - x1;
	x3 <= (x2 and m2) + (("00" & x2(63 downto 2)) and m2);
	x4 <= (x3 + ("0000" & x3(63 downto 4))) and m4;
	x5 <= "00000000" & x4(63 downto 8);
	x6 <= x4 + x5;
	x7 <= "0000000000000000" & x6(63 downto 16);
	x8 <= x6 + x7;
	x9 <= "00000000000000000000000000000000" & x8(63 downto 32);
	x10 <= x8 + x9;
	x11 <= x10 and x"000000000000007f";
	
	process(clk,reset)
        begin
            if(reset = '0') then
                state_reg <= idle;
            elsif(rising_edge(clk)) then
                state_reg <= state_next;
            end if;
    end process;
    
    process(state_reg, en)
        begin
        done <= '0';
        output <= (others => '0');
            case state_reg is    
                when idle =>
                    if(en = '1') then
                        state_next <= op_done;
                    else
                        state_next <= idle;
                    end if;
                when op_done =>
                     done <= '1';
                     output <= x11(31 downto 0); 
                     state_next <= idle;            
            end case;
        end process;
end arch;