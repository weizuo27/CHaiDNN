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

#ifndef _IP110_DEFINES_H_
#define _IP110_DEFINES_H_

	#include <ap_int.h>
	#include <hls_stream.h>

//#define XI_DIET_CHAI_Z 		0 //Enable this Macro to reduce the resource utilization of the design to fit to smaller devices
//#define XI_DIET_CHAI_ZUPLUS	0 //Enable this Macro to reduce the resource utilization of the design to fit to smaller devices

	#define DBG_INFO 1
	#define COMPUTE_OFF 1
	#define PRINT_LINEBUFFER_CONTENT 0
	#define PRINT_PINGPONGBUFFER_CONTENT 0
	#define PRINT_FEEDING_BUFF 1
	#define PRINT_LOAD_KERNEL 0
	
	#define LINEBUFFER_PORT 1
	#define LAYER1INCLUDED 1
	#define FUTRUE_IMPLEMENTATION 0


	//**** URAM ENABLE FLAGS FOR IP110/IP110.cpp
	#define XI_BIAS_URAM_EN 	0
	#define XI_WTS_URAM_EN 		0
	#define XI_ISTG_URAM_EN 	0
	#define XI_OSTG_URAM_EN 	0
	#define XI_FEED_URAM_EN 	0
	#define XI_SCALE_URAM_EN 	0

	//**** CONFIGURABLE MACROS
	#if XI_DIET_CHAI_Z || XI_DIET_CHAI_ZUPLUS
		#define XI_KER_PROC       	8
		#define XI_PIX_PROC       	8
		#define XI_ISTAGEBUFF_DEPTH 1024
		#define XI_OSTAGEBUFF_DEPTH 1024
		#define XI_WEIGHTBUFF_DEPTH 1024
	#else
		#define XI_KER_PROC       	8		//8
		#define XI_PIX_PROC       	16 		//16
		#define XI_ISTAGEBUFF_DEPTH 2048	//2048
		#define XI_OSTAGEBUFF_DEPTH 512  	//1024
		#define XI_WEIGHTBUFF_DEPTH 512 	//1024	
	#endif

//**DON'T MODIFY THE BELOW MACROS**//
#if XI_DIET_CHAI_Z
	#define XI_64BIT_PORT_EN    	1 
	#define XI_POOL_PROC_8_PLN 		1
	#define XI_DISABLE_BN 			1
	#define XI_OSTG_BUFFER_SET 		4
	#define XI_SINGLE_IO_PORT_EN 	0
#elif XI_DIET_CHAI_ZUPLUS
	#define XI_64BIT_PORT_EN    	0 
	#define XI_POOL_PROC_8_PLN 		1
	#define XI_DISABLE_BN 			1
	#define XI_OSTG_BUFFER_SET 		4
	#define XI_SINGLE_IO_PORT_EN 	1
#else
	#define XI_64BIT_PORT_EN    	0
	#define XI_POOL_PROC_8_PLN 		0
	#define XI_DISABLE_BN 			0
	#define XI_OSTG_BUFFER_SET 		8
	#define XI_SINGLE_IO_PORT_EN 	0
#endif

#define XI_DP_ENABLE 			1
#define XI_DISABLE_SOS 			1
#define XI_ODBC_EN 				0
#define XI_RESULT_BUFFER_FF 	0
#define XI_RESULT_BUFFER_LUTRAM 0
#define XI_BATCH1_EN 	  		0
#define XI_MEANSUB_EN 			0

#if XI_DIET_CHAI_Z ||XI_DIET_CHAI_ZUPLUS || XI_WTS_URAM_EN
	#define XI_MERGE_WEIGHTBUFF		1
#else
	#define XI_MERGE_WEIGHTBUFF		0
#endif	

#if XI_64BIT_PORT_EN
	#define XI_IO_64bit_PORT_EN 1
	#define XI_WTS_PORT_64BIT_EN 1
#else
	#define XI_IO_64bit_PORT_EN 0
	#define XI_WTS_PORT_64BIT_EN 0	
