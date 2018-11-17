#include <ap_int.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <hls_stream.h>
#include <time.h>
#include "../include/xi_conv_desc.h"
#include "../include/maxpool_config.h"
#include "../IP36/IP36.h"
#include "verification.h"
#include "pipeSystem.h"








gmem_weighttype weights1[8][WEIGHT_DEPTH]={};
gmem_weighttype weights2[8][WEIGHT_DEPTH]={};
gmem_weighttype weights3[8][WEIGHT_DEPTH]={};
gmem_weighttype weights4[8][WEIGHT_DEPTH]={};



gmem_inputtype_layerx input1[8][INPUT_DEPTH]={};
gmem_inputtype_layerx input2[8][INPUT_DEPTH]={};






gmem_inputtype_layer1 input_layer1[8][INPUT_DEPTH]={};


gmem_inputtype_layer1 inp_norm_2[8][INPUT_DEPTH]={};
gmem_inputtype_layer1 inp_norm_3[8][INPUT_DEPTH]={};



gmem_biastype bias[8][BIAS_DEPTH]={};






gmem_inputtype_layer1 istg_out1[8][OUTPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2[8][OUTPUT_DEPTH]={};



gmem_inputtype_layer1 istg_out1_gold[INPUT_DEPTH]={};
gmem_inputtype_layer1 istg_out2_gold[INPUT_DEPTH]={};

gmem_outputtype output1[8][OUTPUT_DEPTH]={};
gmem_outputtype output2[8][OUTPUT_DEPTH]={};


gmem_outputtype outputSol1[8][OUTPUT_DEPTH]={};
gmem_outputtype outputSol2[8][OUTPUT_DEPTH]={};


gmem_inputtype_layerx wts[100];





void setDivisorArgs
(
    int* divisorArgs,
    int* groupArgs, 
    int opFlag
)
{
    if(opFlag)
    {
        divisorArgs[0]=groupArgs[0];
        divisorArgs[1]=groupArgs[1];
        divisorArgs[2]=groupArgs[61]/16;  
    }
    else
    {
        divisorArgs[0]=0;
        divisorArgs[1]=0;
        divisorArgs[2]=0;
    }
    printf("divider argumetns: %d %d %d\n", divisorArgs[0], divisorArgs[1], divisorArgs[2]);
    
}

void setCombinerArgs
(
    int* combinerArgs,
    int* groupArgs, // the group that goes into it
    int opFlag
)
{
    if(opFlag)
    {
        combinerArgs[0]=groupArgs[2];
        combinerArgs[1]=groupArgs[3];
        combinerArgs[2]=groupArgs[62]/16;  
    }
    else
    {
        combinerArgs[0]=0;
        combinerArgs[1]=0;
        combinerArgs[2]=0;
    }
    printf("combiner argumetns: %d %d %d\n", combinerArgs[0], combinerArgs[1], combinerArgs[2]);
}


void setConvArgs
(
    int* convArgs,
    int opFlag,
    int streamInFlag,
    int streamOutFlag
)
{
    if(opFlag) 
    {
        printf("Conv Operates\n");
        convArgs[125]=0;
    }
    else 
    {
        printf("Conv No Operates\n");
        convArgs[125]=1;
    }

    
    convArgs[126]=streamInFlag;
    convArgs[127]=streamOutFlag;
}

void setPoolArgs
(
    int* poolArgs,
    int* convArgs, // bypass number decided by its previous layer
    int opFlag,
    int byPassFlag,
    int streamInFlag,
    int streamOutFlag
)
{
    printf("Opflags: %d %d %d %d\n",opFlag, byPassFlag, streamInFlag, streamOutFlag);
    if(opFlag)
    {
        poolArgs[19]=streamInFlag;
        poolArgs[20]=streamOutFlag;
        if(byPassFlag)
        {
        int inHeight=convArgs[2];
        int inWidth=convArgs[3];
        int depth=convArgs[62];
        poolArgs[19]=0;
        poolArgs[20]=0;
        load_args_pool_bypass(inHeight,inWidth,depth,poolArgs);
        }
    }
    else
    {
        load_args_pool_bypass(0,0,0,poolArgs);
    }

    printf("Scala Pool:%d %d %d %d\n",poolArgs[19], poolArgs[20],poolArgs[21], poolArgs[22]);
}




#define __SDSVHLS__
int main()
{
    clock_t startTime = clock();
    int scalaArgs[8][2][128]={};
    int scalarPoolArgs[8][32]={};

   

    for(int i=0;i<8;i++)
    {     
        load_args_conv(i,0,scalaArgs[i][0]); 
        load_args_conv(i,1,scalaArgs[i][1]);        
        load_args_pool(i,0,scalarPoolArgs[i]); 
        load_args_pool(i,1,scalarPoolArgs[i]); 
                  
    }

    char filename[200];

    int IP36_layerId=0;
    int IP110_layerId=6;
    int output_layerId=7;


    load_data(
    IP36_layerId, 
    weights1[IP36_layerId],
    weights2[IP36_layerId], 
    weights3[IP36_layerId],
    weights4[IP36_layerId], 
    input1[IP36_layerId], 
    input2[IP36_layerId],
    input_layer1[IP36_layerId],
    bias[IP36_layerId],
    inp_norm_2[IP36_layerId], 
    inp_norm_3[IP36_layerId],
    outputSol1[IP36_layerId],
    outputSol2[IP36_layerId],
    istg_out1[IP36_layerId],
    istg_out2[IP36_layerId]);

    load_data(IP110_layerId, 
    weights1[IP110_layerId],
    weights2[IP110_layerId], 
    weights3[IP110_layerId],
    weights4[IP110_layerId], 
    input1[IP110_layerId], 
    input2[IP110_layerId],
    input_layer1[IP110_layerId],
    bias[IP110_layerId],
    inp_norm_2[IP110_layerId], 
    inp_norm_3[IP110_layerId],
    outputSol1[IP110_layerId],
    outputSol2[IP110_layerId],
    istg_out1[IP110_layerId],
    istg_out2[IP110_layerId]);

        
    sprintf(filename,"/home/xliu79/Research/2018Fall/debugData/Pipe_%02d",5);
    load_answer(filename,input1[IP110_layerId],input2[IP110_layerId]);        
    
    

    int* argsIP36=scalaArgs[IP36_layerId][0];
    int* argsIP110A=scalaArgs[IP110_layerId][0];
    int* argsIP110B=scalaArgs[IP110_layerId][1];
    int* argsPool1=scalarPoolArgs[IP36_layerId+1];
    int* argsPool2=scalarPoolArgs[IP110_layerId+1];
    int argsComb[3];
    int argsDiv[3];

//prepare args for IP36

#define STREAM_ON  1
#define STREAM_OFF 0

#define CONVOP_ON 0
#define CONVOP_OFF 1

#define POOLOP_ON 1
#define POOLOP_OFF 0

#define BYPASS_ON 1
#define BYPASS_OFF 0

    setConvArgs(argsIP36, 0,  0, 1);

    setPoolArgs(argsPool1,argsIP36, 0 ,1, 0,  0);


    setDivisorArgs(argsDiv,argsIP110A,0);

    setConvArgs(argsIP110A,1,0,1);
    setConvArgs(argsIP110B,1,0,1);

    setCombinerArgs(argsComb,argsIP110A,1);
    setPoolArgs(argsPool2,argsIP110A,1,0,1,0);

    ConvPipeline
    (
        weights1[IP36_layerId],
        weights2[IP36_layerId], 
        weights3[IP36_layerId],
        weights4[IP36_layerId], 
        output1[IP36_layerId],
        output2[IP36_layerId],
        input1[IP36_layerId], 
        input2[IP36_layerId],
        input_layer1[IP36_layerId],
        bias[IP36_layerId],
        inp_norm_2[IP36_layerId], 
        inp_norm_3[IP36_layerId],
        istg_out1[IP36_layerId],
        istg_out2[IP36_layerId],
        
        weights1[IP110_layerId],
        weights2[IP110_layerId], 
        output1[IP110_layerId],
        output2[IP110_layerId],
        input1[IP110_layerId],//input1[IP110_layerId], 
        input2[IP110_layerId],//input2[IP110_layerId],
        input_layer1[IP110_layerId],
        bias[IP110_layerId],
        inp_norm_2[IP110_layerId],
        inp_norm_3[IP110_layerId],
        istg_out1[IP110_layerId],
        istg_out2[IP110_layerId],
        
        weights1[IP110_layerId],
        weights2[IP110_layerId], 
        output1[IP110_layerId],
        output2[IP110_layerId],
        input1[IP110_layerId],//input1[IP110_layerId], 
        input2[IP110_layerId],//input2[IP110_layerId],
        input_layer1[IP110_layerId],
        bias[IP110_layerId],
        inp_norm_2[IP110_layerId],
        inp_norm_3[IP110_layerId],
        istg_out1[IP110_layerId],
        istg_out2[IP110_layerId],

        input1[IP36_layerId+1],
        input2[IP36_layerId+1],

        output1[IP36_layerId+1],
        output2[IP36_layerId+1],

        input1[IP110_layerId+1],
        input2[IP110_layerId+1],

        output1[IP110_layerId+1],
        output2[IP110_layerId+1],

        argsIP36,
        argsIP110A,
        argsIP110B,
        argsPool1,
        argsPool2,
        argsDiv,
        argsComb  
    );

    sprintf(filename,"/home/xliu79/Research/2018Fall/debugData/Pipe%02d_Ans", output_layerId);
    printf("%s\n", filename);
    save_answer(filename,output1[output_layerId],output2[output_layerId]);


            
    sprintf(filename,"/home/xliu79/Research/2018Fall/debugData/Pipe_%02d", output_layerId);
    printf("%s\n", filename);
    load_answer(filename,outputSol1[output_layerId],outputSol2[output_layerId]);

    for(int i=0;i<OUTPUT_DEPTH;i++)
    {
        // if(outputSol1[output_layerId][i]!=output1[output_layerId][i]) 
        // {
            printf("Addr[%8d] %08x %08x %08x %08x\n", i,
            (unsigned int) outputSol1[output_layerId][i].range(127,96),
            (unsigned int) outputSol1[output_layerId][i].range(95,64),
            (unsigned int) outputSol1[output_layerId][i].range(63,32),
            (unsigned int) outputSol1[output_layerId][i].range(31,0));
        
            printf("Addr[%8d] %08x %08x %08x %08x\n", i,
            (unsigned int) output1[output_layerId][i].range(127,96),
            (unsigned int) output1[output_layerId][i].range(95,64),
            (unsigned int) output1[output_layerId][i].range(63,32),
            (unsigned int) output1[output_layerId][i].range(31,0));
        // }
    }
    if(!memcmp(outputSol1[output_layerId],output1[output_layerId],sizeof(gmem_outputtype)*OUTPUT_DEPTH) )
    puts("output1 same!");
    else
    puts("output1_differ!");
    
    if(!memcmp(outputSol2[output_layerId],output2[output_layerId],sizeof(gmem_outputtype)*OUTPUT_DEPTH))
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




//   save_answer("/home/xliu79/Research/2018Fall/debugData/dbg_sim_layer0",output1,output2);
//  load_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/dbg_sol_layer0",output1_sol,output2_sol);







#endif



clock_t endTime = clock();

clock_t clockTicksTaken = endTime - startTime;
double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
printf("output time: %f\n", timeInSeconds);
}
