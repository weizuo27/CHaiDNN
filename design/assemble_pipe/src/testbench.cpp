#include <ap_int.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <hls_stream.h>
#include <time.h>
#include "../include/xi_conv_desc.h"
#include "../include/xi_conv_config.h"
#include "../include/maxpool_config.h"
#include "testbench_defines.h"









gmem_weighttype weights1[8][WEIGHT_DEPTH]={};
gmem_weighttype weights2[8][WEIGHT_DEPTH]={};
gmem_weighttype weights3[8][WEIGHT_DEPTH]={};
gmem_weighttype weights4[8][WEIGHT_DEPTH]={};



gmem_inputtype_layerx input1[7][INPUT_DEPTH]={};
gmem_inputtype_layerx input2[7][INPUT_DEPTH]={};






gmem_inputtype_layer1 input_layer1[INPUT_DEPTH]={};


gmem_inputtype_layer1 inp_norm_2[INPUT_DEPTH]={};
gmem_inputtype_layer1 inp_norm_3[INPUT_DEPTH]={};



gmem_biastype bias[7][BIAS_DEPTH]={};






gmem_inputtype_layer1 istg_out1[OUTPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2[OUTPUT_DEPTH]={};



gmem_inputtype_layer1 istg_out1_gold[INPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2_gold[INPUT_DEPTH]={};

gmem_outputtype output1[7][OUTPUT_DEPTH]={};
gmem_outputtype output2[7][OUTPUT_DEPTH]={};


gmem_outputtype outputSol1[7][OUTPUT_DEPTH]={};
gmem_outputtype outputSol2[7][OUTPUT_DEPTH]={};


gmem_inputtype_layerx wts[100];



void dividor(int height, int width, int depthPackNumHalf,
    hls::stream< ap_uint<128> > & in_stream1,
    hls::stream< ap_uint<128> > & in_stream2,
    hls::stream< ap_uint<128> > & out_stream1Group0,
    hls::stream< ap_uint<128> > & out_stream2Group0,
    hls::stream< ap_uint<128> > & out_stream1Group1,
    hls::stream< ap_uint<128> > & out_stream2Group1
)
{
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


void combiner(int height, int width, int depthPackNumHalf,
    hls::stream< ap_uint<128> > & in_stream1,
    hls::stream< ap_uint<128> > & in_stream2,
    hls::stream< ap_uint<128> > & out_stream1Group0,
    hls::stream< ap_uint<128> > & out_stream2Group0,
    hls::stream< ap_uint<128> > & out_stream1Group1,
    hls::stream< ap_uint<128> > & out_stream2Group1
)
{
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


#define __SDSVHLS__
int main()
{
    clock_t startTime = clock();
    int scalaArgs[8][2][128]={};
    int scalarPoolArgs[32]={};

    int layerId=0;




    
    
    
    




//args loadign region

    for(int i=0;i<8;i++)
    {
        
        load_args_conv(i,0,scalaArgs[i][0]); 
        load_args_conv(i,1,scalaArgs[i][1]);        
        load_args_pool(i,0,scalaArgs[i][0]); 
        load_args_pool(i,1,scalaArgs[i][1]);  
            
    }


    
  


    
    


    






    //declaration of streaming system
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

    hls::stream< ap_uint<128> > stream3To4_1("stream3_4_1");
    hls::stream< ap_uint<128> > stream3To4_2("stream3_4_2");

    hls::stream< ap_uint<128> > stream4To5_1("stream4_5_1");
    hls::stream< ap_uint<128> > stream4To5_2("stream4_5_2");


    hls::stream< ap_uint<128> > stream4To5group0_1("stream4_5_g0_1");
    hls::stream< ap_uint<128> > stream4To5group1_1("stream4_5_g0_2");
    hls::stream< ap_uint<128> > stream4To5group0_2("stream4_5_g1_1");
    hls::stream< ap_uint<128> > stream4To5group1_2("stream4_5_g1_2");

    hls::stream< ap_uint<128> > stream5To6group0_1("stream5_6_g0_1");
    hls::stream< ap_uint<128> > stream5To6group1_1("stream5_6_g0_2");
    hls::stream< ap_uint<128> > stream5To6group0_2("stream5_6_g1_1");
    hls::stream< ap_uint<128> > stream5To6group1_2("stream5_6_g1_2");

    hls::stream< ap_uint<128> > stream6To7group0_1("stream6_7_g0_1");
    hls::stream< ap_uint<128> > stream6To7group1_1("stream6_7_g0_2");
    hls::stream< ap_uint<128> > stream6To7group0_2("stream6_7_g1_1");
    hls::stream< ap_uint<128> > stream6To7group1_2("stream6_7_g1_2");

    hls::stream< ap_uint<128> > stream6To7_1("stream6_7_1");
    hls::stream< ap_uint<128> > stream6To7_2("stream6_7_2");


    hls::stream< ap_uint<128> > streamEnd1("streamEnd1");
    hls::stream< ap_uint<128> > streamEnd2("streamEnd2");
    

    //modify stream option





    
    // layerId=1;
    

    // PoolingLayerLineBuffer(
    // output1[0],
    // output2[0],
    // stream0To1_1,
    // stream0To1_2,
    // output1[layerId],
    // output2[layerId],
    // streamEnd1,
    // streamEnd2,
    // scalaArgs[layerId][0]
    // );



    layerId=2;
    load_data(
    layerId, 
    weights1[layerId],
    weights2[layerId], 
    weights3[layerId],
    weights4[layerId], 
    input1[layerId], 
    input2[layerId],
    input_layer1,
    bias[layerId],
    inp_norm_2, 
    inp_norm_3,
    outputSol1[layerId],
    outputSol2[layerId],
    istg_out1,istg_out2);

    // short out_h       	= scalar_args[2]; 
	// short out_w      	= scalar_args[3];
	// short n_planes    	= scalar_args[4]; 


   

    memToStream(
        27, 
        27,
        6,
        stream1To2_1,
        input1[layerId],
        0
        );

    memToStream(
        27, 
        27,
        6,
        stream1To2_2,
        input2[layerId],
        0
        );


    dividor(        
        27, 
        27,
        3,
        stream1To2_1,
        stream1To2_2,
        stream1To2group0_1,
        stream1To2group0_2,
        stream1To2group1_1,
        stream1To2group1_2
        );
    




    


    // int depth=96;
    // int Pixel=27*27;
    // for(int j=0;j<depth/16;j++)
    // for(int i=0;i<Pixel;i++)
    // {
    //     unsigned long int addr=j*Pixel+i;
    //     unsigned long int Val1=i;
    //     unsigned long int Val2=j;

    //     input1[layerId][addr].range(63,0)=Val1;
    //     input1[layerId][addr].range(127,64)=Val2;


    //     input2[layerId][addr].range(63,0)=Val1;
    //     input2[layerId][addr].range(127,64)=Val2;

    // }

    scalaArgs[layerId][0][126]=1;
    scalaArgs[layerId][0][127]=0;
    scalaArgs[layerId][1][126]=1;
    scalaArgs[layerId][1][127]=0;

    Convolution(
    weights1[layerId],
    weights2[layerId],
    #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
    weights3[layerId],
    weights4[layerId],
    #endif
    output1[layerId],
    output2[layerId],
    stream2To3_1,
    stream2To3_2,
    input1[layerId],
    input2[layerId],
    stream1To2group0_1,
    stream1To2group0_2,
    input_layer1,
    bias[layerId],
    inp_norm_2,
    inp_norm_3,
    istg_out1,
    istg_out2,
    scalaArgs[2][0],
    0
    );




    Convolution(
    weights1[layerId],
    weights2[layerId],
    #if (XI_KER_PROC==16 || (XI_WTS_PORT_64BIT_EN==1 && XI_KER_PROC==8) )
    weights3[layerId],
    weights4[layerId],
    #endif
    output1[layerId],
    output2[layerId],
    stream2To3_1,
    stream2To3_2,
    input1[layerId],
    input2[layerId],
    stream1To2group1_1,
    stream1To2group1_2,
    input_layer1,
    bias[layerId],
    inp_norm_2,
    inp_norm_3,
    istg_out1,
    istg_out2,
    scalaArgs[2][1],
    0
    );


    char filename[200];
    sprintf(filename,"/home/xliu79/Research/2018Fall/debugData/AlexConv_%02d",layerId);
    load_answer(filename,outputSol1[layerId],outputSol2[layerId]);


    for(int i=0;i<18150;i++)
    {
        if(outputSol1[layerId][i]!=output1[layerId][i]) 
        {
            printf("Addr[%8d] %08x %08x %08x %08x\n", i,
            (unsigned int) outputSol1[layerId][i].range(127,96),
            (unsigned int) outputSol1[layerId][i].range(95,64),
            (unsigned int) outputSol1[layerId][i].range(63,32),
            (unsigned int) outputSol1[layerId][i].range(31,0));
        
            printf("Addr[%8d] %08x %08x %08x %08x\n", i,
            (unsigned int) output1[layerId][i].range(127,96),
            (unsigned int) output1[layerId][i].range(95,64),
            (unsigned int) output1[layerId][i].range(63,32),
            (unsigned int) output1[layerId][i].range(31,0));
        }
    }


    if(!memcmp(outputSol1[layerId],output1[layerId],sizeof(gmem_outputtype)*INPUT_DEPTH) )
    puts("output1 same!");
    else
    puts("output1_differ!");

    if(!memcmp(outputSol2[layerId],output2[layerId],sizeof(gmem_outputtype)*INPUT_DEPTH))
    puts("output2 same!");
    else 
    puts("output2_differ!");



    
    
// if(layerId!=4 && layerId!=0 )
// {

//     load_args_conv(layerId,1,scala_args);
//     printf("%d\n", scala_args[110]);

//     XiConvolutionTop(weights1,weights2, weights3, weights4, 
//     output1,output2,
//     input_other1, input_other2,
//     input_layer1,
//     bias,
//     inp_norm_2, inp_norm_3,
//     istg_out1,istg_out2, 
//     scala_args
    
//     #ifdef __SDSVHLS__
//     ,0
//     #endif
//     );
// }



#if GEN_SOL
 
    //save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer1",output3,output4);
#else
    // char filename[200];
    // sprintf(filename,"/home/xliu79/Research/2018Fall/debugData/AlexConv_%02dto%02d",layerId,layerId+1);
    // save_answer(filename,outputSol1[layerId+1],outputSol2[layerId+1]);



//   save_answer("/home/xliu79/Research/2018Fall/debugData/dbg_sim_layer0",output1,output2);
//  load_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/dbg_sol_layer0",output1_sol,output2_sol);







#endif



clock_t endTime = clock();

clock_t clockTicksTaken = endTime - startTime;
double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
printf("output time: %f\n", timeInSeconds);
}
