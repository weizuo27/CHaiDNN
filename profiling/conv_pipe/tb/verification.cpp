#include "testbench_defines.h"
#include <stdio.h>
#include "../include/xi_conv_config.h"



void load_args_conv
(
    int layerIdx,
    int groupIdx,
    int *scalar_args
)
{ 
    char filename[200];
    FILE* fd;
    sprintf(filename,DATA_FILE_PATH"args_%d_group%d", layerIdx, groupIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(scalar_args,sizeof(int),128, fd);
    fclose(fd);
}



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
)
{
    char filename[200];

    FILE* fd;

  

    //* read weight data *//
    sprintf(filename,DATA_FILE_PATH"weight_%d_0", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(weights1,sizeof(char),   WEIGHT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"weight_%d_1", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(weights2,sizeof(char),   WEIGHT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"weight_%d_2", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(weights3,sizeof(char),   WEIGHT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"weight_%d_3", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(weights4,sizeof(char),   WEIGHT_BYTES,fd);
    fclose(fd);

    //* read input data *//

    sprintf(filename,DATA_FILE_PATH"input_%d_0", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(input_other1,sizeof(char), INPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"input_%d_1", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(input_other2,sizeof(char), INPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"input_%d_2", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(input_layer1,sizeof(char), INPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"input_%d_3", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(inp_norm_2,sizeof(char), INPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"input_%d_4", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(inp_norm_3,sizeof(char), INPUT_BYTES,fd);
    fclose(fd);


    //* read bias data
    sprintf(filename,DATA_FILE_PATH"bias_%d", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(bias,sizeof(char),BIAS_BYTES,fd);
    fclose(fd);
    

    //* output data
    sprintf(filename,DATA_FILE_PATH"output_%d_0", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(output1,sizeof(char), OUTPUT_BYTES ,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"output_%d_1", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(output2,sizeof(char), OUTPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"output_%d_2", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(istg_out1,sizeof(char), OUTPUT_BYTES,fd);
    fclose(fd);

    sprintf(filename,DATA_FILE_PATH"output_%d_3", layerIdx);
    fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(istg_out2,sizeof(char), OUTPUT_BYTES,fd);
    fclose(fd);
}



void save_answer
(
    char* filename,
    gmem_outputtype* output1_gold,
    gmem_outputtype* output2_gold
)
{
    FILE* fd = fopen(filename, "wb");
    fwrite( output1_gold,sizeof(char),OUTPUT_BYTES,fd);
    fwrite( output2_gold,sizeof(char),OUTPUT_BYTES,fd);
    fclose(fd);
}



void load_answer
(
    char* filename,
    gmem_outputtype* output1_gold,
    gmem_outputtype* output2_gold
)
{
    FILE* fd = fopen(filename, "rb");
    if(fd==NULL)
    {
        fprintf(stderr,"%s not found", filename);
        exit(-4);
    }
    fread( output1_gold,sizeof(char),OUTPUT_BYTES,fd);
    fread( output2_gold,sizeof(char),OUTPUT_BYTES,fd);
    fclose(fd);
}

