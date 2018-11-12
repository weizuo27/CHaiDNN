#include "lat_estimate.hpp"

#define AlignSize(x, y) (x%y == 0) ? x : ((x/y + 1)*y)
#define FEEDING_BUFF_DEPTH 1024
#define max(x,y)     (  ((x)>(y))?( x) : (y)  )

#define AXILATENCY 2
int nkpfCount(
	int *scalar_conv_args,

	int KER_PROC,
	int XI_WEIGHTBUFF_DEPTH

)
{
	int group_flag   = scalar_conv_args[11];
	int inputplanes  = scalar_conv_args[16];   //compute_planes
	int fsz          = scalar_conv_args[7];
	int outDepth     = scalar_conv_args[4];
	int inDepth      = scalar_conv_args[5];


	int outputplanes = AlignSize(outDepth, KER_PROC);

	int con_outDepth = AlignSize(outDepth, KER_PROC);
	int con_inDepth = AlignSize(inDepth, 4);

	int ip = inputplanes;
	int op;
	//TODO : groupflag for AlexNet
	if((group_flag) &&(outDepth > KER_PROC))
	{
		op = outputplanes/2;
	}else{
		op = outputplanes;
	}

	int n_kbuff_depth = XI_WEIGHTBUFF_DEPTH;
#if	DP_ENABLE
	n_kbuff_depth = XI_WEIGHTBUFF_DEPTH - 1;
#else
	n_kbuff_depth = XI_WEIGHTBUFF_DEPTH;
#endif


#if KER_PROC==1
	int max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4)*4);
#elif KER_PROC==2
	int max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4)*2);
#else
	int max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4));
#endif

	if(max_nkpf>15)
		max_nkpf=15;

	int rem;

	if(KER_PROC==16)
	rem = op%(max_nkpf*16);
	else if (KER_PROC==8)
	rem = op%(max_nkpf*8);
	else
	rem = op%(max_nkpf*4);


	while(rem!=0){
		max_nkpf--;

	if(KER_PROC==16)
		rem = op%(max_nkpf*16);
	else if (KER_PROC==8)
		rem = op%(max_nkpf*8);
	else
		rem = op%(max_nkpf*4);


	}

	//*nkpf = max_nkpf;
scalar_conv_args[13]=max_nkpf;
}


