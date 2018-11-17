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

#include "IP217_config.h"
#include "IP217.hpp"

#define DPACK 16

namespace IP217
{
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
    bool bypass=scalar_pool_args[21];
    int bypassRun=scalar_pool_args[22];

    sPlnPackIdx_t planePackNum;
    if(n_planes%16)
        planePackNum= (n_planes>>4)+1;
    else
        planePackNum= n_planes>>4;


    sRowIdx_t readStartRow= 0;
    sRowIdx_t inStartRow=-pad;

    uRowIdx_t inRowStep= rowStep*ps_h;
    
   
    if(bypass)
    {
        
        bypassFunc<DPACK>(
        inStream1,
        inStream2,
        outStream1,
        outStream2,
        bypassRun
        );
    }
    else
    {
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
}

}



