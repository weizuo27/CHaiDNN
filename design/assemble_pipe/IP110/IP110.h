#include "IP110_desc.h"	

namespace IP110{    

    void Convolution(                     
			gmem_weighttype *weights1, 
			gmem_weighttype *weights2,
			gmem_outputtype *output1,
			gmem_outputtype *output2,
			hls::stream< gmem_outputtype> &outStream1,
			hls::stream< gmem_outputtype> &outStream2,
			gmem_inputtype_layerx *input_other1,	
			gmem_inputtype_layerx *input_other2,
			hls::stream< gmem_inputtype_layerx > &inStream1,
			hls::stream< gmem_inputtype_layerx > &inStream2,
			gmem_inputtype_layer1 *input_1st,
			gmem_biastype *bias,
			gmem_inputtype_layer1 *inp_norm_2, 
			gmem_inputtype_layer1 *inp_norm_3,
			gmem_inputtype_layer1 *istg_out1,
			gmem_inputtype_layer1 *istg_out2,
			int *scalar_conv_args);
}