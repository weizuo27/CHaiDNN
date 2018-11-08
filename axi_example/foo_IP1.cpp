#include "head.hpp"
//#include "/opt/Xilinx/Vivado/2018.2/include/gmp.h"

void foo_IP1 ( uint *in, AXI_STREAM &out, int *dest_out) {
#pragma HLS INTERFACE m_axi depth=100 port=dest_out
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE m_axi depth=2048 port=in

	uint buff_in[4][N];
	uint buff_out[2][N];
	AXI_VALUE tmp;
	tmp.dest = dest_out[0];

	int i, j;
	//Initial read
	for(i =0; i < 3; i++){
		for(j = 0; j < N; j++){
			buff_in[i][j] = in[i*N + j];
		}
	}


	int in_next_row = 3;
	int one = 0;
	int two = 1;
	int three = 2;
	int out_comp_row =  0;

	for (i = 0; i < N-2; i++) {
		//Compute
		for(j=0; j < N; j++){
			buff_out[out_comp_row][j] = buff_in[one][j]
									  + buff_in[two][j] + buff_in[three][j];
		}

		//read
		if(i<N){
			for(j = 0; j < N; j++){
				buff_in[in_next_row][j] = in[(i+3)*N + j];
			}
		}
/*
		if(i == 1){
			printf("in_next_row %d,one %d, two %d, three %d\n", in_next_row, one, two, three);
			printf("buff_in[%d][0] = %d, buff_in[%d][0]=%d, buff_in[%d][0]=%d\n",
						one, buff_in[one][0], two, buff_in[two][0], three, buff_in[three][0]);
			printf("buff_out[%d][0], %d\n", out_comp_row, buff_out[out_comp_row][0]);
		}
		*/

		in_next_row = (in_next_row == 3) ? 0 : (in_next_row +1);
		one = (one == 3) ? 0 : one+1;
		two = (two == 3) ? 0 :two+1;
		three = (three == 3) ? 0 : three + 1;

		int out_comp_row_write = out_comp_row == 1 ? 0 : 1;
		out_comp_row = (out_comp_row == 1) ? 0 :1;

		//Write
		if (i > 0){
			for(j = 0; j < N; j++){
				tmp.data = buff_out[out_comp_row_write][j];
				out.write(tmp);
			}
		}


	}

	int out_comp_row_write = out_comp_row == 1 ? 0 : 1;
	//Final write
	for(j = 0; j < N; j++){
		tmp.data = buff_out[out_comp_row_write][j];
		out.write(tmp);
	}
}


