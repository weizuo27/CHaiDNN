###Global variables (need to be set appropriately)
set board_name "zcu102"
set part_name "xczu9eg-ffvb1156-2-e"
set board_part_name "xilinx.com:zcu102:part0:3.2"
set proj_path "."

### Create project
create_project $board_name $proj_path/$board_name -force -part $part_name
set_property board_part $board_part_name [current_project]

### Create block design for the board
create_bd_design $board_name
make_wrapper \
-files [get_files $proj_path/$board_name/$board_name.srcs/sources_1/bd/$board_name/$board_name.bd] -top

add_files -norecurse $proj_path/$board_name/$board_name.srcs/sources_1/bd/$board_name/hdl/${board_name}_wrapper.v

### Add board IP to block design [Embedded Processing -> Processor -> <select the board for custom design>]
create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e ps_e
apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells ps_e]
set_property -dict [list CONFIG.PSU__USE__M_AXI_GP0 {0} CONFIG.PSU__USE__M_AXI_GP1 {0} \
CONFIG.PSU__USE__IRQ1 {1} CONFIG.PSU__HIGH_ADDRESS__ENABLE {1}] [get_bd_cells ps_e]

### Add Clock wizard IP to block design [FPGA Features and Design -> Clocking -> Clocking Wizard]
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0
set_property -dict [list CONFIG.PRIMITIVE {Auto} CONFIG.CLKOUT1_USED {true} \
CONFIG.CLKOUT2_USED {true} CONFIG.USE_RESET {true} CONFIG.USE_LOCKED {true} \
CONFIG.RESET_TYPE {ACTIVE_LOW} CONFIG.NUM_OUT_CLKS {2} CONFIG.RESET_PORT {resetn} \
CONFIG.CLKOUT1_MATCHED_ROUTING {true} CONFIG.CLKOUT2_MATCHED_ROUTING {true}] \
[get_bd_cells clk_wiz_0]

set_property -dict [list CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {500.000} CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {250.000} CONFIG.MMCM_DIVCLK_DIVIDE {1} CONFIG.MMCM_CLKFBOUT_MULT_F {10} CONFIG.MMCM_CLKOUT0_DIVIDE_F {2} CONFIG.MMCM_CLKOUT1_DIVIDE {4} CONFIG.CLKOUT1_JITTER {97.091} CONFIG.CLKOUT1_PHASE_ERROR {98.582} CONFIG.CLKOUT2_JITTER {110.219} CONFIG.CLKOUT2_PHASE_ERROR {98.582}] [get_bd_cells clk_wiz_0]


###Add Reset IP to block design [Embedded Processing -> Clock & Reset-> Processor System Reset]
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_1

### Add Concat IP to block design [Basic Elements -> Concat]
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_1
set_property -dict [list CONFIG.NUM_PORTS {1}] [get_bd_cells xlconcat_0]
set_property -dict [list CONFIG.NUM_PORTS {1}] [get_bd_cells xlconcat_1]
### Connections
connect_bd_net [get_bd_pins clk_wiz_0/resetn] [get_bd_pins proc_sys_reset_0/ext_reset_in]
connect_bd_net [get_bd_pins clk_wiz_0/resetn] [get_bd_pins proc_sys_reset_1/ext_reset_in]


connect_bd_net [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked]
connect_bd_net [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_1/dcm_locked]


connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins proc_sys_reset_0/slowest_sync_clk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins proc_sys_reset_1/slowest_sync_clk]

connect_bd_net [get_bd_pins ps_e/pl_resetn0] [get_bd_pins clk_wiz_0/resetn]
connect_bd_net [get_bd_pins ps_e/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins ps_e/pl_ps_irq0]
connect_bd_net [get_bd_pins xlconcat_1/dout] [get_bd_pins ps_e/pl_ps_irq1]

set_property SELECTED_SIM_MODEL tlm_dpi [get_bd_cells /ps_e]

#create pfm
set_property PFM_NAME "xilinx.com:$board_name:$board_name:1.0" \
[get_files $proj_path/$board_name/$board_name.srcs/sources_1/bd/$board_name/$board_name.bd]
set_property PFM.CLOCK { \
				clk_out1 {id "0" is_default "false" proc_sys_reset "proc_sys_reset_0" } \
				clk_out2 {id "1" is_default "true" proc_sys_reset "proc_sys_reset_1" } \
} [get_bd_cells /clk_wiz_0]
set_property PFM.AXI_PORT { \
				M_AXI_HPM0_FPD {memport "M_AXI_GP"} \
				M_AXI_HPM1_FPD {memport "M_AXI_GP"} \
				M_AXI_HPM0_LPD {memport "M_AXI_GP"} \
				S_AXI_HPC0_FPD {memport "S_AXI_HPC" sptag "HPC0" memory "ps_e HPC0_DDR_LOW"} \
				S_AXI_HPC1_FPD {memport "S_AXI_HPC" sptag "HPC1" memory "ps_e HPC1_DDR_LOW"} \
				S_AXI_HP0_FPD {memport "S_AXI_HP" sptag "HP0" memory "ps_e HP0_DDR_LOW"} \
				S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "ps_e HP1_DDR_LOW"} \
				S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "ps_e HP2_DDR_LOW"} \
				S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "ps_e HP3_DDR_LOW"} \
} [get_bd_cells /ps_e]
set intVar []
for {set i 0} {$i < 8} {incr i} {
				lappend intVar In$i {}
}
set_property PFM.IRQ $intVar [get_bd_cells /xlconcat_0]
set_property PFM.IRQ $intVar [get_bd_cells /xlconcat_1]

##spit out a DSA
generate_target all [get_files $proj_path/$board_name/$board_name.srcs/sources_1/bd/$board_name/$board_name.bd]
set_property dsa.vendor         "xilinx.com" [current_project]
set_property dsa.board_id       $board_name [current_project]
set_property dsa.name           $board_name [current_project]
set_property dsa.version        "1.0" [current_project]
set_property dsa.platform_state "pre_synth" [current_project]
set_param dsa.writeHDFData 1
write_dsa -force -unified ./$board_name.dsa

