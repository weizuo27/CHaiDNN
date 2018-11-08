#include "/opt/Xilinx/Vivado/2018.2/include/gmp.h"
#include "head_ip1.hpp"
#include "stdlib.h"
#include "stdio.h"
int main(){
	printf("just started\n");
	uint in_main[N*N];
	AXI_STREAM out;
	uint out_main[(N-2)*N];
	uint out_gold[(N-2)*N];

	int i, j;
	printf("before initialization\n");

	//input initialization
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			in_main[i*N+j] = i;
		}
	}
	printf("after initialization\n");

	//Create reference output
	for(i = 0; i <N-2; i++){
		for(j = 0; j<N; j++){
			out_gold[i*N + j] = in_main[i*N+j]+in_main[(i+1)*N+j]+in_main[(i+2)*N+j];
		}
	}

	int dest_out = 1; //Not useful for one IP simulation

	printf("before launching IP\n");

	foo_IP1 (in_main, out, &dest_out);
	printf("after launching IP\n");

	//Unpack the output
	for(i = 0; i < N-2; i++)
		for(j = 0; j < N; j++){
			out_main[i*N + j] = out.read().data;
		}

	//Compare the output
	for(i = 0; i < N-2; i++)
		for(j =0; j< N; j++){
			if(out_main[i*N+j] != out_gold[i*N+j]){
				printf("Incorrect result at [%d][%d], golden is %d, mine is%d\n",
						i, j, out_gold[i*N+j], out_main[i*N+j]);
				return -1;
			}
		}

		return 0;
}
