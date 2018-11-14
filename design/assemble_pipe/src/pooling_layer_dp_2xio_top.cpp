/*----------------------------------------------------
Copyright 2017 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
----------------------------------------------------*/

#include <ap_int.h>
#include <string.h>

//# Debug flag
#define DEBUG			0


//# Some FILE Handlers for debug
FILE *iFp = NULL, *oFp = NULL, *rFp = NULL, *mpFp = NULL;

#ifndef __SYNTHESIS__
#define FILE_OUTPUT	0
#else
#define FILE_OUTPUT	0
#endif

FILE * pFp = NULL, *wFp = NULL, *apFp = NULL;

#include "../include/maxpool_config.h"
#include "../include/pool_dp_2xio.hpp"
#include "../include/pool_layer.hpp"

#ifndef __SDSOC

#define DPACK 16
void PoolingLayerLineBuffer
(
    GMEM_MAXPOOLTYPE* inMem1,
    GMEM_MAXPOOLTYPE* inMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream2,
    GMEM_MAXPOOLTYPE* outMem1,
    GMEM_MAXPOOLTYPE* outMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream2,
    int* scalarPoolArgs
)
{


    ap_int<8> outBuffer1[DPACK][OUT_LINE_BUFFER_SIZE];
    ap_int<8> outBuffer2[DPACK][OUT_LINE_BUFFER_SIZE];

#pragma HLS array_partition variable=outBuffer1 dim=1 complete
#pragma HLS array_partition variable=outBuffer2 dim=1 complete

#pragma HLS resource variable=ourBuffer1 core=RAM_S2P_BRAM
#pragma HLS resource variable=ourBuffer2 core=RAM_S2P_BRAM

    ap_int<8> inBuffer1[DPACK][IN_LINE_BUFFER_SIZE];
    ap_int<8> inBuffer2[DPACK][IN_LINE_BUFFER_SIZE];

#pragma HLS array_partition variable=inBuffer1 dim=1 complete
#pragma HLS array_partition variable=inBuffer2 dim=1 complete
#pragma HLS resource variable=inBuffer1 core=RAM_S2P_BRAM
#pragma HLS resource variable=inBuffer2 core=RAM_S2P_BRAM

    int scalar_pool_args[32];
    for(int i=0;i<32;i++)
    {
        scalar_pool_args[i]=scalarPoolArgs[i];
    }

    uRowIdx_t in_h        	= (uRowIdx_t)scalar_pool_args[0];
	uRowIdx_t in_w        	= (short)scalar_pool_args[1];
	uRowIdx_t out_h       	= (short)scalar_pool_args[2];
	uRowIdx_t out_w      	= (short)scalar_pool_args[3];
	uPlnIdxPool_t n_planes    	= (short)scalar_pool_args[4];
	uDimPool_t ps_h	  	    = (short)scalar_pool_args[5];
	uDimPool_t ps_w	  	    = (short)scalar_pool_args[6];
	uDimPool_t pwin_h	  	= (short)scalar_pool_args[7];
	uDimPool_t pwin_w	  	= (short)scalar_pool_args[8];
	bool avg_pool	= (unsigned char)scalar_pool_args[9];
	uDimPool_t pad	    = (unsigned char)scalar_pool_args[10];
	ap_uint<8> one_by_diviser	= (unsigned char)scalar_pool_args[11];
	bool conv3ds	= (bool)scalar_pool_args[12];
	bool relu		= (bool)scalar_pool_args[13];
	ap_uint<5> outshift	= (ap_uint<5>)scalar_pool_args[14];
    uRowIdx_t rowStep = (uRowIdx_t) scalar_pool_args[15];
    uRowIdx_t initialReadRows = (uRowIdx_t) scalar_pool_args[16];
    ap_uint<32> inDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[17];
    ap_uint<32> outDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[18];

    bool inStreamFlag= scalar_pool_args[19];
    bool outStreamFlag= scalar_pool_args[20];

    sPlnPackIdx_t planePackNum;
    if(n_planes%16)
        planePackNum= (n_planes>>4)+1;
    else
        planePackNum= n_planes>>4;


    sRowIdx_t readStartRow= 0;
    sRowIdx_t inStartRow=-pad;

    uRowIdx_t inRowStep= rowStep*ps_h;
    
   

    //** first tiling readin*//
    ReadInBuffer_Pooling<16>
    (
        inMem1,
        inMem2,
        inStream1,
        inStream2,
        readStartRow,
        readStartRow+initialReadRows,
        n_planes,
        in_h,
        in_w,
        planePackNum,
        inDDRPlaneStep,
        inBuffer1,
        inBuffer2,
        inStreamFlag
    );
    readStartRow+=initialReadRows;

    sRowIdx_t writeStartRow=-rowStep;

    for(uRowIdx_t outStartRow=0, outEndRow=rowStep; outStartRow<out_h;
            outStartRow+=rowStep)
    {
#pragma HLS dependence variable=inBuffer2 intra false
#pragma HLS dependence variable=inBuffer1 intra false
#pragma HLS dependence variable=outBuffer1 intra false
#pragma HLS dependence variable=outBuffer2 intra false
        
        ProcessPoolingRow<DPACK>(
        inBuffer1,inBuffer2,
        outBuffer1,outBuffer2,
        outStartRow,
        outStartRow+rowStep,
        inStartRow,
        out_w,
        out_h,
        in_w,
        in_h,
        n_planes,
        pwin_h,
        pwin_w,
        ps_h,
        ps_w,
        one_by_diviser,
        outshift,
        pad,
        avg_pool,
        planePackNum
        );
        inStartRow+=inRowStep;


        WriteOutBuffer_Pooling<DPACK>
        (
            outMem1,outMem2,
            outStream1,
            outStream2,
            writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
            planePackNum,outDDRPlaneStep,outBuffer1,outBuffer2,
            outStreamFlag
        );
        writeStartRow+=rowStep;
            
        ReadInBuffer_Pooling<DPACK>
        (
        inMem1,
        inMem2,
        inStream1,
        inStream2,
            readStartRow,
            readStartRow+inRowStep,
            n_planes,
            in_h,
            in_w,
            planePackNum,
            inDDRPlaneStep,
            inBuffer1,
            inBuffer2,
            inStreamFlag
        );    
        readStartRow+=inRowStep;
    }

    WriteOutBuffer_Pooling<DPACK>
    (
        
        outMem1,outMem2,
        outStream1,
        outStream2,
        writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
        planePackNum,outDDRPlaneStep,outBuffer1,outBuffer2,
        outStreamFlag
    );
}





