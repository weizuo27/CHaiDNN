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
CFLAG += -fPIC 
CFLAG += -fsanitize=address
CC      = g++
GCOV += #-fprofile-arcs -ftest-coverage

csim.out: ./tb/*.cpp ./src/*.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o csim.out  ./tb/*.cpp ./src/*.cpp   -MMD $(IFLAG)

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt




