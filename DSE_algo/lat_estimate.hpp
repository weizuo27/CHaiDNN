#ifndef _LAT_EST_

int computeLatency
(
	int conv_inp_height  , //ttomShape.at(2);  //input_height
	int conv_inp_width   , //ttomShape.at(3);   //input_width
	int conv_out_height  , //pShape.at(2);     //output_height
	int conv_out_width   , //pShape.at(3);      //output_width
	int conv_out_planes  , //nv_params->M_3d;     //output planes
	int conv_inp_planes  , //nv_params->N_3d;     //input planes
	int conv_stride      , //nv_params->stride_h_3d;  //stride_h
	int conv_filter_height, //conv_params->filter_h_3d;   //filter_h
	int conv_filter_width, //conv_params->filter_w_3d;  //filter_w
	int conv_pad         , //nv_params->pad_h_3d;       //pad_h
	int conv_group       , //
	int conv_dilation,
	int layer_id, //only used when it is first layer or not 
	int rowStep,
	int XI_KER_PROC,
	int XI_PIX_PROC,
	int XI_WEIGHTBUFF_DEPTH
);
#endif
