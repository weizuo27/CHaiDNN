
#include "../IP217/IP217.h"
#include "../include/xi_conv_config.h"
#include <hls_stream.h>
#include <ap_int.h>



#pragma SDS data zero_copy(IP36_weights1[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_weights2[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_weights3[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_weights4[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_bias[0:(BIAS_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP36_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_input_other2[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_output1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_output2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP36_inp_norm_2[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP36_inp_norm_3[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP36_istg_out1[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP36_istg_out2[0:(INPUT_SIZE)>>NPC2])



#pragma SDS data sys_port(IP36_weights1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP36_weights2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP36_weights3:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(IP36_weights4:ps_e_S_AXI_HP3_FPD)
#pragma SDS data sys_port(IP36_bias:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP36_input_other1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP36_input_other2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP36_input_1st:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP36_output1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP36_output2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP36_inp_norm_2:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(IP36_inp_norm_3:ps_e_S_AXI_HP3_FPD)
#pragma SDS data sys_port(IP36_istg_out1:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP36_istg_out2:ps_e_S_AXI_HP0_FPD)



#pragma SDS data zero_copy(IP110A_weights1[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_weights2[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_bias[0:(BIAS_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110A_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_input_other2[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_output1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_output2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110A_inp_norm_2[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110A_inp_norm_3[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110A_istg_out1[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110A_istg_out2[0:(INPUT_SIZE)>>NPC2])

#pragma SDS data sys_port(IP110A_weights1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110A_weights2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110A_bias:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110A_input_other1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110A_input_other2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110A_input_1st:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110A_output1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110A_output2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110A_inp_norm_2:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(IP110A_inp_norm_3:ps_e_S_AXI_HP3_FPD)
#pragma SDS data sys_port(IP110A_istg_out1:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110A_istg_out2:ps_e_S_AXI_HP0_FPD)



#pragma SDS data zero_copy(IP110B_weights1[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_weights2[0:(WEIGHT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_bias[0:(BIAS_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110B_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_input_other1[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_input_other2[0:(INPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_output1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_output2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(IP110B_inp_norm_2[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110B_inp_norm_3[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110B_istg_out1[0:(INPUT_SIZE)>>NPC2])
#pragma SDS data zero_copy(IP110B_istg_out2[0:(INPUT_SIZE)>>NPC2])


#pragma SDS data sys_port(IP110B_weights1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110B_weights2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110B_bias:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110B_input_other1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110B_input_other2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110B_input_1st:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110B_output1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(IP110B_output2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110B_inp_norm_2:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(IP110B_inp_norm_3:ps_e_S_AXI_HP3_FPD)
#pragma SDS data sys_port(IP110B_istg_out1:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(IP110B_istg_out2:ps_e_S_AXI_HP0_FPD)


#pragma SDS data zero_copy(pool1_inMem1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool1_inMem2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool1_outMem1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool1_outMem2[0:(OUTPUT_SIZE)>>NPC1])

#pragma SDS data sys_port(pool1_inMem1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(pool1_inMem2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(pool1_outMem1:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(pool1_outMem2:ps_e_S_AXI_HP3_FPD)

#pragma SDS data zero_copy(pool2_inMem1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool2_inMem2[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool2_outMem1[0:(OUTPUT_SIZE)>>NPC1])
#pragma SDS data zero_copy(pool2_outMem2[0:(OUTPUT_SIZE)>>NPC1])

#pragma SDS data sys_port(pool2_inMem1:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(pool2_inMem2:ps_e_S_AXI_HP1_FPD)
#pragma SDS data sys_port(pool2_outMem1:ps_e_S_AXI_HP2_FPD)
#pragma SDS data sys_port(pool2_outMem2:ps_e_S_AXI_HP3_FPD)



#pragma SDS data zero_copy(argsConvIP36[0:128])
#pragma SDS data zero_copy(argsConvIP110A[0:128])
#pragma SDS data zero_copy(argsConvIP110B[0:128])
#pragma SDS data zero_copy(argsPool1[0:32])
#pragma SDS data zero_copy(argsPool2[0:32])
#pragma SDS data zero_copy(argsStreamCombiner[0:4])
#pragma SDS data zero_copy(argsStreamDivisor[0:4])




#pragma SDS data sys_port(argsConvIP36:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(argsConvIP110A:ps_e_S_AXI_HP0_FPD)
#pragma SDS data sys_port(argsConvIP110B:ps_e_S_AXI_HP0_FPD)

//*FIXME: We don't add sys_port for argsPool1, argsPool2 argsStreamCombiner and argsStreamDivisor. 
//* add it after we have better understanding of arangement of memory ports






void ConvPipeline
(
    gmem_weighttype *IP36_weights1, 
    gmem_weighttype *IP36_weights2,
    gmem_weighttype *IP36_weights3,
    gmem_weighttype *IP36_weights4,
    gmem_outputtype *IP36_output1,
    gmem_outputtype *IP36_output2,
    gmem_inputtype_layerx *IP36_input_other1,	
    gmem_inputtype_layerx *IP36_input_other2,
    gmem_inputtype_layer1 *IP36_input_1st,
    gmem_biastype *IP36_bias,
    gmem_inputtype_layer1 *IP36_inp_norm_2, 
    gmem_inputtype_layer1 *IP36_inp_norm_3,
    gmem_inputtype_layer1 *IP36_istg_out1,
    gmem_inputtype_layer1 *IP36_istg_out2,

    gmem_weighttype *IP110A_weights1, 
    gmem_weighttype *IP110A_weights2,
    gmem_outputtype *IP110A_output1,
    gmem_outputtype *IP110A_output2,
    gmem_inputtype_layerx *IP110A_input_other1,	
    gmem_inputtype_layerx *IP110A_input_other2,
    gmem_inputtype_layer1 *IP110A_input_1st,
    gmem_biastype *IP110A_bias,
    gmem_inputtype_layer1 *IP110A_inp_norm_2,  
    gmem_inputtype_layer1 *IP110A_inp_norm_3,
    gmem_inputtype_layer1 *IP110A_istg_out1,
    gmem_inputtype_layer1 *IP110A_istg_out2,

    gmem_weighttype *IP110B_weights1, 
    gmem_weighttype *IP110B_weights2,
    gmem_outputtype *IP110B_output1,
    gmem_outputtype *IP110B_output2,
    gmem_inputtype_layerx *IP110B_input_other1,	
    gmem_inputtype_layerx *IP110B_input_other2,
    gmem_inputtype_layer1 *IP110B_input_1st,
    gmem_biastype *IP110B_bias,
    gmem_inputtype_layer1 *IP110B_inp_norm_2, 
    gmem_inputtype_layer1 *IP110B_inp_norm_3,
    gmem_inputtype_layer1 *IP110B_istg_out1,
    gmem_inputtype_layer1 *IP110B_istg_out2,




    GMEM_MAXPOOLTYPE* pool1_inMem1,
    GMEM_MAXPOOLTYPE* pool1_inMem2,
    GMEM_MAXPOOLTYPE* pool1_outMem1,
    GMEM_MAXPOOLTYPE* pool1_outMem2,

  
    GMEM_MAXPOOLTYPE* pool2_inMem1,
    GMEM_MAXPOOLTYPE* pool2_inMem2,
    GMEM_MAXPOOLTYPE* pool2_outMem1,
    GMEM_MAXPOOLTYPE* pool2_outMem2,
  
    int* argsConvIP36,
    int* argsConvIP110A,
    int* argsConvIP110B,
    int* argsPool1,
    int* argsPool2,
    int* argsStreamDivisor,
    int* argsStreamCombiner
);