open_project CHaiPipelineset_top PoolTopadd_files /home/xliu79/Research/2018Fall/chaisimplify/design/pool_pipe/src/pooling_layer_dp_2xio_top.cpp
add_files /home/xliu79/Research/2018Fall/chaisimplify/design/pool_pipe/include/pool_layer.hpp
add_files /home/xliu79/Research/2018Fall/chaisimplify/design/pool_pipe/include/pool_dp_2xio.hpp
add_files /home/xliu79/Research/2018Fall/chaisimplify/design/pool_pipe/include/maxpool_config.h
open_solution "Sol_3_4_5_6_7"
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
csynth_design
export_design -flow syn -rtl verilog -format ip_catalog
