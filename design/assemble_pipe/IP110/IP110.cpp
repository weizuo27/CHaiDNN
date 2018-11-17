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

#include "IP110_defines.h"
#include "IP110_desc.h"
typedef ap_int<27> dsp27_t;
#if XI_DP_ENABLE
#include <dsp_builtins.h>
#endif
#include "IP110.hpp"

namespace IP110{

	void Convolution(
			gmem_weighttype *weights1, 
			gmem_weighttype *weights2,
	#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
			gmem_weighttype *weights3,
			gmem_weighttype *weights4,
	#endif
			gmem_outputtype *output1,
	#if !XI_SINGLE_IO_PORT_EN
			gmem_outputtype *output2,
	#endif
			hls::stream< gmem_outputtype> &outStream1,
			hls::stream< gmem_outputtype> &outStream2,
			gmem_inputtype_layerx *input_other1,
	#if !XI_SINGLE_IO_PORT_EN
			gmem_inputtype_layerx *input_other2,
	#endif
			hls::stream< gmem_inputtype_layerx > &inStream1,
			hls::stream< gmem_inputtype_layerx > &inStream2,
			gmem_inputtype_layer1 *input_1st,
			gmem_biastype *bias,
	#if !XI_DISABLE_BN
			gmem_inputtype_layer1 *inp_norm_2, gmem_inputtype_layer1 *inp_norm_3,
	#endif
			gmem_inputtype_layer1 *istg_out1,
	#if !XI_SINGLE_IO_PORT_EN
			gmem_inputtype_layer1 *istg_out2,
	#endif
			int *scalar_conv_args) {
	#pragma HLS INLINE OFF
		bool ap_clk_div2=0;
		input_struct input_desc;
		output_struct output_desc;
		weight_struct weight_desc;
		conv_struct conv_desc;
		group_conv_struct group_desc;
		LoadDesc_ffa(scalar_conv_args, input_desc, output_desc, weight_desc,conv_desc, group_desc);
if(conv_desc.noOperateFlag) return;
		// #if DBG_INFO
		// 	printDesc(conv_desc,input_desc,output_desc,weight_desc,group_desc,stdout);
		// #endif

		ConvLayer_fa<0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0>(
				input_desc, weight_desc, output_desc, conv_desc, group_desc,
				weights1 + weight_desc.weight_offset,
				weights2 + weight_desc.weight_offset,

				/*..continue..*/
	#if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
				weights3 + weight_desc.weight_offset,weights4 + weight_desc.weight_offset,
	#endif
				input_1st, input_other1,
				inStream1,
				inStream2,
	#if !XI_SINGLE_IO_PORT_EN
				input_other2,
	#endif
				/*..continue..*/bias + output_desc.bias_offset,
				output1 + output_desc.out_offset,
	#if !XI_SINGLE_IO_PORT_EN
				output2 + output_desc.out_offset,
	#endif
				outStream1,
				outStream2,
				/*..continue..*/
	#if !XI_DISABLE_BN
				inp_norm_2, inp_norm_3,
	#endif
				istg_out1,
	#if !XI_SINGLE_IO_PORT_EN
				istg_out2,
	#endif
				ap_clk_div2);

	}

}