#ifndef HW_KERNEL_INFO
#define HW_KERNEL_INFO

#define IN_LINE_BUFFER_SIZE 1024
#include <ap_int.h>
typedef ap_uint<10> inLineBuffAddr_t;

#define OUT_LINE_BUFFER_SIZE 512
typedef ap_uint<9> outLineBuffAddr_t;
//*****************LINE BUFFER CONFIGURATIONS END **********************//

//*****************INDEX DATATYPE SPECIFICATION**********************//
//*change them to larger one if the dimensions are larger
typedef ap_uint<16> uPixelIdx_t;
typedef ap_int<17> 	sPixelIdx_t;
typedef ap_uint<8> 	uRowIdx_t; 
typedef ap_int<9> 	sRowIdx_t;
typedef ap_uint<16> uPlnIdx_t;
typedef ap_int<8> 	sPlnPackIdx_t;
typedef ap_uint<4>  uDimPool_t;
typedef ap_uint<8> 	uSizePool_t;

#endif
