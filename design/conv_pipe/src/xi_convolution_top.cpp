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

#include "../include/xi_conv_config.h"
#include "../include/xi_conv_desc.h"
typedef ap_int<27> dsp27_t;
#if XI_DP_ENABLE
#include <dsp_builtins.h>
#endif
#include "xi_convolution.hpp"

#ifndef __SDSOC
int XiConvolutionTop(
#else
		void XiConvolutionTop(
#endif
				gmem_weighttype *weights1,
				gmem_weighttype *weights2,
#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8))
				gmem_weighttype *weights3,
				gmem_weighttype *weights4,
#endif
		#if !XI_SINGLE_IO_PORT_EN
			#if STREAMOUT == 0 
				gmem_outputtype *output1,
				gmem_outputtype *output2,			
			#elif STREAMOUT ==1
				hls::stream< gmem_inputtype_layerx > & outStream1,
				hls::stream< gmem_inputtype_layerx > & outStream2,
			#else
				gmem_outputtype *output1,
				gmem_outputtype *output2,
				hls::stream< gmem_inputtype_layerx > & outStream1,
				hls::stream< gmem_inputtype_layerx > & outStream2,
			#endif
			#if STREAMIN == 0 
				gmem_inputtype_layerx *input_other1,
				gmem_inputtype_layerx *input_other2,
			#elif STREAMIN == 1
				hls::stream< gmem_inputtype_layerx > & inStream1,
    			hls::stream< gmem_inputtype_layerx > & inStream2,
			#else
				gmem_inputtype_layerx *input_other1,
				gmem_inputtype_layerx *input_other2,
				hls::stream< gmem_inputtype_layerx > & inStream1,
    			hls::stream< gmem_inputtype_layerx > & inStream2,
			#endif
		#else
			#if STREAMOUT == 0 
				gmem_outputtype *output1,	
			#elif STREAMOUT == 1
				hls::stream< gmem_inputtype_layerx > & outStream1,
			#else
				gmem_outputtype *output1,
				hls::stream< gmem_inputtype_layerx > & outStream1,
			#endif
			#if STREAMIN == 0 
				gmem_inputtype_layerx *input_other1,
			#elif STREAMIN == 1
				hls::stream< gmem_inputtype_layerx > & inStream1,
			#else
				gmem_inputtype_layerx *input_other1,
				hls::stream< gmem_inputtype_layerx > & inStream1,
			#endif
		#endif //* !XI_SINGLE_IO_PORT_EN

				gmem_inputtype_layer1 *input_1st,
				gmem_biastype *bias,
#if !XI_DISABLE_BN
				gmem_inputtype_layer1 *inp_norm_2,
				gmem_inputtype_layer1 *inp_norm_3,
#endif
				gmem_inputtype_layer1 *istg_out1,
#if !XI_SINGLE_IO_PORT_EN
				gmem_inputtype_layer1 *istg_out2,
#endif
				int *scalar_conv_args
#ifdef __SDSVHLS__
				, bool ap_clk_div2 ) {
#else
) {
	bool ap_clk_div2 = 0;
#endif

#ifndef __SDSOC
#pragma HLS interface m_axi port=weights1 		offset=slave bundle=gmem0 depth=576
#pragma HLS interface m_axi port=weights2 		offset=slave bundle=gmem1 depth=576
#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
#pragma HLS interface m_axi port=weights3 		offset=slave bundle=gmem2 depth=576
#pragma HLS interface m_axi port=weights4 		offset=slave bundle=gmem3 depth=576
#endif

#if STREAMOUT !=1
#pragma HLS interface m_axi port=output1 		offset=slave bundle=gmem4 depth=4096
#pragma HLS interface m_axi port=output2 		offset=slave bundle=gmem5 depth=4096
#endif
#if STREAMIN !=1
#pragma HLS interface m_axi port=input_other1 	offset=slave bundle=gmem6 depth=4096
#pragma HLS interface m_axi port=input_other2 	offset=slave bundle=gmem7 depth=4096
#endif
#pragma HLS interface m_axi port=input_1st 		offset=slave bundle=gmem8 depth=4096
#pragma HLS interface m_axi port=bias 			offset=slave bundle=gmem9 depth=64
#if !XI_DISABLE_BN
#pragma HLS interface m_axi port=inp_norm_2 	offset=slave bundle=gmem10 depth=4
#pragma HLS interface m_axi port=inp_norm_3 	offset=slave bundle=gmem11 depth=4
#endif
#pragma HLS interface m_axi port=istg_out1 		offset=slave bundle=gmem12 depth=4096
#pragma HLS interface m_axi port=istg_out2 		offset=slave bundle=gmem13 depth=4096
#pragma HLS interface m_axi port=scalar_conv_args offset=slave bundle=gmem14 depth=128

#pragma HLS interface s_axilite port=weights1 bundle=control
#pragma HLS interface s_axilite port=weights2 bundle=control
#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
#pragma HLS interface s_axilite port=weights3 bundle=control
#pragma HLS interface s_axilite port=weights4 bundle=control
#endif
#if STREAMOUT !=1
#pragma HLS interface s_axilite port=output1 bundle=control
#pragma HLS interface s_axilite port=output2 bundle=control
#endif
#if STREAMIN !=1
#pragma HLS interface s_axilite port=input_other1 bundle=control
#pragma HLS interface s_axilite port=input_other2 bundle=control
#endif

#pragma HLS interface s_axilite port=input_1st bundle=control
#pragma HLS interface s_axilite port=bias bundle=control
#pragma HLS interface s_axilite port=scalar_conv_args bundle=control
#if !XI_DISABLE_BN
#pragma HLS interface s_axilite port=inp_norm_2 bundle=control
#pragma HLS interface s_axilite port=inp_norm_3 bundle=control
#endif
#pragma HLS interface s_axilite port=istg_out1 bundle=control
#pragma HLS interface s_axilite port=istg_out2 bundle=control
#pragma HLS interface s_axilite port=return bundle=control
#endif 

	Convolution<XI_IN_W,XI_IN_H,XI_OUT_W,XI_OUT_H,XI_NUM_KERNELS,XI_FILTER_SIZE,XI_CONV_STRIDE,XI_POOL_STRIDE,XI_POOL_SIZE,XI_INPUT_PLANES,XI_NKPF>
	(weights1,weights2,
#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
			weights3,weights4,
#endif
		#if !XI_SINGLE_IO_PORT_EN
			#if STREAMOUT == 0
				output1, output2,
			#elif STREAMOUT == 1
				outStream1,outStream2,
			#else
				output1, output2,
				outStream1,outStream2,
			#endif
		#else
			#if STREAMOUT == 0
				output1, 
			#elif STREAMOUT == 1
				outStream1,
			#else
				output1,
				outStream1,
			#endif
		#endif
		#if !XI_SINGLE_IO_PORT_EN
			#if STREAMIN== 0
				input_other1, input_other2,
			#elif STREAMOUT == 1
				inStream1, inStream2,
			#else
				input_other1, input_other2,
				inStream1, inStream2,
			#endif
		#else
			#if STREAMIN == 0
				input_other1,
			#elif STREAMIN == 1
				inStream1, 
			#else
				input_other1,
				inStream1, 
			#endif
		#endif
		
			input_1st,bias,
#if !XI_DISABLE_BN
			inp_norm_2,inp_norm_3,
#endif
			istg_out1,
#if !XI_SINGLE_IO_PORT_EN
			istg_out2,
#endif
			scalar_conv_args,ap_clk_div2);
#ifndef __SDSOC
	//return 0;
#endif
}


// #ifndef __SDSOC
// void XiConvolutionTop_pipe(
// #else
// void XiConvolutionTop(
// #endif
// 				gmem_weighttype *weights1A,
// 				gmem_weighttype *weights2A,
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8))
// 				gmem_weighttype *weights3A,
// 				gmem_weighttype *weights4A,
// #endif
// 				gmem_weighttype *weights1B,
// 				gmem_weighttype *weights2B,
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8))
// 				gmem_weighttype *weights3B,
// 				gmem_weighttype *weights4B,
// #endif
//
// 				gmem_outputtype *output1A,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_outputtype *output2A,
// #endif
// 				gmem_outputtype *output1B,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_outputtype *output2B,
// #endif
// 				gmem_inputtype_layerx *input_other1A,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_inputtype_layerx *input_other2A,
// #endif
// 				gmem_inputtype_layerx *input_other1B,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_inputtype_layerx *input_other2B,
// #endif
//
// 				gmem_inputtype_layer1 *input_1stA,
// 				gmem_inputtype_layer1 *input_1stB,
// 				gmem_biastype *biasA,
// 				gmem_biastype *biasB,
// #if !XI_DISABLE_BN
// 				gmem_inputtype_layer1 *inp_norm_2,
// 				gmem_inputtype_layer1 *inp_norm_3,
// #endif
// 				gmem_inputtype_layer1 *istg_out1A,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_inputtype_layer1 *istg_out2A,
// #endif
// 				gmem_inputtype_layer1 *istg_out1B,
// #if !XI_SINGLE_IO_PORT_EN
// 				gmem_inputtype_layer1 *istg_out2B,
// #endif
// 				int *scalar_conv_argsA,
// 				int *scalar_conv_argsB
// #ifdef __SDSVHLS__
// 				, bool ap_clk_div2A
// 				, bool ap_clk_div2B
// 				) {
// #else
// ) {
// 	bool ap_clk_div2 = 0;
// #endif
//
// #ifndef __SDSOC
// #pragma HLS interface m_axi port=weights1A 		offset=slave bundle=gmem0A depth=576
// #pragma HLS interface m_axi port=weights2A 		offset=slave bundle=gmem1A depth=576
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// #pragma HLS interface m_axi port=weights3A		offset=slave bundle=gmem2A depth=576
// #pragma HLS interface m_axi port=weights4A 		offset=slave bundle=gmem3A depth=576
// #endif
// #pragma HLS interface m_axi port=output1A 		offset=slave bundle=gmem4A depth=4096
// #pragma HLS interface m_axi port=output2A		offset=slave bundle=gmem5A depth=4096
// #pragma HLS interface m_axi port=input_other1A 	offset=slave bundle=gmem6A depth=4096
// #pragma HLS interface m_axi port=input_other2A 	offset=slave bundle=gmem7A depth=4096
// #pragma HLS interface m_axi port=input_1stA 		offset=slave bundle=gmem8A depth=4096
// #pragma HLS interface m_axi port=biasA 			offset=slave bundle=gmem9A depth=64
// #if !XI_DISABLE_BN
// #pragma HLS interface m_axi port=inp_norm_2 	offset=slave bundle=gmem10 depth=4
// #pragma HLS interface m_axi port=inp_norm_3 	offset=slave bundle=gmem11 depth=4
// #endif
// #pragma HLS interface m_axi port=istg_out1A 		offset=slave bundle=gmem12A depth=4096
// #pragma HLS interface m_axi port=istg_out2A 		offset=slave bundle=gmem13A depth=4096
// #pragma HLS interface m_axi port=scalar_conv_argsA offset=slave bundle=gmem14A depth=128
//
// #pragma HLS interface s_axilite port=weights1A bundle=control
// #pragma HLS interface s_axilite port=weights2A bundle=control
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// #pragma HLS interface s_axilite port=weights3A bundle=control
// #pragma HLS interface s_axilite port=weights4A bundle=control
// #endif
// #pragma HLS interface s_axilite port=output1A bundle=control
// #pragma HLS interface s_axilite port=output2A bundle=control
// #pragma HLS interface s_axilite port=input_other1A bundle=control
// #pragma HLS interface s_axilite port=input_other2A bundle=control
// #pragma HLS interface s_axilite port=input_1stA bundle=control
// #pragma HLS interface s_axilite port=biasA bundle=control
// #pragma HLS interface s_axilite port=scalar_conv_argsA bundle=control
// #if !XI_DISABLE_BN
// #pragma HLS interface s_axilite port=inp_norm_2 bundle=control
// #pragma HLS interface s_axilite port=inp_norm_3 bundle=control
// #endif
// #pragma HLS interface s_axilite port=istg_out1A bundle=control
// #pragma HLS interface s_axilite port=istg_out2A bundle=control
// #endif
//
//
// #ifndef __SDSOC
// #pragma HLS interface m_axi port=weights1B 		offset=slave bundle=gmem0B depth=576
// #pragma HLS interface m_axi port=weights2B 		offset=slave bundle=gmem1B depth=576
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// #pragma HLS interface m_axi port=weights3B		offset=slave bundle=gmem2B depth=576
// #pragma HLS interface m_axi port=weights4B 		offset=slave bundle=gmem3B depth=576
// #endif
// #pragma HLS interface m_axi port=output1B 		offset=slave bundle=gmem4B depth=4096
// #pragma HLS interface m_axi port=output2B		offset=slave bundle=gmem5B depth=4096
// #pragma HLS interface m_axi port=input_other1B 	offset=slave bundle=gmem6B depth=4096
// #pragma HLS interface m_axi port=input_other2B 	offset=slave bundle=gmem7B depth=4096
// #pragma HLS interface m_axi port=input_1stB 		offset=slave bundle=gmem8 depth=4096
// #pragma HLS interface m_axi port=biasB 			offset=slave bundle=gmem9B depth=64
// #if !XI_DISABLE_BN
// #pragma HLS interface m_axi port=inp_norm_2 	offset=slave bundle=gmem10 depth=4
// #pragma HLS interface m_axi port=inp_norm_3 	offset=slave bundle=gmem11 depth=4
// #endif
// #pragma HLS interface m_axi port=istg_out1B 		offset=slave bundle=gmem12B depth=4096
// #pragma HLS interface m_axi port=istg_out2B 		offset=slave bundle=gmem13B depth=4096
// #pragma HLS interface m_axi port=scalar_conv_argsB offset=slave bundle=gmem14B depth=128
//
// #pragma HLS interface s_axilite port=weights1B bundle=control
// #pragma HLS interface s_axilite port=weights2B bundle=control
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// #pragma HLS interface s_axilite port=weights3B bundle=control
// #pragma HLS interface s_axilite port=weights4B bundle=control
// #endif
// #pragma HLS interface s_axilite port=output1B bundle=control
// #pragma HLS interface s_axilite port=output2B bundle=control
// #pragma HLS interface s_axilite port=input_other1B bundle=control
// #pragma HLS interface s_axilite port=input_other2B bundle=control
// #pragma HLS interface s_axilite port=input_1stB bundle=control
// #pragma HLS interface s_axilite port=biasB bundle=control
// #pragma HLS interface s_axilite port=scalar_conv_argsB bundle=control
// #if !XI_DISABLE_BN
// #pragma HLS interface s_axilite port=inp_norm_2 bundle=control
// #pragma HLS interface s_axilite port=inp_norm_3 bundle=control
// #endif
// #pragma HLS interface s_axilite port=istg_out1B bundle=control
// #pragma HLS interface s_axilite port=istg_out2B bundle=control
// #pragma HLS interface s_axilite port=return bundle=control
// #endif
//
//
//
// #pragma HLS dataflow
//
//
// 	hls::stream< gmem_inputtype_layerx >  Stream1;
// 	hls::stream< gmem_inputtype_layerx >  Stream2;
//
// 	Convolution<XI_IN_W,XI_IN_H,XI_OUT_W,XI_OUT_H,XI_NUM_KERNELS,XI_FILTER_SIZE,XI_CONV_STRIDE,XI_POOL_STRIDE,XI_POOL_SIZE,XI_INPUT_PLANES,XI_NKPF>
// 	(
// 			weights1A,
// 			weights2A,
// #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// 			weights3A,
// 			weights4A,
// #endif
// 			output1A,
// #if !XI_SINGLE_IO_PORT_EN
// 			output2A,
// #endif
// 			Stream1,
// 			Stream2,
// 			input_other1A,
// #if !XI_SINGLE_IO_PORT_EN
// 			input_other2A,
// #endif
// 			inStream1,
// 			inStream2,
// 			input_1stA,
// 			biasA,
// #if !XI_DISABLE_BN
// 			inp_norm_2,inp_norm_3,
// #endif
// 			istg_out1A,
// #if !XI_SINGLE_IO_PORT_EN
// 			istg_out2A,
// #endif
// 			scalar_conv_argsA,
// 			ap_clk_div2A);
//
//
//
//
//
// 		Convolution<XI_IN_W,XI_IN_H,XI_OUT_W,XI_OUT_H,XI_NUM_KERNELS,XI_FILTER_SIZE,XI_CONV_STRIDE,XI_POOL_STRIDE,XI_POOL_SIZE,XI_INPUT_PLANES,XI_NKPF>
// 		(
// 				weights1B,
// 				weights2B,
// 	#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
// 				weights3B,
// 				weights4B,
// 	#endif
// 				output1B,
// 	#if !XI_SINGLE_IO_PORT_EN
// 				output2B,
// 	#endif
// 				outStream1,
// 				outStream2,
// 				input_other1B,
// 	#if !XI_SINGLE_IO_PORT_EN
// 				input_other2B,
// 	#endif
// 				Stream1,
// 				Stream2,
// 				input_1stB,
// 				biasB,
// 	#if !XI_DISABLE_BN
// 				inp_norm_2,inp_norm_3,
// 	#endif
// 				istg_out1B,
// 	#if !XI_SINGLE_IO_PORT_EN
// 				istg_out2B,
// 	#endif
// 				scalar_conv_argsB,
// 				ap_clk_div2A);
//
//
//
//
//
// #ifndef __SDSOC
// 	//return 0;
// #endif
// }

