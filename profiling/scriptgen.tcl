open_project CHaiPipeline
set_top XiConvolutionTop
add_files ./conv_pipe/tb/testbench_defines.h
add_files ./conv_pipe/tb/verification.cpp
add_files ./conv_pipe/src/xi_convolution.hpp
add_files ./conv_pipe/src/xi_convolution_top.cpp
add_files ./conv_pipe/include/defines.h
add_files ./conv_pipe/include/xi_conv_config.h
add_files ./conv_pipe/include/xi_conv_desc.h
open_solution "Sol_512_512_512_8_32"
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
csynth_design
export_design -flow syn -rtl verilog -format ip_catalog
