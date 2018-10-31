#include <ap_int.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <hls_stream.h>

#include "xi_conv_config.h"
#include "xi_conv_desc.h"
#define SCALA_ARG_PATH15 "/home/xliu79/Research/2018Fall/CHaiDNN/SD_Card/Alex2_args.bin"
#define SCALA_ARG_PATH16 "/home/xliu79/Research/2018Fall/CHaiDNN/SD_Card/Alex3_args.bin"
#define SCALA_ARG_PATH17 "/home/xliu79/Research/2018Fall/CHaiDNN/SD_Card/Alex16_args.bin"

#define SCALA_ARG_PATH1 "/home/xliu79/Research/2018Fall/CHaiDNN/SD_Card/Alex1_args.bin"
#define WEIGHT_DEPTH 576
#define OUT_MEM_DEPTH 4096
#define IN_MEM_DEPTH 4096

#define GEN_SOL 0

void random_input
(
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
    gmem_inputtype_layer1* istg_out1,
    gmem_inputtype_layer1* istg_out2
)
{
    int fd = open("/dev/random", O_RDONLY);
    read(fd, weights1,sizeof(gmem_weighttype)*WEIGHT_DEPTH);
    read(fd, weights2,sizeof(gmem_weighttype)*WEIGHT_DEPTH);
    read(fd, weights3,sizeof(gmem_weighttype)*WEIGHT_DEPTH);
    read(fd, weights4,sizeof(gmem_weighttype)*WEIGHT_DEPTH);

    read(fd, input_other1,sizeof(gmem_inputtype_layerx)*IN_MEM_DEPTH);
    read(fd, input_other2,sizeof(gmem_inputtype_layerx)*IN_MEM_DEPTH);
    read(fd, bias,sizeof(gmem_biastype)*32);
    read(fd, input_layer1,sizeof(gmem_inputtype_layer1)*IN_MEM_DEPTH);
    
    read(fd, inp_norm_2,sizeof(gmem_inputtype_layer1)*IN_MEM_DEPTH);
    read(fd, inp_norm_3,sizeof(gmem_inputtype_layer1)*IN_MEM_DEPTH);
    read(fd, istg_out1,sizeof(gmem_inputtype_layer1)*IN_MEM_DEPTH);
    read(fd, istg_out2,sizeof(gmem_inputtype_layer1)*IN_MEM_DEPTH);
    close(fd);
}





