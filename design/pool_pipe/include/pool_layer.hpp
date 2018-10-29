#include "maxpool_config.h"
#include <ap_int.h>


#include <dsp_builtins.h>

template<class indata, int DPACK>
void ReadInBuffer_Pooling(
    indata input1,
    indata input2,
    sRowIdx_t startRow,
    sRowIdx_t endRow,
    uPlnIdx_t depth, 
    uRowIdx_t inHeight,
    uRowIdx_t inWidth,
    uPixelIdx_t lineBufferPlaneStep,
    ap_uint<32> ddrPlaneStep,
    ap_int<8> lineBuffer1[DPACK][IN_LINE_BUFFER_SIZE],
    ap_int<8> lineBuffer2[DPACK][IN_LINE_BUFFER_SIZE]
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


    if(startRowSat > endRowSat)
        pixelsTotal =  0;
    else
        pixelsTotal = (endRowSat-startRowSat)* inWidth;

    ap_uint<16> ddrAddressOffset=startRowSat*inWidth;
    inLineBuffAddr_t lineBufferAddressOffset=startRowSat*inWidth;


    for( uPlnIdx_t depthIdx=0; depthIdx < depth; depthIdx+=DPACK )
    {
        ap_uint<16> ddrAddress=ddrAddressOffset;
        inLineBuffAddr_t lineBufferAddress=lineBufferAddressOffset;

        for(ap_uint<16> pixelIdx=0; pixelIdx < pixelsTotal; pixelIdx++,ddrAddress++, lineBufferAddress++ )
        {
            #pragma HLS pipeline
            #pragma HLS dependence variable=lineBuffer1 intra false
            #pragma HLS dependence variable=lineBuffer1 inter false
			#pragma HLS dependence variable=lineBuffer2 intra false
			#pragma HLS dependence variable=lineBuffer2 inter false
            GMEM_MAXPOOLTYPE temp1, temp2;
            temp1=input1.read( ddrAddress);
            temp2=input2.read( ddrAddress);

            for(int i=0;i<DPACK;i++)
            {
                lineBuffer1[i][lineBufferAddress]=temp1.range(i*8+7,i*8);
            }
            for(int i=0;i<DPACK;i++)
            {
                lineBuffer2[i][lineBufferAddress]=temp2.range(i*8+7,i*8);
            }
        }
        ddrAddressOffset+=ddrPlaneStep;
        lineBufferAddressOffset+=lineBufferPlaneStep;
    }

    // for(int i=0;i<512;i++)
    // {
    //     printf("address[%4d][",i);
    //     for(int j=0;j<16;j++)
    //     printf("%4d ", (int) lineBuffer1[j][i]);
    //     printf("]\n");
    // }
}

