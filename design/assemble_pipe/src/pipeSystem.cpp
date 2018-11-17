#include "../IP36/IP36.h"
#include "../IP110/IP110.h"
#include "../IP217/IP217.h"
#include "../include/xi_conv_config.h"
#include <hls_stream.h>
#include <ap_int.h>

#define WRITE_CHECKPOINT 1


#if WRITE_CHECKPOINT 
#include "verification.h"

FILE* checkpoint1;
FILE* checkpoint2;


gmem_outputtype outSol1[OUTPUT_DEPTH];
gmem_outputtype outSol2[OUTPUT_DEPTH];

#endif

void dividor(
    int* args,
    hls::stream< ap_uint<128> > & in_stream1,
    hls::stream< ap_uint<128> > & in_stream2,
    hls::stream< ap_uint<128> > & out_stream1Group0,
    hls::stream< ap_uint<128> > & out_stream2Group0,
    hls::stream< ap_uint<128> > & out_stream1Group1,
    hls::stream< ap_uint<128> > & out_stream2Group1
)
{
    int height=args[0];
    int width= args[1];
    int depthPackNumHalf=args[2];

    for(int j=0;j<height;j++)
    {
        for(int i=0;i<depthPackNumHalf;i++)
        {
            for(int k=0;k<width; k++)
            {
                #pragma HLS pipeline
                ap_uint<128> temp1,temp2;
                in_stream1>>temp1;
                in_stream2>>temp2;
                out_stream1Group0<<temp1;
                out_stream2Group0<<temp2;
            }
        }
        for(int i=0;i<depthPackNumHalf;i++)
        {
            for(int k=0;k<width; k++)
            {
                #pragma HLS pipeline
                ap_uint<128> temp1,temp2;
                in_stream1>>temp1;
                in_stream2>>temp2;
                out_stream1Group1<<temp1;
                out_stream2Group1<<temp2;
            }
        }
    }
}


void combiner(
    int* args,
    hls::stream< ap_uint<128> > & in_stream1,
    hls::stream< ap_uint<128> > & in_stream2,
    hls::stream< ap_uint<128> > & out_stream1Group0,
    hls::stream< ap_uint<128> > & out_stream2Group0,
    hls::stream< ap_uint<128> > & out_stream1Group1,
    hls::stream< ap_uint<128> > & out_stream2Group1
)
{
    int height=args[0];
    int width= args[1];
    int depthPackNumHalf=args[2];


    for(int j=0;j<height;j++)
    {
        for(int i=0;i<depthPackNumHalf;i++)
        {
            for(int k=0;k<width; k++)
            {
                #pragma HLS pipeline
                ap_uint<128> temp1,temp2;

                out_stream1Group0>>temp1;
                out_stream2Group0>>temp2;
                in_stream1<<temp1;
                in_stream2<<temp2;
            }
        }
        for(int i=0;i<depthPackNumHalf;i++)
        {
            for(int k=0;k<width; k++)
            {
                #pragma HLS pipeline
                ap_uint<128> temp1,temp2;
                out_stream1Group1>>temp1;
                out_stream2Group1>>temp2;
                in_stream1<<temp1;
                in_stream2<<temp2;

            }
        }
    }
}





void ConvPipeline
(
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
)
{
    #pragma HLS dataflow

    hls::stream< ap_uint<128> > streamStart1("streamStart1");
    hls::stream< ap_uint<128> > streamStart2("streamStart2");

    hls::stream< ap_uint<128> > stream0To1_1("stream0_1_1");
    hls::stream< ap_uint<128> > stream0To1_2("stream0_1_2");

    hls::stream< ap_uint<128> > stream1To2_1("stream1_2_1");
    hls::stream< ap_uint<128> > stream1To2_2("stream1_2_2");

    hls::stream< ap_uint<128> > stream1To2group0_1("stream1_2_g0_1");
    hls::stream< ap_uint<128> > stream1To2group1_1("stream1_2_g1_1");
    hls::stream< ap_uint<128> > stream1To2group0_2("stream1_2_g0_2");
    hls::stream< ap_uint<128> > stream1To2group1_2("stream1_2_g1_2");

    hls::stream< ap_uint<128> > stream2To3group0_1("stream2_3_g0_1");
    hls::stream< ap_uint<128> > stream2To3group1_1("stream2_3_g0_2");
    hls::stream< ap_uint<128> > stream2To3group0_2("stream2_3_g1_1");
    hls::stream< ap_uint<128> > stream2To3group1_2("stream2_3_g1_2");

    hls::stream< ap_uint<128> > stream2To3_1("stream2_3_1");
    hls::stream< ap_uint<128> > stream2To3_2("stream2_3_2");

    hls::stream< ap_uint<128> > streamEnd1("streamEnd1");
    hls::stream< ap_uint<128> > streamEnd2("streamEnd2");



    IP36::Convolution(
    IP36_weights1,
    IP36_weights2,
    IP36_weights3,
    IP36_weights4,
    IP36_output1,
    IP36_output2,
    stream0To1_1,
    stream0To1_2,
    IP36_input_other1,
    IP36_input_other2,
    streamStart1,
    streamStart2,
    IP36_input_1st,
    IP36_bias,
    IP36_inp_norm_2,
    IP36_inp_norm_3,
    IP36_istg_out1,
    IP36_istg_out2,
    argsConvIP36
    );
    
// char filename[200];


    IP217::PoolingLayerLineBuffer
    (
        pool1_inMem1,
        pool1_inMem2,
        stream0To1_1,
        stream0To1_2,
        pool1_outMem1,
        pool1_outMem2,
        stream1To2_1,
        stream1To2_2,
        argsPool1
    );
    

  

    dividor
    (
        argsStreamDivisor,
        stream1To2_1,
        stream1To2_2,
        stream1To2group0_1,
        stream1To2group0_2,
        stream1To2group1_1,
        stream1To2group1_2
    );

// puts("divisor done");
    IP110::Convolution
    (
    IP110A_weights1,
    IP110A_weights2,
    IP110A_output1,
    IP110A_output2,
    stream2To3group0_1,
    stream2To3group0_2,
    IP110A_input_other1,
    IP110A_input_other2,
    stream1To2group0_1,
    stream1To2group0_2,
    IP110A_input_1st,
    IP110A_bias,
    IP110A_inp_norm_2,
    IP110A_inp_norm_3,
    IP110A_istg_out1,
    IP110A_istg_out2,
    argsConvIP110A
    );
    
    IP110::Convolution
    (
    IP110B_weights1,
    IP110B_weights2,
    IP110B_output1,
    IP110B_output2,
    stream2To3group1_1,
    stream2To3group1_2,
    IP110B_input_other1,
    IP110B_input_other2,
    stream1To2group1_1,
    stream1To2group1_2,
    IP110B_input_1st,
    IP110B_bias,
    IP110B_inp_norm_2,
    IP110B_inp_norm_3,
    IP110B_istg_out1,
    IP110B_istg_out2,
    argsConvIP110B
    );
    
    combiner
    (
        argsStreamCombiner,
        stream2To3_1,
        stream2To3_2,
        stream2To3group0_1,
        stream2To3group0_2,
        stream2To3group1_1,
        stream2To3group1_2
    );

    IP217::PoolingLayerLineBuffer
    (
        pool2_inMem1,
        pool2_inMem2,
        stream2To3_1,
        stream2To3_2,
        pool2_outMem1,
        pool2_outMem2,
        streamEnd1,
        streamEnd2,
        argsPool2
    );  


}

