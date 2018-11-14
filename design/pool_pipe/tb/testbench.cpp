#include "../include/maxpool_config.h"
//* we actually don't need any weight in this point

#include <ap_int.h>

#define INBYTES 290400
#define INDEPTH INBYTES/16


#define DATA_FILE_PATH "/home/xliu79/Research/2018Fall/model_args/pool/"
using namespace std;

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
    if(fd==NULL) { printf("%s not found\n", filename); exit(-3);}
    fread(scalar_args,sizeof(int),32, fd);
    fclose(fd);
}


void load_input
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





int main()
{
	
    int scala_pool_args[32];
    GMEM_MAXPOOLTYPE in1[INDEPTH]={};
    GMEM_MAXPOOLTYPE in2[INDEPTH]={};
    GMEM_MAXPOOLTYPE out1[INDEPTH]={};
    GMEM_MAXPOOLTYPE out2[INDEPTH]={};
	
	GMEM_MAXPOOLTYPE out1_sol[INDEPTH]={};
    GMEM_MAXPOOLTYPE out2_sol[INDEPTH]={};
	
	GMEM_MAXPOOLTYPE outtest1[INDEPTH]={};
    GMEM_MAXPOOLTYPE outtest2[INDEPTH]={};
	GMEM_POOLTYPE_WTS wts[INDEPTH]={0};




	int layerId=1;
	int groupId=0;
	load_args_pool(layerId,0,scala_pool_args);

	load_input(layerId,0,in1,out1_sol);
	load_input(layerId,1,in2,out2_sol);





    short in_h        	= scala_pool_args[0]; 
	short in_w        	= scala_pool_args[1]; 
	short out_h       	= scala_pool_args[2]; 
	short out_w      	= scala_pool_args[3];
	short n_planes    	= scala_pool_args[4]; 
	short ps_h	  	    = scala_pool_args[5];
	short ps_w	  	    = scala_pool_args[6];
	short pwin_h	  	= scala_pool_args[7];
	short pwin_w	  	= scala_pool_args[8];
	unsigned char avg_pool	= scala_pool_args[9];
	unsigned char pad	    = scala_pool_args[10];
	unsigned char one_by_diviser	= scala_pool_args[11];
	unsigned char conv3ds	= scala_pool_args[12];
	unsigned char relu		= scala_pool_args[13];
	unsigned char outshift	= scala_pool_args[14];


    cout<<"in_h        	"<< scala_pool_args[0]<<endl;
	cout<<"in_w        	"<< scala_pool_args[1]<<endl; 
	cout<<"out_h       	"<< scala_pool_args[2]<<endl; 
	cout<<"out_w      	"<< scala_pool_args[3]<<endl;
	cout<<"n_planes    	"<< scala_pool_args[4]<<endl; 
	cout<<"ps_h	  	    "<< scala_pool_args[5]<<endl;
	cout<<"ps_w	  	    "<< scala_pool_args[6]<<endl;
	cout<<"pwin_h	  	"<< scala_pool_args[7]<<endl;
	cout<<"pwin_w	  	"<< scala_pool_args[8]<<endl;
	cout<<"avg_pool	    "<< scala_pool_args[9]<<endl;
	cout<<"pad	        "<< scala_pool_args[10]<<endl;
	cout<<"one_by_diviser	"<< scala_pool_args[11]<<endl;
	cout<<"conv3ds	"<< scala_pool_args[12]<<endl;
	cout<<"relu		"<< scala_pool_args[13]<<endl;
	cout<<"outshift	"<< scala_pool_args[14]<<endl;




	uRowIdx_t rowStep = 1;
 	uRowIdx_t initialReadRows = pwin_h+(rowStep-1)*ps_h-pad;
    ap_uint<32> inDDRPlaneStep= in_h*in_w;
    ap_uint<32> outDDRPlaneStep= out_w*out_h;

	scala_pool_args[15] = rowStep;
	scala_pool_args[16] = initialReadRows;
	scala_pool_args[17] = inDDRPlaneStep;
    scala_pool_args[18] = outDDRPlaneStep;
	scala_pool_args[19] = 0;
    scala_pool_args[20] = 0;





	// for(int i=0;i<INDEPTH;i++)
	// {
	// 	ap_uint<128> par=0;

	// 	par.range(7,0)=i/(in_h*in_w);
	// 	par.range(23,16) = (i/in_w)%in_h;
	// 	par.range(39,32) = i%in_w;
	// 	par.range(55,48) = 33;
	// 	in1[i]=par;
	// 	in2[i]=par;
	// }

	PoolTop_Gold(in1,in2, out1, out2,
	#if !RESERVE
	wts,
	#endif
	scala_pool_args);



	PoolTop
	(in1, in2, outtest1, outtest2, wts, scala_pool_args);


	


	 for(int i=0;i<INDEPTH;i++)
	 {
	 	if(out1[i]!=outtest1[i] || out2[i]!=outtest2[i])
	 	{
	 		ap_uint<128> par=out1[i];
	 		ap_uint<128> par2=outtest1[i];
	 		printf("Addr %5d:",i);
	 		for(int c=0;c<16;c++)
	 		{
	 			printf("%02x ",(unsigned char) par.range(c*8+7,c*8) );
	 		}
	 		printf("\n");
	 		printf("Addr %5d:",i);
	 		for(int c=0;c<16;c++)
	 		{
	 			printf("%02x ",(unsigned char) par2.range(c*8+7,c*8) );
	 		}
	 		printf("\n");
	 	}
	 }

	if(memcmp(outtest1,out1_sol, sizeof(char)*INBYTES)) 
	printf("output 1 differ\n");
	else
	printf("output 1 same\n");


	if(memcmp(outtest2,out2_sol, sizeof(char)*INBYTES)) 
	printf("output 2 differ\n");
	else
	printf("output 2 same\n");

}



