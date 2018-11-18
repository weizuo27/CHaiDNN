# (c) Copyright 2013 - 2016 Xilinx, Inc. All rights reserved.
#
# This file contains confidential and proprietary information of Xilinx, Inc. and
# is protected under U.S. and international copyright and other intellectual
# property laws.
#
# DISCLAIMER
# This disclaimer is not a license and does not grant any rights to the materials
# distributed herewith. Except as otherwise provided in a valid license issued to
# you by Xilinx, and to the maximum extent permitted by applicable law: (1) THESE
# MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX HEREBY
# DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
# INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
# FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
# in contract or tort, including negligence, or under any other theory of
# liability) for any loss or damage of any kind or nature related to, arising
# under or in connection with these materials, including for any direct, or any
# indirect, special, incidental, or consequential loss or damage (including loss
# of data, profits, goodwill, or any type of loss or damage suffered as a result
# of any action brought by a third party) even if such damage or loss was
# reasonably foreseeable or Xilinx had been advised of the possibility of the
# same.
#
# CRITICAL APPLICATIONS
# Xilinx products are not designed or intended to be fail-safe, or for use in any
# application requiring fail-safe performance, such as life-support or safety
# devices or systems, Class III medical devices, nuclear facilities, applications
# related to the deployment of airbags, or any other applications that could lead
# to death, personal injury, or severe property or environmental damage
# (individually and collectively, "Critical Applications"). Customer assumes the
# sole risk and liability of any use of Xilinx products in Critical Applications,
# subject only to applicable laws and regulations governing limitations on product
# liability.
#
# THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
# ALL TIMES.

# Main Variables
LIBRARY     := libxlnxdnn.so
PLATFORM    := ../../platform/build/zcu102
CLOCK_ID    := 1 # Core clock ID
DM_CLOCK_ID := 1 # Data Motion Clock ID
# Below path should be relative
PROJ_PATH   := ..
# H/W Kernel enables
DIET_CHAI_Z	     ?= 0
DIET_CHAI_ZUPLUS ?= 0

POOL_ENABLE   := 0
DECONV_ENABLE := 0
CONV_ENABLE := 0

# Internal Path Variables
SRC_PATH      := ${PROJ_PATH}
WRAPPER_PATH  := ${PROJ_PATH}/wrapper
# Include directories
ASSEMBLE_PIPE_INC_DIRS   := include src
ASSEMBLE_PIPE_INCS       := ${ASSEMBLE_PIPE_INC_DIRS:%=-I${SRC_PATH}/assemble_pipe/%}

# Below path must have absolute path
ASSEMBLE_PIPE_SCRIPTS_PATH := $(shell pwd)/${PROJ_PATH}/assemble_pipe/scripts

# Options and command(s) variables
VERBOSE  := #-verbose

CC       := sds++ -sds-pf ${PLATFORM} ${VERBOSE}

IDIRS    := -I. ${ASSEMBLE_PIPE_INCS}

CFLAGS   := -Wall -O3 -c ${IDIRS} -D__SDSOC=1 -Wno-unused-label -D__SYNTHESIS__=1
CFLAGS1   := -Wall -O3 -c 

SDSFLAGS := -dmclkid ${DM_CLOCK_ID} \
            -xp param:compiler.skipTimingCheckAndFrequencyScaling=1 \
            -xp "vivado_prop:run.impl_1.{STEPS.OPT_DESIGN.ARGS.MORE OPTIONS}={-directive Explore}" \
            -xp "vivado_prop:run.impl_1.{STEPS.PLACE_DESIGN.ARGS.MORE OPTIONS}={-directive Explore}" \
            -xp "vivado_prop:run.impl_1.STEPS.PHYS_OPT_DESIGN.IS_ENABLED=1" \
            -xp "vivado_prop:run.impl_1.{STEPS.PHYS_OPT_DESIGN.ARGS.MORE OPTIONS}={-directive Explore}" \
            -xp "vivado_prop:run.impl_1.{STEPS.ROUTE_DESIGN.ARGS.MORE OPTIONS}={-directive Explore}"

