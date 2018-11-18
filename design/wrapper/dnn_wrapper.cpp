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

#ifdef __SDSOC
//#include "xi_conv_config.h"
#include "pipeSystem.h"
#endif
#include "ap_int.h"

#if XI_64BIT_PORT_EN==1
#define KER_GMEM_INPUTTYPE		 unsigned long long int
#define KER_GMEM_INTYPE_OTHER    ap_uint<64>
#define KER_GMEM_WEIGHTTYPE      ap_uint<64>
#define KER_GMEM_BIASTYPE        unsigned long long int
#define KER_GMEM_OUTTYPE         ap_uint<64>
#define KER_GMEM_DECONVOUTTYPE   unsigned long long int
#define KER_SOFT_GMEM_TYPE       unsigned int
#define KER_GMEM_FC_INPUTTYPE	 ap_uint<128>
#define KER_GMEM_FC_OUTTYPE      ap_uint<128>
#else
#define KER_GMEM_INPUTTYPE		 unsigned long long int
#define KER_GMEM_INTYPE_OTHER    ap_uint<128>
#define KER_GMEM_WEIGHTTYPE      ap_uint<128>
#define KER_GMEM_BIASTYPE        unsigned long long int
#define KER_GMEM_OUTTYPE         ap_uint<128>
#define KER_GMEM_DECONVOUTTYPE   unsigned long long int
#define KER_SOFT_GMEM_TYPE       unsigned int
#define KER_GMEM_FC_INPUTTYPE	 ap_uint<128>
#define KER_GMEM_FC_OUTTYPE      ap_uint<128>
#endif

#define KER_GMEM_DECONVOUTTYPE   unsigned long long int

#if DECONV_FLOAT
#define KER_GMEM_DECONVINPTYPE   float
#else
#define KER_GMEM_DECONVINPTYPE   short
#endif

#define KER_GMEM_MAXPOOLTYPE   ap_uint<128>
#define KER_GMEM_POOLTYPE_WTS  ap_uint<128>

#define GMEM_RGBTYPE       unsigned long long int
#define GMEM_DECONVTYPE    unsigned long long int
#define GMEM_DISPLAYTYPE   unsigned long long int

typedef unsigned long long int gmem_inputtype_layer1;
typedef  ap_uint<128> gmem_outputtype;
typedef  ap_uint<128>  gmem_weighttype;
typedef ap_uint<128> gmem_inputtype_layerx;
typedef  unsigned long long int gmem_biastype;

