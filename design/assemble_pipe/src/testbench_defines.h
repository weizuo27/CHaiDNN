


#ifndef _TESTBENCH_DEFINE_H
#define _TESTBENCH_DEFINE_H

#include "../include/xi_conv_desc.h"
#include "../include/maxpool_config.h"



#define PLANE_PACK_SIZE 16 

#define WEIGHT_BYTES 221184
#define INPUT_BYTES 1605632
#define OUTPUT_BYTES 1605632
#define BIAS_BYTES 12288



#define WEIGHT_DEPTH WEIGHT_BYTES/PLANE_PACK_SIZE
#define INPUT_DEPTH INPUT_BYTES/PLANE_PACK_SIZE
#define OUTPUT_DEPTH OUTPUT_BYTES/PLANE_PACK_SIZE
#define BIAS_DEPTH BIAS_BYTES/PLANE_PACK_SIZE


#define INBYTES 290400
#define INDEPTH INBYTES/16

#define DATA_FILE_PATH "/home/xliu79/Research/2018Fall/model_args/AlexOneRow/"

void load_args_pool
(
    int layerIdx,
    int groupIdx,
    int *scalar_args
);

void load_pool_input
(
	int layerId,
	int groupId,
	GMEM_MAXPOOLTYPE in[INDEPTH],
	GMEM_MAXPOOLTYPE out[INDEPTH]
);

void save_answer(
    char* filename,
    gmem_outputtype* output1_gold,
    gmem_outputtype* output2_gold
);

void load_answer
(
    char* filename,
    gmem_outputtype* output1_gold,
    gmem_outputtype* output2_gold
);

void load_args_conv
(
    int layerIdx,
    int groupIdx,
    int *scalar_args
);

void load_data
(
    int layerIdx,
    gmem_weighttype* weights1,
    gmem_weighttype* weights2,
    gmem_weighttype* weights3,
    gmem_weighttype* weights4,
    gmem_inputtype_layerx* input_other1,
    gmem_inputtype_layerx* input_other2,
    gmem_inputtype_layer1* input_layer1,
    gmem_biastype* bias,
    gmem_inputtype_layer1* inp_norm_2,
    gmem_inputtype_layer1* inp_norm_3,
    gmem_outputtype* output1,
    gmem_outputtype* output2,
    gmem_inputtype_layer1* istg_out1,
    gmem_inputtype_layer1* istg_out2
);


void streamToMem(
    int height,
    int width,
    int depthBy16,
    hls::stream< ap_uint<128> > &stream,
    ap_uint<128>* mem,
    int offset
);


void memToStream(
    int height,
    int width,
    int depthBy16,
    hls::stream< ap_uint<128> > &stream,
    ap_uint<128>* mem,
    int offset
);


#endif //_TESTBENCH_DEFINE_H 