WRAPPER_FLAGS := -DXI_DIET_CHAI_Z=${DIET_CHAI_Z} -DXI_DIET_CHAI_ZUPLUS=${DIET_CHAI_ZUPLUS}

ASSEMBLE_PIPE_FLAGS    := -DXI_DIET_CHAI_Z=${DIET_CHAI_Z} -DXI_DIET_CHAI_ZUPLUS=${DIET_CHAI_ZUPLUS} \
                 -D__DSP48E2__ \
                 -xp "vivado_prop:run.synth_1.{STEPS.SYNTH_DESIGN.TCL.PRE}={${ASSEMBLE_PIPE_SCRIPTS_PATH}/mcps.tcl}" \
                 -xp "vivado_prop:run.impl_1.{STEPS.PLACE_DESIGN.TCL.PRE}={${ASSEMBLE_PIPE_SCRIPTS_PATH}/mcps.tcl}" 

REQ_LIBS := dnn_wrapper.o

# Add requried kernels
ifeq (${CONV_ENABLE}, 1)
	REQ_LIBS += xi_convolution_top.o
	WRAPPER_FLAGS += -D__CONV_ENABLE__=1
endif

ifeq (${POOL_ENABLE}, 1)
	REQ_LIBS += pooling_layer_dp_2xio_top.o
	WRAPPER_FLAGS += -D__POOL_ENABLE__=1
endif

ifeq (${DECONV_ENABLE}, 1)
	REQ_LIBS += xi_deconv_top.o
	WRAPPER_FLAGS += -D__DECONV_ENABLE__=1
endif

REQ_LIBS += ConvPipeline.o



# Actual Tragets
.PHONY: all
all: ${LIBRARY}

print_vars :
	@echo "PROJ_PATH         = \"${PROJ_PATH}\""
	@echo "SRC_PATH          = \"${SRC_PATH}\""
	@echo "WRAPPER_PATH      = \"${WRAPPER_PATH}\""
	@echo "ASSEMBLE_PIPE_SCRIPTS_PATH = \"${ASSEMBLE_PIPE_SCRIPTS_PATH}\""
	@echo "ASSEMBLE_PIPE_INCS         = \"${ASSEMBLE_PIPE_INCjS}\""

ConvPipeline.o: ${SRC_PATH}/assemble_pipe/src/pipeSystem.cpp 
	${CC} ${SDSFLAGS} ${CFLAGS1} ${ASSEMBLE_PIPE_FLAGS} -sds-hw ConvPipeline ${SRC_PATH}/assemble_pipe/src/pipeSystem.cpp \
        -files ${SRC_PATH}/assemble_pipe/IP110/IP110.cpp,${SRC_PATH}/assemble_pipe/IP110/IP110.h,${SRC_PATH}/assemble_pipe/IP36/IP36.cpp,${SRC_PATH}/assemble_pipe/IP36/IP36.h,${SRC_PATH}/assemble_pipe/IP217/IP217.cpp,${SRC_PATH}/assemble_pipe/IP217/IP217.h\
        -clkid ${CLOCK_ID} -hls-tcl ${ASSEMBLE_PIPE_SCRIPTS_PATH}/config_core.tcl -sds-end -fPIC $< -o $@

#-hls-tcl ${SRC_PATH}/assemble_pipe/scripts/vivado_hls.tcl \

dnn_wrapper.o: ${WRAPPER_PATH}/dnn_wrapper.cpp
	${CC} ${SDSFLAGS} ${CFLAGS} ${WRAPPER_FLAGS} -fPIC $< -o $@


${LIBRARY}: ${REQ_LIBS}
	${CC} ${SDSFLAGS} ${ASSEMBLE_PIPE_FLAGS} -mno-bitstream -shared $^ -o $@

# Copy lib to SD_Card dir
copy:
	cp sd_card/${LIBRARY} ../../SD_Card/lib/
	cp sd_card/BOOT.BIN ../../SD_Card/
	cp sd_card/image.ub ../../SD_Card/

# Some clean Targets
clean:
	${RM} ${LIBRARY} ${REQ_LIBS}

ultraclean: clean
	${RM} ${LIBRARY}.bit
	${RM} -r _sds sd_card .Xil