int ConvlutionPipeWrapper(
    gmem_weighttype *IP36_weights1, 
    gmem_weighttype *IP36_weights2,
    gmem_weighttype *IP36_weights3,
    gmem_weighttype *IP36_weights4,
    gmem_outputtype *IP36_output1,
    gmem_outputtype *IP36_output2,
    gmem_inputtype_layerx *IP36_input_other1,	
    gmem_inputtype_layerx *IP36_input_other2,
    gmem_inputtype_layer1 *IP36_input_1st,
    gmem_biastype *IP36_bias,
    gmem_inputtype_layer1 *IP36_inp_norm_2, 
    gmem_inputtype_layer1 *IP36_inp_norm_3,
    gmem_inputtype_layer1 *IP36_istg_out1,
    gmem_inputtype_layer1 *IP36_istg_out2,

    gmem_weighttype *IP110A_weights1, 
    gmem_weighttype *IP110A_weights2,
    gmem_outputtype *IP110A_output1,
    gmem_outputtype *IP110A_output2,
    gmem_inputtype_layerx *IP110A_input_other1,	
    gmem_inputtype_layerx *IP110A_input_other2,
    gmem_inputtype_layer1 *IP110A_input_1st,
    gmem_biastype *IP110A_bias,
    gmem_inputtype_layer1 *IP110A_inp_norm_2,  
    gmem_inputtype_layer1 *IP110A_inp_norm_3,
    gmem_inputtype_layer1 *IP110A_istg_out1,
    gmem_inputtype_layer1 *IP110A_istg_out2,

    gmem_weighttype *IP110B_weights1, 
    gmem_weighttype *IP110B_weights2,
    gmem_outputtype *IP110B_output1,
    gmem_outputtype *IP110B_output2,
    gmem_inputtype_layerx *IP110B_input_other1,	
    gmem_inputtype_layerx *IP110B_input_other2,
    gmem_inputtype_layer1 *IP110B_input_1st,
    gmem_biastype *IP110B_bias,
    gmem_inputtype_layer1 *IP110B_inp_norm_2, 
    gmem_inputtype_layer1 *IP110B_inp_norm_3,
    gmem_inputtype_layer1 *IP110B_istg_out1,
    gmem_inputtype_layer1 *IP110B_istg_out2,




    GMEM_MAXPOOLTYPE* pool1_inMem1,
    GMEM_MAXPOOLTYPE* pool1_inMem2,
    GMEM_MAXPOOLTYPE* pool1_outMem1,
    GMEM_MAXPOOLTYPE* pool1_outMem2,

  
    GMEM_MAXPOOLTYPE* pool2_inMem1,
    GMEM_MAXPOOLTYPE* pool2_inMem2,
    GMEM_MAXPOOLTYPE* pool2_outMem1,
    GMEM_MAXPOOLTYPE* pool2_outMem2,
  
    int* argsConvIP36,
    int* argsConvIP110A,
    int* argsConvIP110B,
    int* argsPool1,
    int* argsPool2,
    int* argsStreamDivisor,
    int* argsStreamCombiner

        ){
#if __SDSOC
	#pragma SDS async(1)
#endif
    ConvPipeline
(
    IP36_weights1, 
    IP36_weights2,
    IP36_weights3,
    IP36_weights4,
    IP36_output1,
    IP36_output2,
    IP36_input_other1,	
    IP36_input_other2,
    IP36_input_1st,
    IP36_bias,
    IP36_inp_norm_2, 
    IP36_inp_norm_3,
    IP36_istg_out1,
    IP36_istg_out2,

    IP110A_weights1, 
    IP110A_weights2,
    IP110A_output1,
    IP110A_output2,
    IP110A_input_other1,	
    IP110A_input_other2,
    IP110A_input_1st,
    IP110A_bias,
    IP110A_inp_norm_2,  
    IP110A_inp_norm_3,
    IP110A_istg_out1,
    IP110A_istg_out2,

    IP110B_weights1, 
    IP110B_weights2,
    IP110B_output1,
    IP110B_output2,
    IP110B_input_other1,	
    IP110B_input_other2,
    IP110B_input_1st,
    IP110B_bias,
    IP110B_inp_norm_2, 
    IP110B_inp_norm_3,
    IP110B_istg_out1,
    IP110B_istg_out2,

     pool1_inMem1,
     pool1_inMem2,
     pool1_outMem1,
     pool1_outMem2,

  
     pool2_inMem1,
     pool2_inMem2,
     pool2_outMem1,
     pool2_outMem2,
  
     argsConvIP36,
     argsConvIP110A,
     argsConvIP110B,
     argsPool1,
     argsPool2,
     argsStreamDivisor,
     argsStreamCombiner
);
    return 0;
}


#if __CONV_ENABLE__==1
void XiConvolutionTop(
					KER_GMEM_WEIGHTTYPE *weights1, KER_GMEM_WEIGHTTYPE *weights2, 
#if (XI_KER_PROC==16 || (XI_64BIT_PORT_EN==1 && XI_KER_PROC==8))
					KER_GMEM_WEIGHTTYPE *weights3, KER_GMEM_WEIGHTTYPE *weights4,
#endif
					KER_GMEM_OUTTYPE *output1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_OUTTYPE *output2,
#endif
					KER_GMEM_INTYPE_OTHER *input_other1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_INTYPE_OTHER *input_other2,
#endif
					KER_GMEM_INPUTTYPE *input_1st, KER_GMEM_BIASTYPE *bias,
#if !XI_DISABLE_BN
					KER_GMEM_INPUTTYPE *input_norm2, KER_GMEM_INPUTTYPE *input_norm3,
#endif
					KER_GMEM_INPUTTYPE *istg_out1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_INPUTTYPE *istg_out2,
#endif
					int *scalar_conv_args
					);
					
