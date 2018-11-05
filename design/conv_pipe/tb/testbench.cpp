#include <ap_int.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <hls_stream.h>
#include <time.h>

#include "../include/xi_conv_config.h"
#include "../include/xi_conv_desc.h"
#include "testbench_defines.h"






#define GEN_SOL 0
#define DBG_INFO 0





gmem_weighttype weights1[WEIGHT_DEPTH]={};
gmem_weighttype weights2[WEIGHT_DEPTH]={};
gmem_weighttype weights3[WEIGHT_DEPTH]={};
gmem_weighttype weights4[WEIGHT_DEPTH]={};
gmem_inputtype_layerx input_other1[INPUT_DEPTH]={};
gmem_inputtype_layerx input_other2[INPUT_DEPTH]={};
gmem_inputtype_layer1 input_layer1[INPUT_DEPTH]={};
gmem_inputtype_layer1 inp_norm_2[INPUT_DEPTH]={};
gmem_inputtype_layer1 inp_norm_3[INPUT_DEPTH]={};
gmem_biastype bias[BIAS_DEPTH]={};

gmem_inputtype_layer1 istg_out1[OUTPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2[OUTPUT_DEPTH]={};



gmem_inputtype_layer1 istg_out1_gold[INPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2_gold[INPUT_DEPTH]={};

gmem_outputtype output1[OUTPUT_DEPTH]={};
gmem_outputtype output2[OUTPUT_DEPTH]={};

gmem_outputtype output1_sol[OUTPUT_DEPTH]={};
gmem_outputtype output2_sol[OUTPUT_DEPTH]={};

int main()
{
    clock_t startTime = clock();
    int scala_args[128]={};

    int layerId=4;

    puts("loadDesc");
    load_data(layerId, weights1,weights2, weights3, weights4, 
    input_other1, input_other2,
    input_layer1,
    bias,
    inp_norm_2, inp_norm_3,
    output1_sol,output2_sol,
    istg_out1,istg_out2);

    // int depth=4;
    // int Pixel=227*227;
    // for(int j=0;j<depth/4;j++)
    // for(int i=0;i<Pixel;i++)
    // {
    //     unsigned long int Val=j*65536+i;
    //     int addr= (j/4)*Pixel+i;
    //     switch( j%4)
    //     {
    //         case 0: input_other1[addr].range(63,0)=Val;break;
    //         case 1: input_other1[addr].range(127,64)=Val;break;
    //         case 2: input_other2[addr].range(63,0)=Val;break;
    //         default: input_other2[addr].range(127,64)=Val;break;
    //     }
    // }

    // int depth=4;
    // int Pixel=227*227;
    // for(int i=0;i<Pixel;i++)
    // {
    //     input_layer1[i]=i;    
    // }



    // for(int i=0;i<WEIGHT_DEPTH;i++)
    // {
    //     weights1[i]=i*4+1;
    //     weights2[i]=i*4+2;
    //     weights3[i]=i*4+3;
    //     weights4[i]=i*4+4;
    // }




    // for(int i=0;i<BIAS_DEPTH;i++)
    // {
    //     bias[i]=i;
    // }

    load_args_conv(layerId,0,scala_args);
    printf("layer [%d %d]\n", layerId,0);


    XiConvolutionTop(weights1,weights2, weights3, weights4, 
    output1,output2,
    input_other1, input_other2,
    input_layer1,
    bias,
    inp_norm_2, inp_norm_3,
    istg_out1,istg_out2, 
    scala_args    
    #ifdef __SDSVHLS__
    ,0
    #endif
    );

    
if(layerId!=4 && layerId!=0 )
{

    load_args_conv(layerId,1,scala_args);
    printf("%d\n", scala_args[110]);

    XiConvolutionTop(weights1,weights2, weights3, weights4, 
    output1,output2,
    input_other1, input_other2,
    input_layer1,
    bias,
    inp_norm_2, inp_norm_3,
    istg_out1,istg_out2, 
    scala_args
    
    #ifdef __SDSVHLS__
    ,0
    #endif
    );
}

#if DBG_INFO
    for(int i=0;i<OUTPUT_DEPTH;i++)
    {
        printf("Output1 addr:%d, value: %x\t%x\n",i,(int) output1[i].range(63,0), (int)output1[i].range(127,64));
        printf("Output2 addr:%d, value: %x\t%x\n",i,(int) output2[i].range(63,0), (int)output2[i].range(127,64));
        printf("Output3 addr:%d, value: %x\t%x\n",i,(int) output3[i].range(63,0), (int)output3[i].range(127,64));
        printf("Output4 addr:%d, value: %x\t%x\n",i,(int) output4[i].range(63,0), (int)output4[i].range(127,64));
    }
    for(int i=0;i<INPUT_DEPTH;i++)
    {
        printf("INput1 addr:%d, value: %x\t%x\n",i,(int) input_other1[i].range(63,0), (int)input_other1[i].range(127,64));
        printf("INput2 addr:%d, value: %x\t%x\n",i,(int) input_other2[i].range(63,0), (int)input_other2[i].range(127,64));
    }
   for(int i=0;i<WEIGHT_DEPTH;i++)
   {
       printf("weights1 addr:%d, value: %x\t%x\n",i,(int) weights1[i].range(63,0), (int)weights1[i].range(127,64));
       printf("weights2 addr:%d, value: %x\t%x\n",i,(int) weights2[i].range(63,0), (int)weights2[i].range(127,64));
       printf("weights3 addr:%d, value: %x\t%x\n",i,(int) weights3[i].range(63,0), (int)weights3[i].range(127,64));
       printf("weights4 addr:%d, value: %x\t%x\n",i,(int) weights4[i].range(63,0), (int)weights4[i].range(127,64));
   }
#endif


#if GEN_SOL
    save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/dbg_sol_layer0_1",output1,output2);
    // save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer1",output3,output4);
#else
    char answerpath[200];
    sprintf(answerpath,"/home/xliu79/Research/2018Fall/debugData/dbg_sol_layer%d", layerId);
    load_answer(answerpath,output1_sol,output2_sol);



    // for(int i=0;i<11664;i++)
    // {
    //     if(output1_sol[i]!=output1[i]) 
    //     {
    //         printf("Addr[%8d] %08x %08x %08x %08x\n", i,
    //         (unsigned int) output1_sol[i].range(127,96),
    //         (unsigned int) output1_sol[i].range(95,64),
    //         (unsigned int) output1_sol[i].range(63,32),
    //         (unsigned int) output1_sol[i].range(31,0));
        
    //         printf("Addr[%8d] %08x %08x %08x %08x\n", i,
    //         (unsigned int) output1[i].range(127,96),
    //         (unsigned int) output1[i].range(95,64),
    //         (unsigned int) output1[i].range(63,32),
    //         (unsigned int) output1[i].range(31,0));
    //     }
    // }
    if(!memcmp(output1_sol,output1,sizeof(gmem_outputtype)*OUTPUT_DEPTH) )
    puts("output1 same!");
    else
    puts("output1_differ!");
    
    if(!memcmp(output2_sol,output2,sizeof(gmem_outputtype)*OUTPUT_DEPTH))
    puts("output2 same!");
    else puts("output2_differ!");

#endif



clock_t endTime = clock();

clock_t clockTicksTaken = endTime - startTime;
double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
printf("output time: %f\n", timeInSeconds);
}
