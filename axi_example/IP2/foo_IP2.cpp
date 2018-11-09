#include "head_ip2.hpp"

void foo_IP2 ( AXI_STREAM &in, uint* out) {
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE m_axi depth=896 port=out

	uint buff_in[4][N];
	uint buff_out[2][N];
	AXI_VALUE tmp;

	int i, j;
	//Initial read
	for(i =0; i < 3; i++){
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
		//Compute
		for(j=0; j < N; j++){
			buff_out[out_comp_row][j] = buff_in[one][j]
									  + buff_in[two][j] + buff_in[three][j];
		}

		//read
		if(i<M-3){
			for(j = 0; j < N; j++){
				buff_in[in_next_row][j] = in.read().data;
				//printf ("read in %d line: %d\n", i+3, buff_in[in_next_row][j]);
			}
		}

        /*
		if(i == 0){
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
				//printf("out addr %d\n", (i-1)*N + j);
				out[(i-1)*N + j] = buff_out[out_comp_row_write][j];
			}
		}

	}

	int out_comp_row_write = (out_comp_row == 1) ? 0 : 1;
	//Final write
	for(j = 0; j < N; j++){
		out[(i-1)*N+j] = buff_out[out_comp_row_write][j];
	}

}


