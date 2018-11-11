#include "/opt/Xilinx/Vivado/2018.2/include/gmp.h"
#include "head_ip2.hpp"
#include "stdlib.h"
#include "stdio.h"
int main(){
	printf("just started\n");

    int args[1];
    args[0] = 30;
    int M = args[0];

	uint in_main[M*N];
	AXI_STREAM in;
	AXI_VALUE tmp;

	uint out_main[(M-2)*N];
	uint out_gold[(M-2)*N];


	int i, j;
	printf("before initialization\n");

	//input initialization
	for(i = 0; i < M; i++){
		for(j = 0; j < N; j++){
			in_main[i*N+j] = i;
            tmp.data = i;
            in.write(tmp);
		}
	}
	printf("after initialization\n");

	//Create reference output
	for(i = 0; i <M-2; i++){
		for(j = 0; j<N; j++){
			out_gold[i*N + j] = in_main[i*N+j]+in_main[(i+1)*N+j]+in_main[(i+2)*N+j];
		}
	}

	printf("before launching IP\n");

	foo_IP2 (in, out_main, args);
	printf("after launching IP\n");

	//Compare the output
	for(i = 0; i < M-2; i++)
		for(j =0; j< N; j++){
			if(out_main[i*N+j] != out_gold[i*N+j]){
				printf("Incorrect result at [%d][%d], golden is %d, mine is%d\n",
						i, j, out_gold[i*N+j], out_main[i*N+j]);
				return -1;
			}
		}
		return 0;
}