//* quick design output lineBuffer mode
template<class outdata, int DPACK>
void WriteOutBuffer_Pooling(
    outdata output1,
    outdata output2,
    sRowIdx_t startRow,
    sRowIdx_t endRow,
    sPlnPackIdx_t depth, 
    uRowIdx_t outHeight,
    uRowIdx_t outWidth,
    uPixelIdx_t lineBuffPlaneStep,
    ap_uint<32> ddrPlaneStep,
    ap_int<8> lineBuffer1[DPACK][OUT_LINE_BUFFER_SIZE],
    ap_int<8> lineBuffer2[DPACK][OUT_LINE_BUFFER_SIZE]
)
{
    #pragma HLS array_partition variable=lineBuffer1 dim=1 complete
    #pragma HLS array_partition variable=lineBuffer2 dim=1 complete
    uRowIdx_t startRowSat;
    if(startRow<0) startRowSat=0;
    else startRowSat= (uRowIdx_t) startRow;

    uRowIdx_t endRowSat;
    if( endRow> outHeight)  endRowSat= (uRowIdx_t)outHeight;
    else endRowSat= (uRowIdx_t) endRow;

    uPixelIdx_t pixelsTotal;

    if(startRowSat > endRowSat)
        pixelsTotal =  0;
    else
        pixelsTotal = (endRowSat-startRowSat)* outWidth;
    ap_uint<32> ddrAddressOffset=startRowSat*outWidth;
    ap_uint<32> lineBufferAddressOffset=startRowSat*outWidth; // it is a rounding address
    for( uPlnIdx_t depthIdx=0; depthIdx < depth; depthIdx+=DPACK ) 
    {
        ap_uint<32> ddrAddress=ddrAddressOffset;
        outLineBuffAddr_t lineBufferAddress=lineBufferAddressOffset;
        for(ap_uint<16> pixelIdx=0; pixelIdx < pixelsTotal; pixelIdx++,ddrAddress++, lineBufferAddress++ )
        {
            #pragma HLS pipeline
            #pragma HLS dependence variable=lineBuffer1 intra false
            #pragma HLS dependence variable=lineBuffer1 inter false
			#pragma HLS dependence variable=lineBuffer2 intra false
			#pragma HLS dependence variable=lineBuffer2 inter false
            GMEM_MAXPOOLTYPE temp1, temp2;
            for(int i=0;i<DPACK;i++)
            {
                temp1.range(i*8+7,i*8)=lineBuffer1[i][lineBufferAddress];
            }
            for(int i=0;i<DPACK;i++)
            {
                temp2.range(i*8+7,i*8)=lineBuffer2[i][lineBufferAddress];
            }
            output1.write( ddrAddress,temp1);
            output2.write( ddrAddress,temp2);
        }
        ddrAddressOffset+=ddrPlaneStep;
        lineBufferAddressOffset+=lineBuffPlaneStep;
    }
    // for(int i=0;i<512;i++)
    // {
    //     printf("out[%4d][",i);
    //     for(int j=0;j<16;j++)
    //     printf("%4d ", (int) lineBuffer1[j][i]);
    //     printf("]\n");
    // }
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
uPlnIdx_t planes,
uDimPool_t poolWinH,
uDimPool_t poolWinW,
uDimPool_t poolStrideH,
uDimPool_t poolStrideW,
ap_uint<8> oneDivisor,
ap_uint<5> outShift,
ap_int<5> wPad,
bool avgPooling,
uPixelIdx_t inPlaneStep,
uPixelIdx_t outPlaneStep
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

    ap_uint<32> outAddressPlaneOffset=outStartRowSat*outWidth;
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

    for(uPlnIdx_t planeIdx=0; planeIdx<planes; planeIdx+=DPACK)
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
        #if DBG_INFO
        printf("poolStrideW %d\n", (int) poolStrideH);
        #endif
        for(ap_uint<16>  compIdx=0; compIdx<compNumberTotal; compIdx++)
        {
            #pragma HLS pipeline
            #pragma HLS dependence variable=inBuffer1 intra false
            #pragma HLS dependence variable=inBuffer1 inter false
            #pragma HLS dependence variable=outBuffer1 intra false
            #pragma HLS dependence variable=outBuffer1 inter false


            sRowIdx_t inRowComputeIdx= inRowIdx+winRowIdx;
            sRowIdx_t inColComputeIdx= inColIdx+winColIdx;
            sPixelIdx_t inPixelComputeIdx = inRowComputeIdx*inWidth+inColComputeIdx;



            inLineBuffAddr_t inAddr=inAddressPlaneOffset+inPixelComputeIdx;

            #if DBG_INFO
            printf("[%d,%d, %d] ", (int) inRowComputeIdx, (int) inColComputeIdx, (int) inBuffer1[0][inAddr] ); 
            #endif
            
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
                            max1[i] = inBuffer2[i][inAddr];
                        }
                    }
                }
                
                outAddr++;
                poolCnt=1;
            }
            else
            {

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
					max2[i] = max2[i] > temp1[i] ? max2[i] : temp2[i];
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
            #if DBG_INFO
            if(winColDone && winRowDone)
            printf("\n");
            #endif                      
        }
        outAddressPlaneOffset+=outPlaneStep;
        inAddressPlaneOffset+=inPlaneStep;
    }
}









