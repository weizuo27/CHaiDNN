
def AlignSize(x, y):
    ret = x if (x%y == 0) else ((x/y + 1)*y)
    return ret

def nkpfCount(scalar_conv_args,KER_PROC,XI_WEIGHTBUFF_DEPTH):
    group_flag   = scalar_conv_args[11]
    inputplanes  = scalar_conv_args[16]   
    fsz          = scalar_conv_args[7]
    outDepth     = scalar_conv_args[4]
    inDepth      = scalar_conv_args[5]

    outputplanes = AlignSize(outDepth, KER_PROC)
    con_outDepth = AlignSize(outDepth, KER_PROC)
    con_inDepth = AlignSize(inDepth, 4)
    ip = inputplanes
    op = outputplanes
    if((group_flag) and (outDepth > KER_PROC)):
        op = outputplanes/2
        
    n_kbuff_depth = XI_WEIGHTBUFF_DEPTH
    
    max_nkpf = 0
    if KER_PROC==1:
        max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4)*4)
    elif KER_PROC==2:
        max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4)*2)
    else:
        max_nkpf = n_kbuff_depth/(((fsz*fsz))*(ip/4))

    if(max_nkpf>15):
        max_nkpf=15

    rem = 0

    if(KER_PROC==16):
        rem = op%(max_nkpf*16)
    elif (KER_PROC==8):
        rem = op%(max_nkpf*8)
    else:
        rem = op%(max_nkpf*4)

    while(rem!=0):
        max_nkpf-= 1
        if(KER_PROC==16):
            rem = op%(max_nkpf*16)
        elif (KER_PROC==8):
            rem = op%(max_nkpf*8)
        else:
            rem = op%(max_nkpf*4) 
    scalar_conv_args[13]=max_nkpf

def straddleFactorCount(scalar_conv_args, inDepth, filter_size, group_flag):
    numInpPlanes  = AlignSize(inDepth, 4)
    fsz2 = filter_size*filter_size
    split = 0

    if(inDepth < 4):
        split = 1
    else:
        split = group_flag + 1

    inp_planes = 0
    if((group_flag) and (inDepth > 4)):
        inp_planes = numInpPlanes/2
    else:
        inp_planes = numInpPlanes

    exp1 = False
    exp2 = False
    FEEDING_BUFF_DEPTH = 1024
    strad_fact=1
    n_inbuff_depth = FEEDING_BUFF_DEPTH

    while(not exp1):
        print "bb"
        comp_planes = inp_planes/ strad_fact
        exp1 =  (((comp_planes/4)*fsz2) <=  (n_inbuff_depth/2))
        exp2 =  ((comp_planes*fsz2) <= XI_WEIGHTBUFF_DEPTH)
        strad_fact= strad_fact <<1

    straddle_factor = strad_fact>>1 
    compute_planes =  numInpPlanes / (straddle_factor*split)

    scalar_conv_args[16] = compute_planes
    scalar_conv_args[17] = straddle_factor

