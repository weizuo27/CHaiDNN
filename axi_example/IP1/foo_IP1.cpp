#include "head_ip1.hpp"

void comp1(uint buff_out[2][N],uint buff_in[4][N], int out_comp_row, int one, int two, int three){
	int j;
	for(j=0; j < N; j++){
	#pragma HLS UNROLL
			buff_out[out_comp_row][j] = buff_in[one][j]
									  + buff_in[two][j] + buff_in[three][j];
	}
}
void read1(uint buff_in[4][N], int i, int in_next_row, uint *in, int M){
	int j;
if(i<M-3){
	for(j = 0; j < N; j++){
#pragma HLS PIPELINE II=1
		buff_in[in_next_row][j] = in[(i+3)*N + j];
	}
}
}

void write1(int i, AXI_VALUE & tmp, uint buff_out[2][N], int out_comp_row_write, AXI_STREAM &out){
//void write(int i, AXI_VALUE & tmp, uint buff_out[2][N], int out_comp_row_write, AXI_STREAM &out){
	int j;
	if (i > 0){
				for(j = 0; j < N; j++){
	#pragma HLS PIPELINE
					tmp.data = buff_out[out_comp_row_write][j];
					out.write(tmp);
				}
			}

}
void foo_IP1 ( uint *in, AXI_STREAM &out, int *dest_out) {

	uint buff_in[4][N];
#pragma HLS RESOURCE variable=buff_in core=RAM_S2P_BRAM
	uint buff_out[2][N];
#pragma HLS RESOURCE variable=buff_out core=RAM_S2P_BRAM
	AXI_VALUE tmp;
    int M = args[0];

	int i, j;
	//Initial read
	foo_IP1_label0:for(i =0; i < 3; i++){
#pragma HLS PIPELINE II=1
		for(j = 0; j < N; j++){
			buff_in[i][j] = in[i*N + j];
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
		comp1(buff_out,buff_in, out_comp_row, one, two, three);

		//read
		read1(buff_in, i,in_next_row,in, M);
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
		write1(i, tmp, buff_out, out_comp_row_write, out);
	}

	int out_comp_row_write = out_comp_row == 1 ? 0 : 1;
	//Final write
	for(j = 0; j < N; j++){
#pragma HLS PIPELINE
		tmp.data = buff_out[out_comp_row_write][j];
		//tmp = buff_out[out_comp_row_write][j];
		out.write(tmp);
	}
	write(writeStartAddr,writeEndAddr,buff_out,out, destID);


}


