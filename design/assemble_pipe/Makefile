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

all: csim.out

IP36.o: ./IP36/IP36.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o IP36.o  -c ./IP36/IP36.cpp   -MMD $(IFLAG)
IP110.o: ./IP110/IP110.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o IP110.o  -c ./IP110/IP110.cpp   -MMD $(IFLAG)
IP217.o: ./IP217/IP217.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o IP217.o  -c ./IP217/IP217.cpp   -MMD $(IFLAG)

pipeSystem.o: ./src/pipeSystem.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o pipeSystem.o  -c ./src/pipeSystem.cpp   -MMD $(IFLAG)

verification.o:./src/verification.cpp	
	$(CC) $(GCOV)  $(CFLAG)  -o verification.o  -c ./src/verification.cpp   -MMD $(IFLAG)


main.o:./src/testbench.cpp	
	$(CC) $(GCOV)  $(CFLAG)  -o main.o  -c ./src/testbench.cpp   -MMD $(IFLAG)

csim.out: main.o verification.o pipeSystem.o IP110.o IP217.o IP36.o
	$(CC) $(GCOV)  $(CFLAG)  -o csim.out  main.o verification.o pipeSystem.o IP110.o IP217.o IP36.o   -MMD $(IFLAG)
	

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d




