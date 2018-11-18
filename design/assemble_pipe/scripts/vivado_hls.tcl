############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project ConvPipeline
set_top ConvPipeline
add_files ../IP110/IP110.cpp
add_files ../IP110/IP110.h
add_files ../IP217/IP217.cpp
add_files ../IP217/IP217.h
add_files ../IP36/IP36.cpp
add_files ../IP36/IP36.h
add_files ../src/pipeSystem.cpp
open_solution "solution" -reset
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 4 
set_clock_uncertainty 27.0%
config_interface -m_axi_addr64
config_rtl -reset_level low
config_rtl -prefix a0_
csynth_design
export_design -ipname ConvPipeline -acc
exit
