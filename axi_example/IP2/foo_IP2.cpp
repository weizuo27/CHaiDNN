#include "head_ip2.hpp"

void comp(uint buff_out[2][N], uint buff_in[4][N],
		int out_comp_row, int one, int two, int three){
	int j;
	for(j=0; j < N; j++){
	#pragma HLS UNROLL
		buff_out[out_comp_row][j] = buff_in[one][j]
							  + buff_in[two][j] + buff_in[three][j];
	}
}

void read(uint buff_in[4][N], int i, int in_next_row, AXI_STREAM &in, int M){
	int j;
	if(i<M-3){
		for(j = 0; j < N; j++){
#pragma HLS PIPELINE II=1
			buff_in[in_next_row][j] = in.read().data;
		}
	}
}

void write(int i, uint buff_out[2][N], int out_comp_row_write, uint *out){
	int j;
	if (i > 0){
		for(j = 0; j < N; j++){
#pragma HLS PIPELINE II=1
			out[(i-1)*N + j] = buff_out[out_comp_row_write][j];
		}
	}
}

void foo_IP2 ( AXI_STREAM &in, uint* out, int* args) {
#pragma HLS INTERFACE m_axi depth=100 port=args
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE m_axi depth=896 port=out

	uint buff_in[4][N];
#pragma HLS RESOURCE variable=buff_in core=RAM_S2P_BRAM
	uint buff_out[2][N];
#pragma HLS RESOURCE variable=buff_out core=RAM_S2P_BRAM
	AXI_VALUE tmp;

    int M = args[0];
	int i, j;
	//Initial read
	for(i =0; i < 3; i++){
#pragma HLS PIPELINE II=1
		for(j = 0; j < N; j++){
			buff_in[i][j] = in.read().data;
		}
	}

	int in_next_row = 3;
	int one = 0;
	int two = 1;
	int three = 2;
	int out_comp_row =  0;

	for (i = 0; i < M-2; i++) {
#pragma HLS DEPENDENCE variable=buff_in intra false
#pragma HLS DEPENDENCE variable=buff_out intra false
		//Compute
		 comp(buff_out, buff_in, out_comp_row, one, two, three);

		//read
		read(buff_in, i, in_next_row,in, M);

		in_next_row = (in_next_row == 3) ? 0 : (in_next_row +1);

		one = (one == 3) ? 0 : one+1;
		two = (two == 3) ? 0 :two+1;
		three = (three == 3) ? 0 : three + 1;
		int out_comp_row_write = out_comp_row == 1 ? 0 : 1;

		out_comp_row = (out_comp_row == 1) ? 0 :1;
		//Write
		write(i, buff_out, out_comp_row_write, out);
	}

	int out_comp_row_write = (out_comp_row == 1) ? 0 : 1;
	//Final write
	for(j = 0; j < N; j++){
	#pragma HLS PIPELINE
		out[(i-1)*N+j] = buff_out[out_comp_row_write][j];
	}
}