template<class in_data_t, class out_data_t, int DPACK>
void PoolingLayerLineBuffer
(
    in_data_t inData1,
    in_data_t inData2,
    out_data_t outData1,
    out_data_t outData2,
    int* scalar_pool_args
)
{


    ap_int<8> outBuffer1[DPACK][OUT_LINE_BUFFER_SIZE];
    ap_int<8> outBuffer2[DPACK][OUT_LINE_BUFFER_SIZE];

#pragma HLS array_partition variable=outBuffer1 dim=1 complete
#pragma HLS resource variable=ourBuffer1 core=RAM_S2P_BRAM

#pragma HLS array_partition variable=outBuffer2 dim=1 complete
#pragma HLS resource variable=ourBuffer1 core=RAM_S2P_BRAM

    ap_int<8> inBuffer1[DPACK][IN_LINE_BUFFER_SIZE];
    ap_int<8> inBuffer2[DPACK][IN_LINE_BUFFER_SIZE];

#pragma HLS array_partition variable=inBuffer1 dim=1 complete
#pragma HLS resource variable=inBuffer1 core=RAM_S2P_BRAM

#pragma HLS array_partition variable=inBuffer2 dim=1 complete
#pragma HLS resource variable=inBuffer1 core=RAM_S2P_BRAM
    



    uRowIdx_t in_h        	= (uRowIdx_t)scalar_pool_args[0];
	uRowIdx_t in_w        	= (short)scalar_pool_args[1];
	uRowIdx_t out_h       	= (short)scalar_pool_args[2];
	uRowIdx_t out_w      	= (short)scalar_pool_args[3];
	uPlnIdx_t n_planes    	= (short)scalar_pool_args[4];
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
    uPixelIdx_t inLineBufferPlaneStep =(uPixelIdx_t) scalar_pool_args[17];
    uPixelIdx_t outLineBufferPlaneStep =(uPixelIdx_t) scalar_pool_args[18];
    ap_uint<32> inDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[19];
    ap_uint<32> outDDRPlaneStep= (ap_uint<32> ) scalar_pool_args[20];


    sRowIdx_t readStartRow= 0;
    sRowIdx_t inStartRow=-pad;

    uRowIdx_t inRowStep= rowStep*ps_h;
    
   

    //** first tiling readin*//
    ReadInBuffer_Pooling<in_data_t,16>
    (
        inData1,inData2,
        readStartRow,
        readStartRow+initialReadRows,
        n_planes,
        in_h,
        in_w,
        inLineBufferPlaneStep,
        inDDRPlaneStep,
        inBuffer1,
        inBuffer2
    );
    readStartRow+=initialReadRows;
    puts("forstart Reading");

    sRowIdx_t writeStartRow=-rowStep;

    for(uRowIdx_t outStartRow=0, outEndRow=rowStep; outStartRow<out_h;
            outStartRow+=rowStep)
    {
#pragma HLS dependence variable=inBuffer1 intra false
#pragma HLS dependence variable=inBuffer2 intra false

#pragma HLS dependence variable=outBuffer1 intra false
#pragma HLS dependence variable=outBuffer2 intra false


        printf("%d %d\n",(int) outStartRow,(int) rowStep);
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
        inLineBufferPlaneStep,
        outLineBufferPlaneStep);
        inStartRow+=inRowStep;

        WriteOutBuffer_Pooling<out_data_t,DPACK>
        (
            outData1, outData2,
            writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
            outLineBufferPlaneStep,outDDRPlaneStep,outBuffer1,outBuffer2
        );
        writeStartRow+=rowStep;
            
        ReadInBuffer_Pooling<in_data_t,DPACK>
        (
            inData1,inData2,
            readStartRow,
            readStartRow+inRowStep,
            n_planes,
            in_h,
            in_w,
            inLineBufferPlaneStep,
            inDDRPlaneStep,
            inBuffer1,
            inBuffer2
        );    
        readStartRow+=inRowStep;
    }

    WriteOutBuffer_Pooling<out_data_t,DPACK>
    (
        outData1, outData2,
        writeStartRow,writeStartRow+rowStep, n_planes,out_h,out_w,
        outLineBufferPlaneStep,outDDRPlaneStep,outBuffer1,outBuffer2
    );


    


}