int PoolTop_Gold(
	GMEM_MAXPOOLTYPE *in1, 
	GMEM_MAXPOOLTYPE *in2, 
	GMEM_MAXPOOLTYPE *out1, 
	GMEM_MAXPOOLTYPE *out2,
	
	#if !RESERVE
	GMEM_POOLTYPE_WTS * wts,
	#endif 
	int *scalar_pool_args)
#else
void PoolTop_Gold(
	GMEM_MAXPOOLTYPE *in1, 
	GMEM_MAXPOOLTYPE *in2, 
	GMEM_MAXPOOLTYPE *out1, 
	GMEM_MAXPOOLTYPE *out2, 
	GMEM_POOLTYPE_WTS * wts, 
	int *scalar_pool_args)
#endif
{
#ifndef __SDSOC
#pragma HLS interface m_axi port=in1       		offset=slave bundle=gmem0 depth=4096
#pragma HLS interface m_axi port=out1			offset=slave bundle=gmem1 depth=4096
#pragma HLS interface m_axi port=in2       		offset=slave bundle=gmem2 depth=4096
#pragma HLS interface m_axi port=out2			offset=slave bundle=gmem3 depth=4096

#pragma HLS interface m_axi port=wts			offset=slave bundle=gmem4 depth=36

#pragma HLS interface m_axi port=scalar_pool_args	offset=slave bundle=gmem5 depth=32
#pragma HLS interface s_axilite port=in1		bundle=control
#pragma HLS interface s_axilite port=out1 		bundle=control
#pragma HLS interface s_axilite port=in2		bundle=control
#pragma HLS interface s_axilite port=out2 		bundle=control

#pragma HLS interface s_axilite port=wts 		bundle=control



#pragma HLS interface s_axilite port=scalar_pool_args 	bundle=control
#pragma HLS interface s_axilite port=return 	bundle=control
#endif

	short in_h        	= (short)scalar_pool_args[0];
	short in_w        	= (short)scalar_pool_args[1];
	short out_h       	= (short)scalar_pool_args[2];
	short out_w      	= (short)scalar_pool_args[3];
	short n_planes    	= (short)scalar_pool_args[4];
	short ps_h	  	    = (short)scalar_pool_args[5];
	short ps_w	  	    = (short)scalar_pool_args[6];
	short pwin_h	  	= (short)scalar_pool_args[7];
	short pwin_w	  	= (short)scalar_pool_args[8];
	unsigned char avg_pool	= (unsigned char)scalar_pool_args[9];
	unsigned char pad	    = (unsigned char)scalar_pool_args[10];
	unsigned char one_by_diviser	= (unsigned char)scalar_pool_args[11];
	unsigned char conv3ds	= (unsigned char)scalar_pool_args[12];
	unsigned char relu		= (unsigned char)scalar_pool_args[13];
	unsigned char outshift	= (unsigned char)scalar_pool_args[14];

#if DEBUG
	fprintf(stderr, "pool-ker args : ");
	for(int i = 0; i < 12; i++)
	{
		fprintf(stderr, "%d ", scalar_pool_args[i]);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "maxp-args : ");

	fprintf(stderr, "%d %d %d %d %d %d %d %d %d %d %d %d\n",  in_h,  in_w,  out_h,   out_w,   n_planes,
			ps_h,  ps_w,  pwin_h,  pwin_w, avg_pool,
			pad,  one_by_diviser);
#endif

	//# Call Kernel
	poolKernel(in1, in2, out1, out2, wts, in_h, in_w, out_h, out_w, n_planes, ps_h, ps_w, pwin_h, pwin_w, avg_pool, pad, one_by_diviser,
			conv3ds, relu, outshift);
	#ifndef __SDSOC
	return 0;
	#endif
}



