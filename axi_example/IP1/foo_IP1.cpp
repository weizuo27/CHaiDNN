#include "head_ip1.hpp"
//#include "/opt/Xilinx/Vivado/2018.2/include/gmp.h"



void load(int startAddr, int endAddr, uint buff_in[IBUFFERSIZE], uint* in)
{
#pragma HLS inline off
#pragma HLS INTERFACE m_axi depth=2048 port=in
	for(int i=startAddr;i<endAddr;i++)
	{
#pragma HLS pipeline
		inBufferAddr_t addr=i;
		buff_in[addr]=in[i];
	}
}

void compute(int startAddr, int endAddr, uint buff_in[IBUFFERSIZE], uint buff_out[OBUFFERSIZE])
{
#pragma HLS inline off
	for(int j=startAddr; j < endAddr; j++){
#pragma HLS pipeline
			inBufferAddr_t addr1=j;
			inBufferAddr_t addr2=j+N;
			inBufferAddr_t addr3=j+2*N;
			outBufferAddr_t oaddr=j;
			buff_out[oaddr] = buff_in[addr1] + buff_in[addr2] + buff_in[addr3];
		}
}

void write(int startAddr, int endAddr, uint buff_out[OBUFFERSIZE],AXI_STREAM &out, ap_uint<5> destID)
{
	#pragma HLS inline off
	AXI_VALUE tmp;
	tmp.dest = destID;
	int j=startAddr;
	for(int  i= 0; i < endAddr-startAddr;i++, j++){
	#pragma HLS INTERFACE axis register both port=out
#pragma HLS pipeline
		outBufferAddr_t oaddr=j;
		tmp.data = buff_out[oaddr];
		out.write(tmp);
	}
}



void foo_IP1 ( uint *in, AXI_STREAM &out, int *dest_out) {

#pragma HLS INTERFACE m_axi depth=100 port=dest_out
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE m_axi depth=2048 port=in

	uint buff_in[IBUFFERSIZE];
#pragma HLS resource variable=buff_in core=RAM_S2P_BRAM

	uint buff_out[OBUFFERSIZE];
#pragma HLS resource variable=buff_out core=RAM_S2P_BRAM

	AXI_VALUE tmp;
	ap_uint<5> destID = dest_out[0];

	int i, j;
	//Initial read
	int startReadAddr=0;
	int endReadAddr=3*N;

	load(0,3*N,buff_in,in);
	startReadAddr+=N;
	endReadAddr+=N;


	int startComputeAddr=0;
	int endComputeAddr=N;


	int writeStartAddr=0;
	int writeEndAddr=0;




	LOOP_CNT:for (i = 0; i < N-2; i++) {
#pragma HLS dependence variable=buff_out intra false
#pragma HLS dependence variable=buff_in intra false


	compute(startComputeAddr,endComputeAddr,buff_in,buff_out);
	startComputeAddr+=N;
	endComputeAddr+=N;

	write(writeStartAddr,writeEndAddr,buff_out,out, destID);
	if(writeEndAddr!=0) writeStartAddr+=N;
	writeEndAddr+=N;


	load(startReadAddr,endReadAddr,buff_in,in);
	if(startReadAddr!=(N-2)*N) endReadAddr+=N;
	startReadAddr+=N;
	}
	write(writeStartAddr,writeEndAddr,buff_out,out, destID);


}


