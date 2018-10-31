#include "../include/xi_conv_config.h"


//* batch8_enable off
//* batch8 actually waste a lot of storage space, not idea why now 

template<int BUFFER_LENGTH, int DATAWIDTH, class uInBuffAddr_t, class InData_t, int DPACK>
void readLineBuffer
(
    InData_t inData1,
    #if !XI_SINGLE_IO_PORT_EN
    InData_t inData2,
    #endif
    ap_int<DATAWIDTH*8> inLineBuffer[8][BUFFER_LENGTH], // not so sure about why this is 8., maybe it some times may inccur 
    sRowIdx_t startRowIdx,
    sRowIdx_t endRowIdx,
    uRowIdx_t inWidth,
    uPixelIdx_t imageSize,
    uPlnPackIdx_t packNum,
    uPixelIdx_t widthxPacknum
)
{ 
    uRowIdx_t startRowSat;
    if(startRow<0) startRowSat=0;
    else startRowSat= (uRowIdx_t ) startRow;

    uRowIdx_t endRowSat;
    if(endRow> inHeight)  endRowSat= (uRowIdx_t)inHeight;
    else endRowSat= (uRowIdx_t) endRow;

    uPixelIdx_t pixelsTotal;


    if(startRowSat > endRowSat)
        pixelsTotal =  0;
    else
        pixelsTotal = (endRowSat-startRowSat)* inWidth;

    ap_uint<16> ddrAddressOffset=startRowSat*inWidth;
    ap_uint<32>  bufferAddrOffset=startRowSat*widthxPack;

    #if !XI_SINGLE_IO_PORT_EN
	ap_uint<1> switch4=0;
    #else 
    ap_uint<2> switch4=0;
    #endif
    
    
    for(uPlnIdx_t planePackIdx =0; planePackIdx<packNum; planePackIdx++)
    {
        inLineBuffer_t bufferAddr=bufferAddrOffset;
        ap_uint<32> ddrAddr=ddrAddressOffset;
        for(ap_uint<16> pixelIdx=0; pixelIdx < pixelsTotal; pixelIdx++, ddrAddr, bufferAddr+=packNum)
        {

            #if !XI_SINGLE_IO_PORT_EN
            #pragma HLS pipeline
            #pragma HLS dependence variable=inLineBuffer intra false
			#pragma HLS dependence variable=inLineBuffer inter false

            gmem_inputtype_layerx readPortA, readPortB;
            readPortA=inData1.read(ddrAddress);
            readPortB=inData2.read(ddrAddress);
			inLineBuffer[switch4*4+0][bufferAddr] = readPortA.range(63,0);
			inLineBuffer[switch4*4+2][bufferAddr] = readPortB.range(63, 0);
			inLineBuffer[switch4*4+1][bufferAddr] = readPortA.range(127, 64);
			inLineBuffer[switch4*4+3][bufferAddr] = readPortB.range(127, 64);
            //* if the output is using same starting address input Memory address,then it shall computes 2 different input differently
            #else //XI_SINGLE_IO_PORT_EN
            inLineBuffer[switch4*2+0][bufferAddr] = readPortA.range(63,0);
            inLineBuffer[switch4*2+1][bufferAddr] = readPortA.range(127,0);
            #endif
        }
        #if !XI_SINGLE_IO_PORT_EN
        if(switch4)
        {
            bufferAddrOffset++;
        }
        #else 
        bufferAddrOffset++;
        #endif
        switch4++;
        ddrAddressOffset+=imageSize;
    }    
}