void straddleFactorCount( 
	int *scalar_conv_args,
	int inDepth       ,//= scalar_conv_args[5];
	int filter_size   ,//= scalar_conv_args[7];
	int group_flag    //= scalar_conv_args[11];
	
	)
{



	int	numInpPlanes  = AlignSize(inDepth, 4);

	int fsz2 = filter_size*filter_size;

	int split;
	if(inDepth < 4)
	{
		split = 1;
	}else
	{
		split = group_flag + 1;
	}

	int inp_planes;
	if((group_flag) && (inDepth > 4))
		inp_planes = numInpPlanes/2;
	else
		inp_planes = numInpPlanes;

	bool exp1;
	bool exp2;
	int strad_fact=1;

	int n_inbuff_depth = FEEDING_BUFF_DEPTH;
	
#if	DP_ENABLE
	n_inbuff_depth = FEEDING_BUFF_DEPTH - 1;
#else
	n_inbuff_depth = FEEDING_BUFF_DEPTH;
#endif

	do{
		int comp_planes = inp_planes/ strad_fact;
		//expression based on inputbuff depth
		//## input_buff depth is fixed to 1024
		exp1 =  (((comp_planes/4)*fsz2) <=  (n_inbuff_depth/2));
		//exp2 =  ((comp_planes*fsz2) <= XI_WEIGHTBUFF_DEPTH);
		strad_fact= strad_fact <<1;

	}while(!(exp1));

	int straddle_factor = strad_fact>>1;
	//printf("***straddle factor is %d\n", strad_fact>>1);

	int compute_planes =  numInpPlanes / (straddle_factor*split);

	scalar_conv_args[16] = compute_planes;
	scalar_conv_args[17] = straddle_factor;

}




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
)
{
	int scalar_conv_args[128];


	scalar_conv_args[0]  = conv_inp_height;
	scalar_conv_args[1]  = conv_inp_width;
	scalar_conv_args[2]  = conv_out_height;
	scalar_conv_args[3]  = conv_out_width;
	scalar_conv_args[4]  = conv_out_planes;
	scalar_conv_args[5]  = conv_inp_planes;
	scalar_conv_args[6]  = conv_stride;
	scalar_conv_args[7]  = conv_filter_height;
	scalar_conv_args[8]  = conv_filter_width;
	scalar_conv_args[9]  = conv_pad;         //pad_h
	scalar_conv_args[11] = conv_group;
	scalar_conv_args[29] = conv_dilation;


	scalar_conv_args[12] = layer_id;                  //layer_id

	straddleFactorCount(scalar_conv_args,conv_inp_planes,conv_filter_height,conv_group); // compute args[16]and args[17]

	
	scalar_conv_args[61] = AlignSize(scalar_conv_args[16], 4);

	scalar_conv_args[77] = conv_filter_height * conv_filter_width * (scalar_conv_args[61]/4);

	int compute_loop_count=scalar_conv_args[77];
	int LatCompute16Ker_fy=compute_loop_count+1;


	nkpfCount(scalar_conv_args,XI_KER_PROC,XI_WEIGHTBUFF_DEPTH); //comput args[13]



	int nkpf= scalar_conv_args[13];

	int latOsggBuff_fx=XI_PIX_PROC;

	int latProcResult_fe=latOsggBuff_fx+LatCompute16Ker_fy+nkpf*max(latOsggBuff_fx,LatCompute16Ker_fy);



	scalar_conv_args[92] =  scalar_conv_args[13] * conv_filter_height * conv_filter_width * (scalar_conv_args[61]/4);
	scalar_conv_args[62] = AlignSize(scalar_conv_args[4], 16); // output_planes_align16


	int latLoadKernelsEn_fz=scalar_conv_args[92]*AXILATENCY;

	int ker_loop_cnt=scalar_conv_args[62];
	int ker_loop_tripcount=  ker_loop_cnt%(XI_KER_PROC*nkpf)? ker_loop_cnt/(XI_KER_PROC*nkpf)+1:ker_loop_cnt%(XI_KER_PROC*nkpf);

	int latProcWeightBuff_fd=latLoadKernelsEn_fz+latProcResult_fe+ker_loop_tripcount*max(latLoadKernelsEn_fz,latProcResult_fe);

	scalar_conv_args[15]=rowStep;

	int total_pixel_fc0 = scalar_conv_args[15]*conv_out_width;
	int pix_per_kp = XI_PIX_PROC;
	int pcmf_off1_fc0 = (total_pixel_fc0/pix_per_kp) +1;
	int pcmf_m1=(scalar_conv_args[17]-1);
	int pcmf_off_fc0 = pcmf_off1_fc0*pcmf_m1;
	int pc_loop_mul_fc0=pix_per_kp * pcmf_off_fc0;
	int pc_loop_bound = total_pixel_fc0 + pc_loop_mul_fc0;



	if((scalar_conv_args[15] * conv_out_width) < pix_per_kp){
		pc_loop_bound = scalar_conv_args[17]*pix_per_kp;	
	}

	int pc_tripcount= (pc_loop_bound%XI_PIX_PROC)?pc_loop_bound/XI_PIX_PROC+1:(pc_loop_bound)/XI_PIX_PROC;

	int compute_planes=scalar_conv_args[61];

	

	int latLoadFeedingBuff_fl;
	latLoadFeedingBuff_fl=compute_planes/64*conv_filter_height*conv_filter_width*16*2*( (XI_PIX_PROC%16)? (XI_PIX_PROC/16+1) : (XI_PIX_PROC/16) );
	int ProcInputBuff_fd;
	ProcInputBuff_fd=latLoadFeedingBuff_fl+latProcWeightBuff_fd+ pc_tripcount*max(latLoadFeedingBuff_fl,latProcWeightBuff_fd);

return ProcInputBuff_fd;
}
//
