open_project IP1
set_top foo_IP1
add_files foo_IP1.cpp
add_files head_ip1.hpp
add_files -tb foo_IP1_test.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 10 -name default
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
