#include "maxpool_config.h"
//* we actually don't need any weight in this point

#include <ap_int.h>

#define INDEPTH 4096

int main()
{
	
    int scala_pool_args[32];
    GMEM_MAXPOOLTYPE in1[INDEPTH]={};
    GMEM_MAXPOOLTYPE in2[INDEPTH]={};
    GMEM_MAXPOOLTYPE out1[INDEPTH]={};
    GMEM_MAXPOOLTYPE out2[INDEPTH]={};
	GMEM_MAXPOOLTYPE outtest1[INDEPTH]={};
    GMEM_MAXPOOLTYPE outtest2[INDEPTH]={};
	GMEM_POOLTYPE_WTS wts[INDEPTH]={0};



    short in_h        	= 27;
	short in_w        	= 27;
	short out_h       	= 13;
	short out_w      	= 13;
	short n_planes    	= 64;
	short ps_h	  	    = 2;
	short ps_w	  	    = 2;
	short pwin_h	  	= 3;
	short pwin_w	  	= 3;
	unsigned char avg_pool	= 0;
	unsigned char pad	    = 0;
	unsigned char one_by_diviser	= 1;
	unsigned char conv3ds	= 0;
	unsigned char relu		= 0;
	unsigned char outshift	= 0;
	uRowIdx_t rowStep = 1;
 	uRowIdx_t initialReadRows = pwin_h+(rowStep-1)*ps_h;
    uPixelIdx_t inlineBufferPlaneStep = (rowStep*ps_h+initialReadRows)*in_w;
    uPixelIdx_t outlineBufferPlaneStep =out_w*2*rowStep;
    ap_uint<32> inDDRPlaneStep= in_h*in_w;
    ap_uint<32> outDDRPlaneStep= out_w*out_h;

	scala_pool_args[0] = in_h;
	scala_pool_args[1] = in_w;
	scala_pool_args[2] = out_h;
	scala_pool_args[3] = out_w;
	scala_pool_args[4] = n_planes;
	scala_pool_args[5] = ps_h;
	scala_pool_args[6] = ps_w;
	scala_pool_args[7] = pwin_h;
	scala_pool_args[8] = pwin_w;
	scala_pool_args[9] = avg_pool;
	scala_pool_args[10] = pad;
	scala_pool_args[11] = one_by_diviser;
	scala_pool_args[12] = conv3ds;
	scala_pool_args[13] = relu;
	scala_pool_args[14] = outshift;
	scala_pool_args[15] = rowStep;
	scala_pool_args[16] = initialReadRows;
	scala_pool_args[17] = inlineBufferPlaneStep;
	scala_pool_args[18] = outlineBufferPlaneStep;
	scala_pool_args[19] = inDDRPlaneStep;
    scala_pool_args[20] = outDDRPlaneStep;





	for(int i=0;i<INDEPTH;i++)
	{
		ap_uint<128> par=0;
		par.range(7,0)=i%128;
		par.range(23,16) = i%128;
		par.range(39,32) = i%128;
		par.range(55,48) = i%128;
		in1[i]=par;
		in2[i]=par;
	}
//
	PoolTop_Gold(in1,in2, out1, out2,
	#if !RESERVE
	wts,
	#endif
	scala_pool_args);



	PoolTop
	(in1, in2, outtest1, outtest2, wts, scala_pool_args);


	


//	for(int i=0;i<INDEPTH;i++)
//	{
//		if(out1[i]!=outtest1[i])
//		{
//			ap_uint<128> par=out1[i];
//			ap_uint<128> par2=outtest1[i];
//				printf("ADDRS[%d][%d %d %d %d][%d %d %d %d]\n",i, (char) par.range(63,48),
//			(char) par.range(47,32), (char) par.range(31,16),(char)par.range(15,0),(char) par2.range(63,48),
//			(char) par2.range(47,32), (char) par2.range(31,16),(char)par2.range(15,0) );
//		}
//	}
}



