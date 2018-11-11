#ifndef __HEAD_H__
#define __HEAD_H__
#include "/opt/Xilinx/Vivado/2018.2/include/gmp.h"
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#define N 32

//typedef ap_axiu<32,4,5,5> AXI_VALUE;
typedef int AXI_VALUE;
typedef hls::stream<AXI_VALUE> AXI_STREAM;

void foo_IP1 ( uint *in, AXI_STREAM &out, int *dest_out);
#endif