class memData
{

public:
	GMEM_MAXPOOLTYPE *MEMPOINTER;
	GMEM_MAXPOOLTYPE read(size_t address)
	{
#pragma HLS interface m_axi port=MEMPOINTER      	depth=115200
#pragma HLS inline
		return MEMPOINTER[address];
	}

	void write(size_t address,GMEM_MAXPOOLTYPE val )
	{
#pragma HLS interface m_axi port=MEMPOINTER      	depth=115200
#pragma HLS inline
		MEMPOINTER[address]=val;
	}
	memData(GMEM_MAXPOOLTYPE *MEMPOINTER):MEMPOINTER(MEMPOINTER) {};
};


class streamData
{

public:
	hls::stream< GMEM_MAXPOOLTYPE > & streamIn;
	ap_uint<32> read(ap_uint<32> address)
	{
#pragma HLS inline
		return streamIn.read();
	}

	void write(ap_uint<32> address,GMEM_MAXPOOLTYPE val )
	{
#pragma HLS inline
		streamIn<<val;
	}
	streamData(hls::stream< GMEM_MAXPOOLTYPE > & streamIn) : streamIn(streamIn) {};
};


#ifndef __SDSOC
int PoolTop
#else
void PoolTop
#endif
(
	#if STREAMINTERFACE
    hls::stream<GMEM_MAXPOOLTYPE> &in1, 
	hls::stream<GMEM_MAXPOOLTYPE> &in2, 
	hls::stream<GMEM_MAXPOOLTYPE> &out1, 
	hls::stream<GMEM_MAXPOOLTYPE> &out2,
	#else 
	GMEM_MAXPOOLTYPE *in1,
	GMEM_MAXPOOLTYPE *in2,
	GMEM_MAXPOOLTYPE *out1,
	GMEM_MAXPOOLTYPE *out2,
	#endif	
	GMEM_POOLTYPE_WTS * wts,
	int *scalar_pool_args
)

{

#ifndef __SDSOC
#pragma HLS interface m_axi port=in1       		offset=slave bundle=gmem0 depth=18150
#pragma HLS interface m_axi port=out1			offset=slave bundle=gmem1 depth=18150
#pragma HLS interface m_axi port=in2       		offset=slave bundle=gmem2 depth=18150
#pragma HLS interface m_axi port=out2			offset=slave bundle=gmem3 depth=18150
#pragma HLS interface m_axi port=wts			offset=slave bundle=gmem4 depth=36
#pragma HLS interface m_axi port=scalar_pool_args	offset=slave bundle=gmem5 depth=32
#pragma HLS interface s_axilite port=in1		bundle=control
#pragma HLS interface s_axilite port=out1 		bundle=control
#pragma HLS interface s_axilite port=in2		bundle=control
#pragma HLS interface s_axilite port=out2 		bundle=control
#pragma HLS interface s_axilite port=wts 		bundle=control
#pragma HLS interface s_axilite port=scalar_pool_args 	bundle=control
#pragma HLS interface s_axilite port=return 	bundle=control
#endif

hls::stream< GMEM_MAXPOOLTYPE > inStreamEnd1("inStreamEnd1");
hls::stream< GMEM_MAXPOOLTYPE > inStreamEnd2("inStreamEnd2");
hls::stream< GMEM_MAXPOOLTYPE > outStreamEnd1("outStreamEnd1");
hls::stream< GMEM_MAXPOOLTYPE > outStreamEnd2("outStreamEnd2");


PoolingLayerLineBuffer
					(					
					in1,in2,
					inStreamEnd1,
					inStreamEnd2,
					out1,out2,
					outStreamEnd1,
					outStreamEnd2,
					scalar_pool_args);
	#ifndef __SDSOC
	return 0;
	#endif
   
}
