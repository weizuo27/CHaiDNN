/*----------------------------------------------------
Copyright 2017 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
----------------------------------------------------*/

#ifndef _AU_MAXPOOL_CONFIG_H_
#define _AU_MAXPOOL_CONFIG_H_
#define RESERVE 0

#include <ap_int.h>
#include <hls_stream.h>
#define GMEM_MAXPOOLTYPE   ap_uint<128>
#define GMEM_POOLTYPE_WTS  ap_uint<128>

#define GMEM_ELEMENT   4
#define GMEM_SHIFT_CNT 2

#define NPC1 GMEM_SHIFT_CNT

#define NUM_KERNELS 8
#define OUTWIDTH	480
#define OUTHEIGHT	480
#define DBG_INFO 1

//*****************LINE BUFFER CONFIGURATIONS BEGIN**********************//
//* Due to the address warpping implementation, please make sure the line buffer size to be 2's power 
//* and change the bitwith of its address type to fit the buffer size 

#define IN_LINE_BUFFER_SIZE 1024
typedef ap_uint<10> inLineBuffAddr_t;

#define OUT_LINE_BUFFER_SIZE 512
typedef ap_uint<9> outLineBuffAddr_t;
//*****************LINE BUFFER CONFIGURATIONS END **********************//

//*****************INDEX DATATYPE SPECIFICATION**********************//
//*change them to larger one if the dimensions are larger
typedef ap_uint<16> uPixelIdx_t;
typedef ap_int<17> 	sPixelIdx_t;
typedef ap_uint<8> 	uRowIdx_t; 
typedef ap_int<9> 	sRowIdx_t;
typedef ap_uint<16> uPlnIdx_t;
typedef ap_int<8> 	sPlnPackIdx_t;
typedef ap_uint<4>  uDimPool_t;
typedef ap_uint<8> 	uSizePool_t;



#define DEPTHPACK 16

#define OUTPUT_SIZE NUM_KERNELS*OUTWIDTH*OUTHEIGHT

#pragma SDS data zero_copy(in1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(out1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(in2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(out2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(wts[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(scalar_pool_args[0:16])

#pragma SDS data sys_port(in1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(in2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(out1:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(out2:ps_e_S_AXI_HP3_FPD)
#pragma SDS data sys_port(wts:ps_e_S_AXI_HP0_FPD)

#ifndef __SDSOC
int PoolTop(GMEM_MAXPOOLTYPE *in1, GMEM_MAXPOOLTYPE *in2, GMEM_MAXPOOLTYPE *out1, GMEM_MAXPOOLTYPE *out2, 
#if !RESERVE
GMEM_POOLTYPE_WTS* wts, 
#endif
int *scalar_pool_args);
#else
void PoolTop(GMEM_MAXPOOLTYPE *in1, GMEM_MAXPOOLTYPE *in2, GMEM_MAXPOOLTYPE *out1, GMEM_MAXPOOLTYPE *out2, GMEM_POOLTYPE_WTS* wts, int *scalar_pool_args);
#endif


int PooltopPipeline
(
	#if STREAMINTERFACE
    hls::stream<GMEM_MAXPOOLTYPE> &in1, 
	hls::stream<GMEM_MAXPOOLTYPE> &in2, 
	hls::stream<GMEM_MAXPOOLTYPE> &out1, 
	hls::stream<GMEM_MAXPOOLTYPE> &out2,
	#else 
	GMEM_MAXPOOLTYPE *in1,
	GMEM_MAXPOOLTYPE *in2,
	GMEM_MAXPOOLTYPE *out1,
	GMEM_MAXPOOLTYPE *out2,
	#endif	
	int *scalar_pool_args
);




#define STREAMIN
#define STREAMOUT

#endif // _AU_MAXPOOL_CONFIG_H_
