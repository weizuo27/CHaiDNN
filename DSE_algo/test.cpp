# include "stdlib.h"
#include "stdio.h"
#include "lat_estimate.hpp"

int main(){
	int conv_inp_height = 224;
	int conv_inp_width = 224;
	int conv_out_height = 55;
	int conv_out_width = 55;
	int conv_out_planes = 64;
	int conv_inp_planes = 3;
	int conv_stride = 4;
	int conv_filter_height = 11;
	int conv_filter_width = 11;
	int conv_pad = 0;
	int conv_group = 1;
	int conv_dilation = 0;  //Dont care
	int layer_id = 0; //Dont care
	int rowStep = 4;
	int XI_KER_PROC = 16;
	int XI_PIX_PROC = 32;
	int XI_WEIGHTBUFF_DEPTH = 2048;

    int lat = 
    computeLatency(
	 conv_inp_height  , //ttomShape.at(2);  //input_height
	 conv_inp_width   , //ttomShape.at(3);   //input_width
	 conv_out_height  , //pShape.at(2);     //output_height
	 conv_out_width   , //pShape.at(3);      //output_width
	 conv_out_planes  , //nv_params->M_3d;     //output planes
	 conv_inp_planes  , //nv_params->N_3d;     //input planes
	 conv_stride      , //nv_params->stride_h_3d;  //stride_h
	 conv_filter_height, //conv_params->filter_h_3d;   //filter_h
	 conv_filter_width, //conv_params->filter_w_3d;  //filter_w
	 conv_pad         , //nv_params->pad_h_3d;       //pad_h
	 conv_group       , //
	 conv_dilation,
	 layer_id, //only used when it is first layer or not 
	 rowStep,
	 XI_KER_PROC,
	 XI_PIX_PROC,
	 XI_WEIGHTBUFF_DEPTH);

    printf ("latency %d\n", conv_inp_height/rowStep * lat);
}
