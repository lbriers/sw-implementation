--------------------------------------------------------------------------------
-- KU Leuven - ESAT/COSIC - Emerging technologies, Systems & Security
--------------------------------------------------------------------------------
-- Module Name:     riscv_microcontroller_tb - Behavioural
-- Project Name:    Testbench for RISC-V microcontroller
-- Description:     
--
-- Revision     Date       Author     Comments
-- v0.1         20241128   VlJo       Initial version
--
--------------------------------------------------------------------------------
library IEEE;
    use IEEE.STD_LOGIC_1164.ALL;

library work;
    use work.PKG_hwswcd.ALL;

entity riscv_microcontroller_tb is
    generic (
        G_DATA_WIDTH : integer := 32;
        G_DEPTH_LOG2 : integer := 11;
        FNAME_OUT_FILE : string := "/home/luca/Documents/school/master/hwsw-cd/sw-implementation/firmware/course_example_1/simulation_output.dat"

    );
end entity riscv_microcontroller_tb;

architecture Behavioural of riscv_microcontroller_tb is
    --basicio
    signal dmem_di, dmem_a : STD_LOGIC_VECTOR(31 downto 0);
    signal dmem_we: STD_LOGIC;
    signal cpu_clock : STD_LOGIC;
    
    -- clock and reset
    signal sys_clock : STD_LOGIC;
    signal sys_reset : STD_LOGIC;
    signal external_irq : STD_LOGIC;
    signal gpio_leds : STD_LOGIC_VECTOR(31 downto 0);

    -- constants
    constant C_ZEROES: STD_LOGIC_VECTOR(G_DATA_WIDTH-1 downto 0) := (others => '0');
    constant clock_period : time := 8 ns;

begin

    PSTIM: process
    begin
        -- external_irq <= '0';
        -- wait for 30 us;
        -- external_irq <= '1';
        -- wait for 1 us;
        -- external_irq <= '0';
        -- wait for 30 us;
        -- external_irq <= '1';
        -- wait for 1 us;
        -- external_irq <= '0';
        -- wait for 30 us;
        -- external_irq <= '1';
        -- wait for 1 us;
        -- external_irq <= '0';
        -- wait;
    end process PSTIM;

    -------------------------------------------------------------------------------
    -- DUT
    -------------------------------------------------------------------------------
    DUT: component riscv_microcontroller port map(
        sys_clock => sys_clock,
        sys_reset => sys_reset,
        external_irq => external_irq,
        gpio_leds => gpio_leds,
        dmem_di_e => dmem_di,
        dmem_a_e => dmem_a,
        dmem_we_e => dmem_we,
        cpu_clock => cpu_clock
    );

    -------------------------------------------------------------------------------
    -- basicIO 
    -------------------------------------------------------------------------------
    basicIO_model_inst00: component basicIO_model generic map(
        G_DATA_WIDTH => G_DATA_WIDTH,
        FNAME_OUT_FILE => FNAME_OUT_FILE
    ) port map(
        clock => cpu_clock,
        reset => sys_reset,
        di => dmem_di,
        ad => dmem_a,
        we => dmem_we,
        do => open
    );

    -------------------------------------------------------------------------------
    -- CLOCK
    -------------------------------------------------------------------------------
    PCLK: process
    begin
        sys_clock <= '1';
        wait for clock_period/2;
        sys_clock <= '0';
        wait for clock_period/2;
    end process PCLK;


    -------------------------------------------------------------------------------
    -- RESET
    -------------------------------------------------------------------------------
    PRST: process
    begin
        sys_reset <= '1';
        wait for clock_period*9;
        wait for clock_period/2;
        sys_reset <= '0';
        wait;
    end process PRST;

end Behavioural;
