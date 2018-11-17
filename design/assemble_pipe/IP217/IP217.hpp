#include "IP217_config.h"
#include <ap_int.h>


//#include <dsp_builtins.h>


#define PRINT_INPUT_LINEBUFFER 0
#define PRINT_OUTPUT_LINEBUFFER 0
#define PRINT_INPUT_INDEX 0
#define DBG_INFO 0
#define PRINT_OUTPUT_DDR_ADDR 0

namespace IP217{

template< int DPACK>
void ReadInBuffer_Pooling(
    GMEM_MAXPOOLTYPE* inMem1,
    GMEM_MAXPOOLTYPE* inMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &inStream2,
    sRowIdx_t startRow,
    sRowIdx_t endRow,
    uPlnIdxPool_t depth, 
    uRowIdx_t inHeight,
    uRowIdx_t inWidth,
    uPixelIdx_t planePackNum,
    ap_uint<32> ddrPlaneStep,
    ap_int<8> lineBuffer1[DPACK][IN_LINE_BUFFER_SIZE],
    ap_int<8> lineBuffer2[DPACK][IN_LINE_BUFFER_SIZE],
    bool inStreamFlag
)
{
    #pragma HLS array_partition variable=lineBuffer1 dim=1 complete
    #pragma HLS array_partition variable=lineBuffer2 dim=1 complete
    inLineBuffAddr_t startRowSat;
    if(startRow<0) startRowSat=0;
    else startRowSat= (inLineBuffAddr_t ) startRow;

    inLineBuffAddr_t endRowSat;
    if(endRow> inHeight)  endRowSat= (inLineBuffAddr_t)inHeight;
    else endRowSat= (inLineBuffAddr_t) endRow;

    inLineBuffAddr_t pixelsTotal;


    if(startRowSat > endRowSat) startRowSat=endRowSat;
 



    for(sRowIdx_t startRowIdx=startRowSat;startRowIdx<endRowSat;startRowIdx++)
    {
        ap_uint<16> ddrAddressOffset=startRowIdx*inWidth;
        inLineBuffAddr_t lineBufferAddressOffset=startRowIdx*inWidth*planePackNum;

        for( uPlnIdxPool_t depthIdx=0; depthIdx < depth; depthIdx+=DPACK )
        {
            ap_uint<16> ddrAddress=ddrAddressOffset;
            inLineBuffAddr_t lineBufferAddress=lineBufferAddressOffset;
            for(ap_uint<16> pixelIdx=0; pixelIdx < inWidth; pixelIdx++,ddrAddress++, lineBufferAddress+=planePackNum)
            {
                #pragma HLS pipeline
                #pragma HLS dependence variable=lineBuffer1 intra false
                #pragma HLS dependence variable=lineBuffer2 intra false

                GMEM_MAXPOOLTYPE temp1, temp2;
                if(inStreamFlag)
                {
                    inStream1>>temp1;
                    inStream2>>temp2;
                }
                else
                {
                    temp1=inMem1[ddrAddress];
                    temp2=inMem2[ddrAddress];
                }
                
                for(int i=0;i<DPACK;i++)
                {
                #pragma HLS unroll
                    lineBuffer1[i][lineBufferAddress]=temp1.range(i*8+7,i*8);
                }
                for(int i=0;i<DPACK;i++)
                {
                #pragma HLS unroll
                    lineBuffer2[i][lineBufferAddress]=temp2.range(i*8+7,i*8);
                }
            }
            ddrAddressOffset+=ddrPlaneStep;
            lineBufferAddressOffset++;
        }
    }
#if PRINT_INPUT_LINEBUFFER
    for(int i=0;i<IN_LINE_BUFFER_SIZE;i++)
    {
        printf("address[%4d][",i);
        for(int j=0;j<16;j++)
        printf("%4d ", (int) lineBuffer1[j][i]);
        printf("]\n");
    }
    #endif
}

//* quick design output lineBuffer mode
template< int DPACK>
void WriteOutBuffer_Pooling(
    GMEM_MAXPOOLTYPE* outMem1,
    GMEM_MAXPOOLTYPE* outMem2,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream1,
    hls::stream< GMEM_MAXPOOLTYPE > &outStream2,
    sRowIdx_t startRow,
    sRowIdx_t endRow,
    sPlnPackIdx_t depth, 
    uRowIdx_t outHeight,
    uRowIdx_t outWidth,
    uPixelIdx_t planePackNum,
    ap_uint<32> ddrPlaneStep,
    ap_int<8> lineBuffer1[DPACK][OUT_LINE_BUFFER_SIZE],
    ap_int<8> lineBuffer2[DPACK][OUT_LINE_BUFFER_SIZE],
    bool outStreamFlag
)
{
    #pragma HLS array_partition variable=lineBuffer1 dim=1 complete
    #pragma HLS array_partition variable=lineBuffer2 dim=1 complete
    uRowIdx_t startRowSat;
    if(startRow<0) startRowSat=0;
    else startRowSat= (uRowIdx_t) startRow;

    uRowIdx_t endRowSat;
    if( endRow> outHeight)  endRowSat= (uRowIdx_t) outHeight;
    else endRowSat= (uRowIdx_t) endRow;

    uPixelIdx_t pixelsTotal;

    if(startRowSat > endRowSat)  
        pixelsTotal =  0;
    else
        pixelsTotal = (endRowSat-startRowSat)* outWidth;

    ap_uint<32> ddrAddressOffset=startRowSat*outWidth;

    ap_uint<32> lineBufferAddressOffset=startRowSat*outWidth*planePackNum; // it is a rounding address

    
     

    for( uPlnIdxPool_t depthIdx=0; depthIdx < depth; depthIdx+=DPACK ) 
    {
        ap_uint<32> ddrAddress=ddrAddressOffset;
        outLineBuffAddr_t lineBufferAddress=lineBufferAddressOffset;
      
        for(ap_uint<16> pixelIdx=0; pixelIdx < pixelsTotal; pixelIdx++,ddrAddress++, lineBufferAddress+=planePackNum )
        {
            #pragma HLS pipeline
            #pragma HLS dependence variable=lineBuffer1 intra false
			#pragma HLS dependence variable=lineBuffer2 intra false
            GMEM_MAXPOOLTYPE temp1, temp2;

           
            for(int i=0;i<DPACK;i++)
            {
                temp1.range(i*8+7,i*8)=lineBuffer1[i][lineBufferAddress];
            }
            for(int i=0;i<DPACK;i++)
            {
                temp2.range(i*8+7,i*8)=lineBuffer2[i][lineBufferAddress];
            }
            #if PRINT_OUTPUT_DDR_ADDR
                printf("Addr:%6d, Pix[%2d %2d], PlanIdx%2d\n",
                (int) ddrAddress, 
                (int) (ddrAddress%(outWidth*outHeight)/outWidth),
                (int) (ddrAddress%(outWidth*outHeight)%outWidth),
                (int) (ddrAddress/(outWidth*outHeight)) );
            #endif
            if(outStreamFlag)
            {
                outStream1<<temp1;
                outStream2<<temp2;
            }           
            else
            {
                outMem1[ddrAddress]=temp1;
                outMem2[ddrAddress]=temp2;
            }
        }
        ddrAddressOffset+=ddrPlaneStep;
        lineBufferAddressOffset++;
    }

    #if PRINT_OUTPUT_LINEBUFFER
    for(int i=0;i<512;i++)
    {
        printf("out[%4d][",i);
        for(int j=0;j<16;j++)
        printf("%4d ", (int) lineBuffer1[j][i]);
        printf("]\n");
    }
    #endif
}

template< int DPACK> 
void bypassFunc(
    hls::stream< ap_uint<128> > & inStream1,
    hls::stream< ap_uint<128> > & inStream2,
    hls::stream< ap_uint<128> > & outStream1,
    hls::stream< ap_uint<128> > & outStream2,
    int bypassRun
)
{
    printf("bypassing Number: %d\n", bypassRun);
    for(int i=0;i<bypassRun;i++)
        {
        #pragma HLS pipeline
            ap_uint<128> temp1;
            ap_uint<128> temp2;
            inStream1>>temp1;
            inStream2>>temp2;
            outStream1<<temp1;
            outStream2<<temp2;
        }
}

//* this one is designed specifically for row by row output
template<int DPACK>
void ProcessPoolingRow(
ap_int<8> inBuffer1[DPACK][IN_LINE_BUFFER_SIZE],
ap_int<8> inBuffer2[DPACK][IN_LINE_BUFFER_SIZE],
ap_int<8> outBuffer1[DPACK][OUT_LINE_BUFFER_SIZE],
ap_int<8> outBuffer2[DPACK][OUT_LINE_BUFFER_SIZE],
uRowIdx_t outStartRow,
uRowIdx_t outEndRow,
sRowIdx_t inStartRow,
uRowIdx_t outWidth,
uRowIdx_t outHeight,
uRowIdx_t inWidth,
uRowIdx_t inHeight,
uPlnIdxPool_t planes,
uDimPool_t poolWinH,
uDimPool_t poolWinW,
uDimPool_t poolStrideH,
uDimPool_t poolStrideW,
ap_uint<8> oneDivisor,
ap_uint<5> outShift,
ap_int<5> wPad,
bool avgPooling,
uPixelIdx_t planePackNum
)
{
    #pragma HLS array_partition variable=inBUffer1 dim=1 complete
    #pragma HLS array_partition variable=inBUffer2 dim=1 complete
    #pragma HLS array_partition variable=outBUffer1 dim=1 complete    
    #pragma HLS array_partition variable=outBUffer2 dim=1 complete




    uRowIdx_t outStartRowSat;
    if(outStartRow<0)outStartRowSat=0;
    else outStartRowSat= (uRowIdx_t) outStartRow;

    uRowIdx_t outEndRowSat;
    if(outEndRow> outHeight)  outEndRowSat= (uRowIdx_t)outHeight;
    else outEndRowSat= (uRowIdx_t) outEndRow;

    uPixelIdx_t pixelsTotal;

    if(outStartRowSat > outEndRowSat)
        pixelsTotal =  0;
    else
        pixelsTotal = (outEndRowSat-outStartRowSat)* outWidth;

    ap_uint<32> outAddressPlaneOffset=outStartRowSat*outWidth*planePackNum;
    ap_int<32>  inAddressPlaneOffset=0; // must be int here since it could be negative


    
    uSizePool_t poolWinSize= poolWinH*poolWinW;
    ap_uint<32> compNumberTotal= pixelsTotal*poolWinSize+1;

       

    ap_int<8> temp1[DPACK];
#pragma HLS ARRAY_PARTITION variable=temp1 complete dim=1
    ap_int<8> temp2[DPACK];
#pragma HLS ARRAY_PARTITION variable=temp2 complete dim=1
    ap_int<8> max1[DPACK];
#pragma HLS ARRAY_PARTITION variable=max1 complete dim=1
    ap_int<8> max2[DPACK];
#pragma HLS ARRAY_PARTITION variable=max2 complete dim=1
    ap_int<24> sum1[DPACK];
#pragma HLS ARRAY_PARTITION variable=sum1 complete dim=1
    ap_int<24> sum2[DPACK];
#pragma HLS ARRAY_PARTITION variable=sum2 complete dim=1

    for(uPlnIdxPool_t planeIdx=0; planeIdx<planes; planeIdx+=DPACK)
    {
        ap_int<10> inRowIdx=inStartRow;
        ap_int<10> inColIdx=-wPad;
        ap_int<10> outColIdx=0;
        ap_uint<8> winRowIdx=0, winColIdx=0;
        uSizePool_t poolCnt=0;
        outLineBuffAddr_t outAddr=outAddressPlaneOffset;
 
            for(int i = 0; i < DPACK; i++)
            {
                #pragma HLS unroll
                if(wPad){
                    max1[i] = max2[i] = 0x0;
                } else {
                    max1[i] = max2[i] = 0x80;
                }
                sum1[i] = sum2[i] = 0;

            }
  
        for(ap_uint<16>  compIdx=0; compIdx<compNumberTotal; compIdx++)
        {
            #pragma HLS pipeline
            #pragma HLS dependence variable=inBuffer1 intra false
			#pragma HLS dependence variable=inBuffer1 inter false

			#pragma HLS dependence variable=inBuffer2 intra false
			#pragma HLS dependence variable=inBuffer2 inter false

            #pragma HLS dependence variable=outBuffer1 intra false
			#pragma HLS dependence variable=outBuffer1 inter false

			#pragma HLS dependence variable=outBuffer2 intra false
			#pragma HLS dependence variable=outBuffer2 inter false

            sRowIdx_t inRowComputeIdx= inRowIdx+winRowIdx;
            sRowIdx_t inColComputeIdx= inColIdx+winColIdx;
            sPixelIdx_t inPixelComputeIdx = inRowComputeIdx*inWidth+inColComputeIdx;



            inLineBuffAddr_t inAddr=inAddressPlaneOffset+inPixelComputeIdx*planePackNum;


            
            if(poolCnt==poolWinSize)
            {
                
                for(int i = 0 ; i < DPACK; i++)
                {
                    if(avgPooling)
                    {
                        outBuffer1[i][outAddr] = (sum1[i] >> outShift);
                        outBuffer2[i][outAddr] = (sum2[i] >> outShift);
                        if ( inColComputeIdx < 0 || inRowComputeIdx < 0 ){
                            sum1[i] = 0;
                            sum2[i] = 0;
                        } else {
                            sum1[i] = inBuffer1[i][inAddr] * oneDivisor;
                            sum2[i] = inBuffer2[i][inAddr] * oneDivisor;
                        }
                    }
                    else
                    {
                        outBuffer1[i][outAddr] = max1[i];
                        outBuffer2[i][outAddr] = max2[i];
                        if (inColComputeIdx < 0 || inRowComputeIdx < 0){
                            max1[i] = 0;
                            max2[i] = 0;
                        } else {
                            max1[i] = inBuffer1[i][inAddr];
                            max2[i] = inBuffer2[i][inAddr];
                        }
                    }
                }
                #if PRINT_INPUT_INDEX
                printf("[Addr %d][", (int) outAddr);
                for(int i=0;i<DPACK;i++)
                printf("%2d ", (int) outBuffer1[i][outAddr]);
                printf("]\n");
                
                printf("OUT[%2d,%2d %2d][", (int) inRowComputeIdx, (int) inColComputeIdx, (int) inAddr); 
                for(int i=0;i<DPACK;i++)
                printf(" (%2d %2d)",(int) max1[i], (int) inBuffer1[i][inAddr]);
                printf("]\n");
                #endif
                
                outAddr+=planePackNum;
                poolCnt=1;
            }
            else
            {

                #if PRINT_INPUT_INDEX
                printf("INN[%2d,%2d %2d][", (int) inRowComputeIdx, (int) inColComputeIdx , (int) inAddr ); 
                for(int i=0;i<DPACK;i++)
                printf(" (%2d %2d)",(int) max1[i], (int) inBuffer1[i][inAddr]);
                printf("]\n");
                #endif

                if (inColComputeIdx < 0 || inRowComputeIdx < 0 || (inColComputeIdx >= inWidth || inRowComputeIdx >= inHeight) && avgPooling  )  
                {
                    for(int i = 0 ; i < DPACK; i++)
                    {
                        temp1[i] = 0;
                        temp2[i] = 0;
                    }
                } 
                else if ( inColComputeIdx >= inWidth || inRowComputeIdx >= inHeight ) 
                {
                    for(int i = 0 ; i < DPACK; i++)
                    {
                        temp1[i] =0x80;
                        temp2[i] =0x80;
                    }
                }
                else
                {
                    for(int i = 0 ; i < DPACK; i++)
                    {
                        temp1[i] = inBuffer1[i][inAddr];
                        temp2[i] = inBuffer2[i][inAddr];
                    }
                }
                
                for(int i = 0 ; i <DPACK; i++)
			    {
					sum1[i] += (temp1[i]*oneDivisor);
					sum2[i] += (temp2[i]*oneDivisor);
					max1[i] = max1[i] > temp1[i] ? max1[i] : temp1[i];
					max2[i] = max2[i] > temp2[i] ? max2[i] : temp2[i];
			    }
                poolCnt++;
		    }
// can be put to the end;
            bool winColDone= (winColIdx==poolWinW-1);
            bool winRowDone= (winRowIdx==poolWinH-1);
            bool rowEndDone= (outColIdx==outHeight-1);
            
            if(winColDone) winColIdx=0;
            else winColIdx++;

            if(winColDone && winRowDone) winRowIdx=0;
            else if(winColDone) winRowIdx++;

            if(winColDone && winRowDone && rowEndDone)
            {
                inRowIdx+=poolStrideH;
                inColIdx=-wPad;
                outColIdx=0;
            }
            else if( winColDone && winRowDone)
            {
                inColIdx+=poolStrideW;
                outColIdx++;
            }  
            #if PRINT_INPUT_INDEX
            if(winColDone && winRowDone)
            printf("\n");
            #endif                      
        }
        outAddressPlaneOffset++;
        inAddressPlaneOffset++;
    }
}









// template<class in_data_t, class out_data_t, int DPACK>
// void PoolingLayerLineBuffer
// (
//     GMEM_MAXPOOLTYPE* inMem1,
//     GMEM_MAXPOOLTYPE* inMem2,
//     hls::stream< GMEM_MAXPOOLTYPE > &inStream1,
//     hls::stream< GMEM_MAXPOOLTYPE > &inStream2,
//     GMEM_MAXPOOLTYPE* outMem1,
//     GMEM_MAXPOOLTYPE* outMem2,
//     hls::stream< GMEM_MAXPOOLTYPE > &outStream1,
//     hls::stream< GMEM_MAXPOOLTYPE > &outStream2,
//     int* scalarPoolArgs
// )
// {


//     ap_int<8> outBuffer1[DPACK][OUT_LINE_BUFFER_SIZE];
//     ap_int<8> outBuffer2[DPACK][OUT_LINE_BUFFER_SIZE];

// #pragma HLS array_partition variable=outBuffer1 dim=1 complete
// #pragma HLS array_partition variable=outBuffer2 dim=1 complete

// #pragma HLS resource variable=ourBuffer1 core=RAM_S2P_BRAM
// #pragma HLS resource variable=ourBuffer2 core=RAM_S2P_BRAM

//     ap_int<8> inBuffer1[DPACK][IN_LINE_BUFFER_SIZE];
//     ap_int<8> inBuffer2[DPACK][IN_LINE_BUFFER_SIZE];

// #pragma HLS array_partition variable=inBuffer1 dim=1 complete
// #pragma HLS array_partition variable=inBuffer2 dim=1 complete
// #pragma HLS resource variable=inBuffer1 core=RAM_S2P_BRAM
// #pragma HLS resource variable=inBuffer2 core=RAM_S2P_BRAM

//     int scalar_pool_args[32];
//     for(int i=0;i<32;i++)
//     {
//         scalar_pool_args[i]=scalarPoolArgs[i];
//     }

//     uRowIdx_t in_h        	= (uRowIdx_t)scalar_pool_args[0];
// 	uRowIdx_t in_w        	= (short)scalar_pool_args[1];
// 	uRowIdx_t out_h       	= (short)scalar_pool_args[2];
// 	uRowIdx_t out_w      	= (short)scalar_pool_args[3];
// 	uPlnIdxPool_t n_planes    	= (short)scalar_pool_args[4];
// 	uDimPool_t ps_h	  	    = (short)scalar_pool_args[5];
// 	uDimPool_t ps_w	  	    = (short)scalar_pool_args[6];
// 	uDimPool_t pwin_h	  	= (short)scalar_pool_args[7];
// 	uDimPool_t pwin_w	  	= (short)scalar_pool_args[8];
// 	bool avg_pool	= (unsigned char)scalar_pool_args[9];
// 	uDimPool_t pad	    = (unsigned char)scalar_pool_args[10];
// 	ap_uint<8> one_by_diviser	= (unsigned char)scalar_pool_args[11];
// 	bool conv3ds	= (bool)scalar_pool_args[12];
// 	bool relu		= (bool)scalar_pool_args[13];
// 	ap_uint<5> outshift	= (ap_uint<5>)scalar_pool_args[14];
//     uRowIdx_t rowStep = (uRowIdx_t) scalar_pool_args[15];
//     uRowIdx_t initialReadRows = (uRowIdx_t) scalar_pool_args[16];
//     ap_uint<32> inDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[17];
//     ap_uint<32> outDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[18];

//     bool inStreamFlag= scalar_pool_args[19];
//     bool outStreamFlag= scalar_pool_args[20];

//     sPlnPackIdx_t planePackNum;
//     if(n_planes%16)
//         planePackNum= (n_planes>>4)+1;
//     else
//         planePackNum= n_planes>>4;


//     sRowIdx_t readStartRow= 0;
//     sRowIdx_t inStartRow=-pad;

//     uRowIdx_t inRowStep= rowStep*ps_h;
    
   

//     //** first tiling readin*//
//     ReadInBuffer_Pooling<16>
//     (
//         inMem1,
//         inMem2,
//         inStream1,
//         inStream2,
//         readStartRow,
//         readStartRow+initialReadRows,
//         n_planes,
//         in_h,
//         in_w,
//         planePackNum,
//         inDDRPlaneStep,
//         inBuffer1,
//         inBuffer2,
//         inStreamFlag
//     );
//     readStartRow+=initialReadRows;

//     sRowIdx_t writeStartRow=-rowStep;

//     for(uRowIdx_t outStartRow=0, outEndRow=rowStep; outStartRow<out_h;
//             outStartRow+=rowStep)
//     {
// #pragma HLS dependence variable=inBuffer2 intra false
// #pragma HLS dependence variable=inBuffer1 intra false
// #pragma HLS dependence variable=outBuffer1 intra false
// #pragma HLS dependence variable=outBuffer2 intra false
        
//         ProcessPoolingRow<DPACK>(
//         inBuffer1,inBuffer2,
//         outBuffer1,outBuffer2,
//         outStartRow,
//         outStartRow+rowStep,
//         inStartRow,
//         out_w,
//         out_h,
//         in_w,
//         in_h,
//         n_planes,
//         pwin_h,
//         pwin_w,
//         ps_h,
//         ps_w,
//         one_by_diviser,
//         outshift,
//         pad,
//         avg_pool,
//         planePackNum
//         );
//         inStartRow+=inRowStep;


//         WriteOutBuffer_Pooling<DPACK>
//         (
//             outMem1,outMem2,
//             outStream1,
//             outStream2,
//             writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
//             planePackNum,outDDRPlaneStep,outBuffer1,outBuffer2,
//             outStreamFlag
//         );
//         writeStartRow+=rowStep;
            
//         ReadInBuffer_Pooling<DPACK>
//         (
//         inMem1,
//         inMem2,
//         inStream1,
//         inStream2,
//             readStartRow,
//             readStartRow+inRowStep,
//             n_planes,
//             in_h,
//             in_w,
//             planePackNum,
//             inDDRPlaneStep,
//             inBuffer1,
//             inBuffer2,
//             inStreamFlag
//         );    
//         readStartRow+=inRowStep;
//     }

//     WriteOutBuffer_Pooling<DPACK>
//     (
        
//         outMem1,outMem2,
//         outStream1,
//         outStream2,
//         writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
//         planePackNum,outDDRPlaneStep,outBuffer1,outBuffer2,
//         outStreamFlag
//     );
// }




}

#undef DBG_INFO





