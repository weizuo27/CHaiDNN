AUTOPILOT_ROOT := /opt/Xilinx/Vivado/2018.2




IFLAG += -I "${AUTOPILOT_ROOT}/include"

IFLAG += -I "/usr/include/x86_64-linux-gnu"
IFLAG += -D__SIM_FPO__

IFLAG += -D__SIM_OPENCV__

IFLAG += -D__SIM_FFT__

IFLAG += -D__SIM_FIR__

IFLAG += -D__SIM_DDS__

IFLAG += -D__DSP48E1__
IFLAG += -g
CC      = g++
GCOV = #-fprofile-arcs -ftest-coverage

csim.out: testbench.cpp xi_convolution_top.cpp print_parameter.cpp
	$(CC) $(GOCV)  -fsanitize=address -o csim.out  testbench.cpp xi_convolution_top.cpp print_parameter.cpp -MMD $(IFLAG)

clean:

	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda  *.txt




