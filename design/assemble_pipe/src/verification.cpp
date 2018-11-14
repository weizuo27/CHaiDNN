#include "testbench_defines.h"
#include <stdio.h>
#include "../include/xi_conv_config.h"


#define DBG_DATA_PATH "/home/xliu79/Research/2018Fall/debugData/"



void memToStream(
    int height,
    int width,
    int depthBy16,
    hls::stream< ap_uint<128> > &stream,
    ap_uint<128>* mem,
    int offset
)
{
    for(int rowIdx=0;rowIdx<height;rowIdx++)
    {
        for(int j=0;j<depthBy16;j++)
        {
            for(int colIdx=0;colIdx<width;colIdx++)
            {
                int addr=offset+j*width*height+rowIdx*width+colIdx;
                ap_uint<128> temp;
                temp=mem[addr];
                stream<<temp;
            }
        }
    }
}


void streamToMem(
    int height,
    int width,
    int depthBy16,
    hls::stream< ap_uint<128> > &stream,
    ap_uint<128>* mem,
    int offset
)
{
    for(int rowIdx=0;rowIdx<height;rowIdx++)
    {
        for(int j=0;j<depthBy16;j++)
        {
            for(int colIdx=0;colIdx<width;colIdx++)
            {
                int addr=offset+j*width*height+rowIdx*width+colIdx;
                ap_uint<128> temp;
                stream>>temp;
                mem[addr]=temp;
            }
        }
    }
}

void saveStreamData(int layerId, int groupId,
                    hls::stream< ap_uint<128> > &stream1,
                    hls::stream< ap_uint<128> > &stream2
                    )
{

    char filename[200];
    sprintf(filename, DBG_DATA_PATH"stream%dTo%d_group%d", layerId, layerId+1, groupId);
    FILE* fd = fopen(filename, "wb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    ap_uint<128> temp;

    while(stream1.read_nb(temp))
    {
        fwrite(&temp,sizeof(ap_uint<128> ), 1, fd);
    }
    while(stream2.read_nb(temp))
    {
        fwrite(&temp,sizeof(ap_uint<128> ), 1, fd);
    }
    fclose(fd);                    
}


void loadStreamData(int layerId, int groupId,
                    hls::stream< ap_uint<128> > &stream1,
                    hls::stream< ap_uint<128> > &stream2
                    )
{

    char filename[200];
    sprintf(filename, DBG_DATA_PATH"stream%dTo%d_group%d", layerId, layerId+1, groupId);
    FILE* fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    ap_uint<128> temp;

    while(fread(&temp,sizeof(ap_uint<128> ), 1, fd))
    {
        stream1<<temp;
    }
    while(fread(&temp,sizeof(ap_uint<128> ), 1, fd))
    {
        stream2<<temp;
    }
    fclose(fd);                    
}





void load_args_pool
(
    int layerIdx,
    int groupIdx,
    int *scalar_args
)
{ 
    char filename[200];
    FILE* fd;
    sprintf(filename,DATA_FILE_PATH"pool_args_%d_group%d", layerIdx, groupIdx);
    fd = fopen(filename, "rb");

    if(fd==NULL) { 
        printf("%s not found\n", filename); 
        
        return;
        }
    fread(scalar_args,sizeof(int),32, fd);
    fclose(fd);

    short in_h        	= scalar_args[0]; 
	short in_w        	= scalar_args[1]; 
	short out_h       	= scalar_args[2]; 
	short out_w      	= scalar_args[3];
	short n_planes    	= scalar_args[4]; 
	short ps_h	  	    = scalar_args[5];
	short ps_w	  	    = scalar_args[6];
	short pwin_h	  	= scalar_args[7];
	short pwin_w	  	= scalar_args[8];
	unsigned char avg_pool	= scalar_args[9];
	unsigned char pad	    = scalar_args[10];
	unsigned char one_by_diviser	= scalar_args[11];
	unsigned char conv3ds	= scalar_args[12];
	unsigned char relu		= scalar_args[13];
	unsigned char outshift	= scalar_args[14];

	uRowIdx_t rowStep = 1;
 	uRowIdx_t initialReadRows = pwin_h+(rowStep-1)*ps_h-pad;
    ap_uint<32> inDDRPlaneStep= in_h*in_w;
    ap_uint<32> outDDRPlaneStep= out_w*out_h;

	scalar_args[15] = rowStep;
	scalar_args[16] = initialReadRows;
	scalar_args[17] = inDDRPlaneStep;
    scalar_args[18] = outDDRPlaneStep;
	scalar_args[19] = 0;
    scalar_args[20] = 0;

}


void load_pool_input
(
	int layerId,
	int groupId,
	GMEM_MAXPOOLTYPE in[INDEPTH],
	GMEM_MAXPOOLTYPE out[INDEPTH]
)
{
	char filename[200];
    FILE* fd;
    sprintf(filename,DATA_FILE_PATH"pool_input_%d_%d", layerId, groupId);
	
	fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
	fread(in,sizeof(char),INBYTES, fd);
    fclose(fd);

	sprintf(filename,DATA_FILE_PATH"pool_output_%d_%d", layerId, groupId);
	fd = fopen(filename, "rb");
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
	fread(out,sizeof(char),INBYTES, fd);
    fclose(fd);
}



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
    if(fd==NULL) { 
        printf("%s not found\n", filename); 
      
        return;}
    fread(scalar_args,sizeof(int),128, fd);
    scalar_args[126]=0;
    scalar_args[127]=0;
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
    gmem_outputtype* output2_gold)
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