def computeLatency (
    conv_inp_height  , 
    conv_inp_width   , 
    conv_out_height  , 
    conv_out_width   , 
    conv_out_planes  , 
    conv_inp_planes  , 
    conv_stride      , 
    conv_filter_height,
    conv_filter_width, 
    conv_pad         , 
    conv_group       , 
    rowStep,
    XI_KER_PROC,
    XI_PIX_PROC,
    XI_WEIGHTBUFF_DEPTH
):

    scalar_conv_args = [0] * 128
    scalar_conv_args[0]  = conv_inp_height
    scalar_conv_args[1]  = conv_inp_width
    scalar_conv_args[2]  = conv_out_height
    scalar_conv_args[3]  = conv_out_width
    scalar_conv_args[4]  = conv_out_planes
    scalar_conv_args[5]  = conv_inp_planes
    scalar_conv_args[6]  = conv_stride
    scalar_conv_args[7]  = conv_filter_height
    scalar_conv_args[8]  = conv_filter_width
    scalar_conv_args[9]  = conv_pad         
    scalar_conv_args[11] = conv_group

    straddleFactorCount(scalar_conv_args,conv_inp_planes,conv_filter_height,conv_group)
    scalar_conv_args[61] = AlignSize(scalar_conv_args[16], 4)

    scalar_conv_args[77] = conv_filter_height * conv_filter_width * (scalar_conv_args[61]/4)
    compute_loop_count=scalar_conv_args[77]
    LatCompute16Ker_fy=compute_loop_count+1

    nkpfCount(scalar_conv_args,XI_KER_PROC,XI_WEIGHTBUFF_DEPTH) 

    nkpf= scalar_conv_args[13]

    latOsggBuff_fx=XI_PIX_PROC

    latProcResult_fe=latOsggBuff_fx+LatCompute16Ker_fy+nkpf*max(latOsggBuff_fx,LatCompute16Ker_fy)
    scalar_conv_args[92] =  scalar_conv_args[13] * conv_filter_height * conv_filter_width * (scalar_conv_args[61]/4)
    scalar_conv_args[62] = AlignSize(scalar_conv_args[4], 16) 


    AXILATENCY = 2
    latLoadKernelsEn_fz=scalar_conv_args[92]*AXILATENCY
    print "latLoadKernelsEn_fz", latLoadKernelsEn_fz

    ker_loop_cnt=scalar_conv_args[62]

    ker_loop_tripcount= ker_loop_cnt/(XI_KER_PROC*nkpf)+1 if ker_loop_cnt%(XI_KER_PROC*nkpf) else ker_loop_cnt%(XI_KER_PROC*nkpf)

    latProcWeightBuff_fd=latLoadKernelsEn_fz+latProcResult_fe+ker_loop_tripcount*max(latLoadKernelsEn_fz,latProcResult_fe)

    scalar_conv_args[15]=rowStep

    total_pixel_fc0 = scalar_conv_args[15]*conv_out_width;
    pix_per_kp = XI_PIX_PROC
    pcmf_off1_fc0 = (total_pixel_fc0/pix_per_kp) +1;
    pcmf_m1=(scalar_conv_args[17]-1);
    pcmf_off_fc0 = pcmf_off1_fc0*pcmf_m1
    pc_loop_mul_fc0=pix_per_kp * pcmf_off_fc0;
    pc_loop_bound = total_pixel_fc0 + pc_loop_mul_fc0;


    if((scalar_conv_args[15] * conv_out_width) < pix_per_kp):
        pc_loop_bound = scalar_conv_args[17]*pix_per_kp    

    pc_tripcount= pc_loop_bound/XI_PIX_PROC+1 if (pc_loop_bound%XI_PIX_PROC) else (pc_loop_bound)/XI_PIX_PROC

    compute_planes=scalar_conv_args[61]
    latLoadFeedingBuff_fl = 0
    tmp = (XI_PIX_PROC/16+1) if (XI_PIX_PROC%16) else  (XI_PIX_PROC/16)
    latLoadFeedingBuff_fl=compute_planes/64*conv_filter_height*conv_filter_width*16*2*tmp;
    putBuff_fd = 0
    ProcInputBuff_fd=latLoadFeedingBuff_fl+latProcWeightBuff_fd+ pc_tripcount*max(latLoadFeedingBuff_fl,latProcWeightBuff_fd)

    return ProcInputBuff_fd

conv_inp_height = 20 
conv_inp_width = 20
conv_out_height = 18
conv_out_width = 18
conv_out_planes =  1
conv_inp_planes = 1
conv_stride = 1 
conv_filter_height = 3
conv_filter_width = 3
conv_pad = 0
conv_group = 1
rowStep = 1
XI_KER_PROC = 8
XI_PIX_PROC = 8
XI_WEIGHTBUFF_DEPTH = 512

aa = computeLatency(
    conv_inp_height  , 
    conv_inp_width   , 
    conv_out_height  , 
    conv_out_width   , 
    conv_out_planes  , 
    conv_inp_planes  , 
    conv_stride      , 
    conv_filter_height,
    conv_filter_width, 
    conv_pad         , 
    conv_group       , 
    rowStep,
    XI_KER_PROC,
    XI_PIX_PROC,
    XI_WEIGHTBUFF_DEPTH
    )

print conv_out_height/rowStep *  aa * 4