int ConvolutionWrapper(
					KER_GMEM_WEIGHTTYPE *weights1, KER_GMEM_WEIGHTTYPE *weights2,
#if (XI_KER_PROC==16 || (XI_64BIT_PORT_EN==1 && XI_KER_PROC==8))
					KER_GMEM_WEIGHTTYPE *weights3, KER_GMEM_WEIGHTTYPE *weights4,
#endif
					KER_GMEM_OUTTYPE *output1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_OUTTYPE *output2,
#endif
					KER_GMEM_INTYPE_OTHER *input_other1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_INTYPE_OTHER *input_other2,
#endif
					KER_GMEM_INPUTTYPE *input_1st, KER_GMEM_BIASTYPE *bias,
#if !XI_DISABLE_BN
					KER_GMEM_INPUTTYPE *input_norm2, KER_GMEM_INPUTTYPE *input_norm3,
#endif
					KER_GMEM_INPUTTYPE *istg_out1,
#if !XI_SINGLE_IO_PORT_EN
					KER_GMEM_INPUTTYPE *istg_out2,
#endif
					int *scalar_conv_args
					)
{
#if __SDSOC
	#pragma SDS async(1)
#endif

	XiConvolutionTop(weights1, weights2,
#if (XI_KER_PROC==16 || (XI_64BIT_PORT_EN==1 && XI_KER_PROC==8))
			         weights3, weights4,
#endif
			         output1,
#if !XI_SINGLE_IO_PORT_EN
					 output2,
#endif
					 input_other1,
#if !XI_SINGLE_IO_PORT_EN
					 input_other2,
#endif
					 input_1st, bias,
#if !XI_DISABLE_BN
					 input_norm2, input_norm3,
#endif
					 istg_out1,
#if !XI_SINGLE_IO_PORT_EN
					 istg_out2,
#endif
					 scalar_conv_args);
					 
					 return 0;
}
#endif//CONV KERNEL

#if __POOL_ENABLE__==1
void PoolTop(
			KER_GMEM_MAXPOOLTYPE *in1, KER_GMEM_MAXPOOLTYPE *in2,
			KER_GMEM_MAXPOOLTYPE *out1, KER_GMEM_MAXPOOLTYPE *out2, 
			KER_GMEM_POOLTYPE_WTS *wts,
			int *scalar_pool_args
			);
			
int PoolWrapper(
				KER_GMEM_MAXPOOLTYPE *in1,KER_GMEM_MAXPOOLTYPE *in2,
				KER_GMEM_MAXPOOLTYPE *out1,KER_GMEM_MAXPOOLTYPE *out2,
				KER_GMEM_POOLTYPE_WTS *wts,
				int *scalar_pool_args)
{
#if __SDSOC
	#pragma SDS async(2)
#endif
   PoolTop(in1,in2,out1,out2, wts, scalar_pool_args);
   return 0;
}

#endif//POOL kernel

#if __DECONV_ENABLE__==1
void XiDeconvTop(KER_GMEM_DECONVINPTYPE* deconvIN, KER_GMEM_DECONVINPTYPE* deconvWT, KER_GMEM_DECONVINPTYPE* deconvBias, KER_GMEM_DECONVOUTTYPE* deconvIDout, int *scalar_deconv_args);

int DeconvWrapper(
				KER_GMEM_DECONVINPTYPE* deconvIN, KER_GMEM_DECONVINPTYPE* deconvWT, 
				KER_GMEM_DECONVINPTYPE* deconvBias, KER_GMEM_DECONVOUTTYPE* deconvIDout, int *scalar_deconv_args)
{
#if __SDSOC
	#pragma SDS async(3)
#endif
		XiDeconvTop(deconvIN, deconvWT, deconvBias, deconvIDout, scalar_deconv_args);
		return 0;
		
}

#endif//Deconv kernel