#endif

#define PIX_PROC_ODD 	  	(XI_PIX_PROC%2==1)
#if !XI_SINGLE_IO_PORT_EN
	#define LOG2_NUM_PORT_IO 1
#else
	#define LOG2_NUM_PORT_IO 0
#endif

#if !XI_SINGLE_IO_PORT_EN && !XI_IO_64bit_PORT_EN
	#define OUT_PARTITION 4
#else
	#define OUT_PARTITION 2
#endif

#define XI_ROW8 (XI_ISTAGEBUFF_DEPTH>=4096)
#define XI_ROW4 (XI_ISTAGEBUFF_DEPTH>=2048) & (XI_ISTAGEBUFF_DEPTH<4096)
#define XI_ROW2 (XI_ISTAGEBUFF_DEPTH<2048)

#if (XI_ISTAGEBUFF_DEPTH<=1024)
typedef ap_uint<10> istg_datatype;
#elif (XI_ISTAGEBUFF_DEPTH<=2048)
typedef ap_uint<11> istg_datatype;
#elif (XI_ISTAGEBUFF_DEPTH<=4096)
typedef ap_uint<12> istg_datatype;
#elif (XI_ISTAGEBUFF_DEPTH<=8192)
typedef ap_uint<13> istg_datatype;
#endif

///******END********
#define __SDSOC 1




typedef unsigned long long int gmem_inputtype_layer1;
#if XI_IO_64bit_PORT_EN
typedef ap_uint<64> gmem_inputtype_layerx;
#else
typedef ap_uint<128> gmem_inputtype_layerx;
#endif
typedef  unsigned long long int gmem_biastype;

#if XI_IO_64bit_PORT_EN
typedef  ap_uint<64> gmem_outputtype;
#else
typedef  ap_uint<128> gmem_outputtype;
#endif

typedef ap_int<8> weight_width;

typedef ap_uint<64> inputtype ;
#define XI_INPUTPACKCOUNT_LOG2		2
#if XI_IO_64bit_PORT_EN
typedef ap_uint<64> inputtype2 ;
#define XI_INPUTPACKCOUNT2_LOG2		3 - LOG2_NUM_PORT_IO
#else
typedef ap_uint<128> inputtype2 ;
#define XI_INPUTPACKCOUNT2_LOG2		4 - LOG2_NUM_PORT_IO
#endif

#if XI_KER_PROC ==8

	#if XI_WTS_PORT_64BIT_EN
	typedef  ap_uint<64>  gmem_weighttype;
	#else
	typedef  ap_uint<128>  gmem_weighttype;
	#endif
	#define XI_WEIGHTPACKCOUNT_LOG2		5

#else//16 KP

	typedef  ap_uint<128>  gmem_weighttype;
	#define XI_WEIGHTPACKCOUNT_LOG2		6

#endif//KP

typedef ap_uint<64> biastype ;
#define XI_BIASMAXSIZE			1024//512
#define XI_BIASPACKCOUNT_LOG2		2
#if XI_IO_64bit_PORT_EN
	typedef ap_uint<64> outtype ;
	#define XI_OUTPUTPACKCOUNT_LOG2		3 - LOG2_NUM_PORT_IO
#else
	typedef ap_uint<128> outtype ;
	#define XI_OUTPUTPACKCOUNT_LOG2		4 - LOG2_NUM_PORT_IO
#endif

#if 1
	#define XI_IN_H 227 //layer1
	#define XI_IN_W 227 //layer1
	#define XI_OUT_H 55 //layer1
	#define XI_OUT_W 55 //layer1
	#define XI_NUM_KERNELS 96
	#define XI_FILTER_SIZE 11
	#define XI_CONV_STRIDE 4
	#define XI_POOL_STRIDE 2
	#define XI_POOL_SIZE 3
	#define XI_INPUT_PLANES 4
	#define XI_NKPF 8//4
	#define XI_PAD 0
#endif



#endif//_XI_CONV_CONFIG_H_
