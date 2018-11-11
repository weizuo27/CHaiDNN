open_project IP2
set_top foo_IP2
add_files foo_IP2.cpp
add_files head_ip2.hpp
add_files -tb foo_IP2_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 10 -name default
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