void save_input
(
    char* filename,
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
    gmem_inputtype_layer1* istg_out1,
    gmem_inputtype_layer1* istg_out2
)
{
    FILE* fd = fopen(filename, "wb");
    if(fd==NULL) exit(-3);
    fwrite(weights1,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fwrite(weights2,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fwrite(weights3,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fwrite(weights4,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);

    fwrite(input_other1,sizeof(gmem_inputtype_layerx), IN_MEM_DEPTH, fd);
    fwrite(input_other2,sizeof(gmem_inputtype_layerx), IN_MEM_DEPTH, fd);
    fwrite(bias,sizeof(gmem_biastype),64,fd);
    fwrite(input_layer1,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);

    fwrite(inp_norm_2,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fwrite(inp_norm_3,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fwrite(istg_out1,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fwrite(istg_out2,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fclose(fd);
}

void load_input
(
    char* filename,
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
    gmem_inputtype_layer1* istg_out1,
    gmem_inputtype_layer1* istg_out2
)
{
    FILE* fd = fopen(filename, "rb");
    if(fd==NULL) exit(-3);
    fread(weights1,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fread(weights2,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fread(weights3,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);
    fread(weights4,sizeof(gmem_weighttype), WEIGHT_DEPTH,fd);

    fread(input_other1,sizeof(gmem_inputtype_layerx), IN_MEM_DEPTH, fd);
    fread(input_other2,sizeof(gmem_inputtype_layerx), IN_MEM_DEPTH, fd);
    fread(bias,sizeof(gmem_biastype),64,fd);
    fread(input_layer1,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);

    fread(inp_norm_2,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fread(inp_norm_3,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fread(istg_out1,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
    fread(istg_out2,sizeof(gmem_inputtype_layer1),IN_MEM_DEPTH,fd);
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
    fwrite( output1_gold,sizeof(gmem_outputtype),OUT_MEM_DEPTH,fd);
    fwrite( output2_gold,sizeof(gmem_outputtype),OUT_MEM_DEPTH,fd);
    fclose(fd);
}


void load_answer
(
    char* filename,
    gmem_outputtype* output1_gold,
    gmem_outputtype* output2_gold
)
{
    FILE* fd = fopen(filename,"rb");
    fread( output1_gold,sizeof(gmem_outputtype),OUT_MEM_DEPTH,fd);
    fread( output2_gold,sizeof(gmem_outputtype),OUT_MEM_DEPTH,fd);
    fclose(fd);
}





void prepareStream( 
    gmem_inputtype_layerx* input,
    int width,
    int height,
    int depth,
    int rowStep, 
    hls::stream< gmem_inputtype_layerx > & inStream
    )
{
    int depthBy16=depth/16;
    int cnt=0;
    for(int r=0;r<height;r+=rowStep)
    {
        for(int d=0;d<depthBy16;d++)
        {
            for(int rr=r;rr<height && rr< r+rowStep;rr++)
            {
                for(int w=0;w<width;w++)
                {
                    int address=d*height*width+rr*width+w;
                    inStream<<input[address];
                    cnt++;
                }
            }
        }
    }

    printf("write out in %d to sidebuffer\n",cnt);
}


void decodeStream( 
    gmem_inputtype_layerx* input,
    int width,
    int height,
    int depth,
    int rowStep, 
    hls::stream< gmem_inputtype_layerx > & inStream
    )
{
    int depthBy16=depth/16;
    int cnt=0;
    for(int r=0;r<height;r+=rowStep)
    {
        for(int d=0;d<depthBy16;d++)
        {
            for(int rr=r;rr<height && rr< r+rowStep;rr++)
            {
                for(int w=0;w<width;w++)
                {
                    int address=d*height*width+rr*width+w;
                    
                    inStream>>input[address];
                    cnt++;
                }
            }
        }
    }

    printf("write out in %d to sidebuffer\n",cnt);
}





int main()
{
    int scala_args1[128]={};
    int scala_args2[128]={};
    gmem_weighttype weights1[WEIGHT_DEPTH]= {};
    gmem_weighttype weights2[WEIGHT_DEPTH]={};
    gmem_weighttype weights3[WEIGHT_DEPTH]={};
    gmem_weighttype weights4[WEIGHT_DEPTH]={};

    gmem_inputtype_layerx input_other1[IN_MEM_DEPTH]={};
    gmem_inputtype_layerx input_other2[IN_MEM_DEPTH]={};
    gmem_inputtype_layer1 input_layer1[IN_MEM_DEPTH]={};
    gmem_biastype bias[64]={};

//** following 2 argument are no longer required for VGG net
    gmem_inputtype_layer1 inp_norm_2[IN_MEM_DEPTH]={};
    gmem_inputtype_layer1 inp_norm_3[IN_MEM_DEPTH]={};
//** nolonger required



    gmem_inputtype_layer1 istg_out1[IN_MEM_DEPTH]={};
    gmem_inputtype_layer1 istg_out2[IN_MEM_DEPTH]={};



    gmem_inputtype_layer1 istg_out1_gold[IN_MEM_DEPTH]={};
    gmem_inputtype_layer1 istg_out2_gold[IN_MEM_DEPTH]={};





    gmem_outputtype output1[OUT_MEM_DEPTH]={};
    gmem_outputtype output2[OUT_MEM_DEPTH]={};

    gmem_outputtype output3[OUT_MEM_DEPTH]={};
    gmem_outputtype output4[OUT_MEM_DEPTH]={};


    gmem_outputtype output1_sol[OUT_MEM_DEPTH]={};
    gmem_outputtype output2_sol[OUT_MEM_DEPTH]={};
    gmem_outputtype output3_sol[OUT_MEM_DEPTH]={};
    gmem_outputtype output4_sol[OUT_MEM_DEPTH]={};


    //random_input(weights1,weights2,weights3,weights4,input_other1,input_other2, input_layer1, bias, inp_norm_2,inp_norm_3,istg_out1, istg_out2);
    //save_input("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/randInput",weights1,weights2,weights3,weights4,input_other1,input_other2, input_layer1, bias, inp_norm_2,inp_norm_3,istg_out1, istg_out2);
    load_input("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/randInput",weights1,weights2,weights3,weights4,input_other1,input_other2, input_layer1, bias, inp_norm_2,inp_norm_3,istg_out1, istg_out2);
    
    hls::stream< gmem_inputtype_layerx > inStream1;
    hls::stream< gmem_inputtype_layerx > inStream2;


    hls::stream< gmem_inputtype_layerx > outStream1;
    hls::stream< gmem_inputtype_layerx > outStream2;

//     for(int i=0;i<IN_MEM_DEPTH;i++)
//     {
//     	gmem_inputtype_layerx par=i*2;
//         par.range(63,32)=i*2;
//     	par.range(95,64)=i*2;
//         par.range(127,96)=i*2;
//     	input_other1[i]=par;
//         par=i*2+1;
//         par.range(63,32)=i*2+1;
//     	par.range(95,64)=i*2+1;
//         par.range(127,96)=i*2+1;
//     	input_other2[i]=par;
//         // printf("addr:%d, value: %x\t%x\n", i,(int)input_other1[i].range(63,0), (int) input_other1[i].range(127,64) );
//     }



  prepareStream(input_other1,28,28,64,7,inStream1);
  prepareStream(input_other2,28,28,64,7,inStream2);

    scala_args1[125]=28*7*3+28;
    scala_args1[126]=0;
    scala_args1[127]=0;



    for(int i=0;i<64;i++)
    {
        ap_uint<64> par;
        par.range(31,0)=2789;
        par.range(63,32)=2789;
        bias[i]=par;
    }


        FILE* arg_file_ptr;
    arg_file_ptr=fopen(SCALA_ARG_PATH15,"rb");
    if(arg_file_ptr==NULL) exit(255);
    fread(scala_args1,sizeof(int), 110,arg_file_ptr);
    fclose(arg_file_ptr);
    XiConvolutionTop(
    		weights1,
			weights2,
			weights3,
			weights4,
		#if !XI_SINGLE_IO_PORT_EN
			#if STREAMOUT == 0
				output1, 
				output2,
			#elif STREAMOUT == 1
				outStream1,outStream2,
			#else
				output1 + output_desc.out_offset, 
				output2 + output_desc.out_offset,
				outStream1,outStream2,
			#endif
		#else
			#if STREAMOUT == 0
				output1 + output_desc.out_offset, 
			#elif STREAMOUT == 1
				outStream1,
			#else
				output1 + output_desc.out_offset, 
				outStream1,
			#endif
		#endif
        #if !XI_SINGLE_IO_PORT_EN
			#if STREAMIN== 0
				input_other1, input_other2,
			#elif STREAMOUT == 1
				inStream1, inStream2,
			#else
				input_other1, input_other2,
				inStream1, inStream2,
			#endif
		#else
			#if STREAMIN == 0
				input_other1,
			#elif STREAMIN == 1
				inStream1, 
			#else
				input_other1,
				inStream1, 
			#endif
		#endif
			input_layer1,
			bias,
			istg_out1,
			istg_out2,
			scala_args1,0
			);
    decodeStream(output1,28,28,64,7,outStream1);
    decodeStream(output2,28,28,64,7,outStream2);



    // XiConvolutionTop_pipe(
    // 		weights1,
	// 		weights2,
	// 		weights3,
	// 		weights4,
    // 		weights1,
	// 		weights2,
	// 		weights3,
	// 		weights4,
	// 		output1,
	// 		output2,
	// 		output3,
	// 		output4,
    //         outStream1,
    //         outStream2,
	// 		input_other1,
	// 		input_other2,
	// 		input_other1,
	// 		input_other2,
    //         inStream1,
    //         inStream2,
	// 		input_layer1,
	// 		input_layer1,
	// 		bias,
	// 		bias,
	// 		istg_out1,
	// 		istg_out2,
	// 		istg_out1,
	// 		istg_out2,
	// 		scala_args1,
	// 		scala_args2,
	// 		0,
	// 		0
	// 		);
//    decodeStream(output3,28,28,64,7,outStream1);
//    decodeStream(output4,28,28,64,7,outStream2);
       
//    arg_file_ptr=fopen(SCALA_ARG_PATH16,"rb");
//    if(arg_file_ptr==NULL) exit(255);
//    fread(scala_args,sizeof(int), 110,arg_file_ptr);
//    fclose(arg_file_ptr);
//    XiConvolutionTop(
//    		weights1,
// 			weights2,
// 			weights3,
// 			weights4,
// 			output3,
// 			output4,
// 			output1,
// 			output2,
// 			input_layer1,
// 			bias,
// 			istg_out1,
// 			istg_out2,
// 			scala_args,0);

#if DBG_INFO
    for(int i=0;i<OUT_MEM_DEPTH;i++)
    {
        printf("Output1 addr:%d, value: %x\t%x\n",i,(int) output1[i].range(63,0), (int)output1[i].range(127,64));
        printf("Output2 addr:%d, value: %x\t%x\n",i,(int) output2[i].range(63,0), (int)output2[i].range(127,64));
        printf("Output3 addr:%d, value: %x\t%x\n",i,(int) output3[i].range(63,0), (int)output3[i].range(127,64));
        printf("Output4 addr:%d, value: %x\t%x\n",i,(int) output4[i].range(63,0), (int)output4[i].range(127,64));
    }

    
//     for(int i=0;i<IN_MEM_DEPTH;i++)
//    {
//        printf("INput1 addr:%d, value: %x\t%x\n",i,(int) input_other1[i].range(63,0), (int)input_other1[i].range(127,64));
//        printf("INput2 addr:%d, value: %x\t%x\n",i,(int) input_other2[i].range(63,0), (int)input_other2[i].range(127,64));
//    }


//    for(int i=0;i<WEIGHT_DEPTH;i++)
//    {
//        printf("weights1 addr:%d, value: %x\t%x\n",i,(int) weights1[i].range(63,0), (int)weights1[i].range(127,64));
//        printf("weights2 addr:%d, value: %x\t%x\n",i,(int) weights2[i].range(63,0), (int)weights2[i].range(127,64));
//        printf("weights3 addr:%d, value: %x\t%x\n",i,(int) weights3[i].range(63,0), (int)weights3[i].range(127,64));
//        printf("weights4 addr:%d, value: %x\t%x\n",i,(int) weights4[i].range(63,0), (int)weights4[i].range(127,64));
//    }
#endif


#if GEN_SOL
    save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer0",output1,output2);
    save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer1",output3,output4);
#else

    load_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer0",output1_sol,output2_sol);
    load_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/sol_layer1",output3_sol,output4_sol);

    save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/ans2_layer0",output1,output2);
    save_answer("/home/xliu79/Research/2018Fall/CHaiPipeline/hls_project/src/gold_model/ans2_layer1",output3,output4);

    if(!memcmp(output1_sol,output1,sizeof(gmem_outputtype)*OUT_MEM_DEPTH))
    puts("output1 same!");
    else
    puts("output1 dffer!");
    if(!memcmp(output2_sol,output2,sizeof(gmem_outputtype)*OUT_MEM_DEPTH))
    puts("output2 same!");
    else
    puts("output2 dffer!");
    if(!memcmp(output3_sol,output3,sizeof(gmem_outputtype)*OUT_MEM_DEPTH))
    puts("output3 same!");
    else
    puts("output３ dffer!");
    if(!memcmp(output4_sol,output4,sizeof(gmem_outputtype)*OUT_MEM_DEPTH))
    puts("output4 same!");
    else
    puts("output４ dffer!");
#endif


    // arg_file_ptr=fopen(SCALA_ARG_PATH1,"r");
    // if(arg_file_ptr==NULL) exit(255);
    // fread(scala_args,sizeof(int), 110,arg_file_ptr);
    // fclose(arg_file_ptr);
    //     XiConvolutionTop(
    // 		weights1,
	// 		weights2,
	// 		weights3,weights4,output1,output2, input_other1,input_other2,input_layer1,bias,inp_norm_2,inp_norm_3,istg_out1, istg_out2,scala_args);





}
