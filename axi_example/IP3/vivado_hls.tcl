open_project IP3
set_top foo_IP3
add_files foo_IP3.cpp
add_files head_ip3.hpp
add_files -tb foo_IP3_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 10 -name default
#source "./axi_stream_example/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
