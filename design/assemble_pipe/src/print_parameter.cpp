

#include "dbg.h"


void printConvDesc(conv_struct & conv_desc, FILE* fp)
{
    fprintf(fp,"conv_desc.in_start_row： %d \n",(int) conv_desc.in_start_row); 
    fprintf(fp,"conv_desc.in_end_row： %d \n",(int) conv_desc.in_end_row);
    fprintf(fp,"conv_desc.group_en： %d \n",(int) conv_desc.group_en);
    fprintf(fp,"conv_desc.pad_en： %d \n",(int) conv_desc.pad_en);
    fprintf(fp,"conv_desc.pad_num： %d \n",(int) conv_desc.pad_num);
    fprintf(fp,"conv_desc.relu_en： %d \n",(int) conv_desc.relu_en);
    fprintf(fp,"conv_desc.layer_id： %d \n",(int) conv_desc.layer_id);
    fprintf(fp,"conv_desc.ostg_row_count： %d \n",(int) conv_desc.ostg_row_count);
    fprintf(fp,"conv_desc.ostg_row_cnt_last_itr： %d \n",(int) conv_desc.ostg_row_cnt_last_itr);
    fprintf(fp,"conv_desc.istg_row_count： %d \n",(int) conv_desc.istg_row_count);
    fprintf(fp,"conv_desc.straddle： %d \n",(int) conv_desc.straddle);
    fprintf(fp,"conv_desc.compute_loop_count： %d \n",(int) conv_desc.compute_loop_count);
    fprintf(fp,"conv_desc.out_pix： %d \n",(int) conv_desc.out_pix);
    fprintf(fp,"conv_desc.shift_precision1： %d \n",(int) conv_desc.shift_precision1);
    fprintf(fp,"conv_desc.shift_precision2： %d \n",(int) conv_desc.shift_precision2);
    fprintf(fp,"conv_desc.inout_precision： %d \n",(int) conv_desc.inout_precision);
    fprintf(fp,"conv_desc.buff_split： %d \n",(int) conv_desc.buff_split);
    fprintf(fp,"conv_desc.pix_by_outwidth： %d \n",(int) conv_desc.pix_by_outwidth);
    fprintf(fp,"conv_desc.pix_mod_outwidth： %d \n",(int) conv_desc.pix_mod_outwidth);
    fprintf(fp,"conv_desc.pix2_div_outwidth： %d \n",(int) conv_desc.pix2_div_outwidth);
    fprintf(fp,"conv_desc.pix2_div_outwidth_x_ow： %d \n",(int) conv_desc.pix2_div_outwidth_x_ow);
    fprintf(fp,"conv_desc.pix2_mod_outwidth： %d \n",(int) conv_desc.pix2_mod_outwidth);
    fprintf(fp,"conv_desc.fsz_by_stride： %d \n",(int) conv_desc.fsz_by_stride);
    fprintf(fp,"conv_desc.fsz_mod_stride： %d \n",(int) conv_desc.fsz_mod_stride);
    fprintf(fp,"conv_desc.mac_loop_count： %d \n",(int) conv_desc.mac_loop_count);
    fprintf(fp,"conv_desc.opcode： %d \n",(int) conv_desc.opcode);
    fprintf(fp,"conv_desc.conv_enable： %d \n",(int) conv_desc.conv_enable);
    fprintf(fp,"conv_desc.lrn_intra_sos_enable： %d \n",(int) conv_desc.lrn_intra_sos_enable);
    fprintf(fp,"conv_desc.lrn_inter_sos_enable： %d \n",(int) conv_desc.lrn_inter_sos_enable);
    fprintf(fp,"conv_desc.bn_intra_sos_enable： %d \n",(int) conv_desc.bn_intra_sos_enable);
    fprintf(fp,"conv_desc.l2_sos_enable： %d \n",(int) conv_desc.l2_sos_enable);
    fprintf(fp,"conv_desc.sos_enable： %d \n",(int) conv_desc.sos_enable);
    fprintf(fp,"conv_desc.bn_intra_mean_enable： %d \n",(int) conv_desc.bn_intra_mean_enable);
    fprintf(fp,"conv_desc.bn_snb_enable： %d \n",(int) conv_desc.bn_snb_enable);
    fprintf(fp,"conv_desc.pix_per_kp： %d \n",(int) conv_desc.pix_per_kp);
    fprintf(fp,"conv_desc.loop_bound_1x1_filter： %d \n",(int) conv_desc.loop_bound_1x1_filter);
    fprintf(fp,"conv_desc.feeding_buff_plane_loop_bound： %d \n",(int) conv_desc.feeding_buff_plane_loop_bound);
    fprintf(fp,"conv_desc.feeding_buff_row_loop_bound： %d \n",(int) conv_desc.feeding_buff_row_loop_bound);
    fprintf(fp,"conv_desc.straddle_in_plane_inc_by4： %d \n",(int) conv_desc.straddle_in_plane_inc_by4);
    fprintf(fp,"conv_desc.read_from_ddr： %d \n",(int) conv_desc.read_from_ddr);
    fprintf(fp,"conv_desc.write_to_ddr： %d \n",(int) conv_desc.write_to_ddr);
    fprintf(fp,"conv_desc.bn_en： %d \n",(int) conv_desc.bn_en);
    fprintf(fp,"conv_desc.bn_beta： %d \n",(int) conv_desc.bn_beta);
    fprintf(fp,"conv_desc.mode： %d \n",(int) conv_desc.mode);
    fprintf(fp,"conv_desc.elem_wise_offset： %d \n",(int) conv_desc.elem_wise_offset);
    fprintf(fp,"conv_desc.bn_mean_offset： %d \n",(int) conv_desc.bn_mean_offset);
    fprintf(fp,"conv_desc.intra_en： %d \n",(int) conv_desc.intra_en);
    fprintf(fp,"conv_desc.mul_in： %d \n",(int) conv_desc.mul_in);
    fprintf(fp,"conv_desc.norm_prec： %d \n",(int) conv_desc.norm_prec);
    fprintf(fp,"conv_desc.adder_loop_cnt： %d \n",(int) conv_desc.adder_loop_cnt);
    fprintf(fp,"conv_desc.ker_loop_cnt： %d \n",(int) conv_desc.ker_loop_cnt);
    fprintf(fp,"conv_desc.norm_prec_2： %d \n",(int) conv_desc.norm_prec_2);
    fprintf(fp,"conv_desc.norm_prec_3： %d \n",(int) conv_desc.norm_prec_3);
    fprintf(fp,"conv_desc.lrn_pns_enable： %d \n",(int) conv_desc.lrn_pns_enable);
    fprintf(fp,"conv_desc.norm_k： %d \n",(int) conv_desc.norm_k);
    fprintf(fp,"conv_desc.norm_loop_cnt： %d \n",(int) conv_desc.norm_loop_cnt);
    fprintf(fp,"conv_desc.norm_flag_set[4]： %d \n",(int) conv_desc.norm_flag_set[4]);
    fprintf(fp,"conv_desc.elem_wise_add_en： %d \n",(int) conv_desc.elem_wise_add_en);
    fprintf(fp,"conv_desc.bn_conv_fuse_en： %d \n",(int) conv_desc.bn_conv_fuse_en);
    fprintf(fp,"conv_desc.bn_snb_c_relu_en： %d \n",(int) conv_desc.bn_snb_c_relu_en);
    fprintf(fp,"conv_desc.bn_c_relu_en： %d \n",(int) conv_desc.bn_c_relu_en);
    fprintf(fp,"conv_desc.scale_relu_en： %d \n",(int) conv_desc.scale_relu_en);
    fprintf(fp,"conv_desc.conv_c_relu_en： %d \n",(int) conv_desc.conv_c_relu_en);
    fprintf(fp,"conv_desc.conv_c_relu_loop_cnt： %d \n",(int) conv_desc.conv_c_relu_loop_cnt);
    fprintf(fp,"conv_desc.int6_en_in： %d \n",(int) conv_desc.int6_en_in);
    fprintf(fp,"conv_desc.int6_en_out： %d \n",(int) conv_desc.int6_en_out);
    fprintf(fp,"conv_desc.int8_2x_loopbound： %d \n",(int) conv_desc.int8_2x_loopbound);
    fprintf(fp,"conv_desc.overflow_pattern： %d \n",(int) conv_desc.overflow_pattern);
    fprintf(fp,"conv_desc.overflow_pattern_bn： %d \n",(int) conv_desc.overflow_pattern_bn);
    fprintf(fp,"conv_desc.iw_mul_istg_row_count： %d \n",(int) conv_desc.iw_mul_istg_row_count);
    fprintf(fp,"conv_desc.ow_mul_ostg_row_count： %d \n",(int) conv_desc.ow_mul_ostg_row_count);
    fprintf(fp,"conv_desc.ow_mul_ostg_row_cnt_last_itr： %d \n",(int) conv_desc.ow_mul_ostg_row_cnt_last_itr);
    
    
    fprintf(fp,"conv_desc.feed_addr16：[ ");
    for(int i=0;i<16;i++) fprintf(fp,"%d ",(int) conv_desc.feed_addr16[i]);
    fprintf(fp,"]\n");

    fprintf(fp,"conv_desc.rounding_conv： %d \n",(int) conv_desc.rounding_conv);
    fprintf(fp,"conv_desc.rounding_bn： %d \n",(int) conv_desc.rounding_bn);
    fprintf(fp,"conv_desc.startrow_inc： %d \n",(int) conv_desc.startrow_inc);
    fprintf(fp,"conv_desc.pc_loop_bound_last_itr： %d \n",(int) conv_desc.pc_loop_bound_last_itr);
    fprintf(fp,"conv_desc.pc_loop_bound： %d \n",(int) conv_desc.pc_loop_bound);
    fprintf(fp,"conv_desc.fsz_by_stride_x_stride： %d \n",(int) conv_desc.fsz_by_stride_x_stride);
    fprintf(fp,"conv_desc.conv3d_intg_en： %d \n",(int) conv_desc.conv3d_intg_en);
    fprintf(fp,"conv_desc.conv3d_inp_offset： %d \n",(int) conv_desc.conv3d_inp_offset);
    fprintf(fp,"conv_desc.conv3d_ip_h： %d \n",(int) conv_desc.conv3d_ip_h);
    fprintf(fp,"conv_desc.conv3d_ip_w： %d \n",(int) conv_desc.conv3d_ip_w);
    fprintf(fp,"conv_desc.conv3d_ip_size： %d \n",(int) conv_desc.conv3d_ip_size);
    fprintf(fp,"conv_desc.conv3d_op_size： %d \n",(int) conv_desc.conv3d_op_size);
    fprintf(fp,"conv_desc.conv3d_op_h： %d \n",(int) conv_desc.conv3d_op_h);
    fprintf(fp,"conv_desc.conv3d_op_w： %d \n",(int) conv_desc.conv3d_op_w);
    fprintf(fp,"conv_desc.conv3d_ip_pln： %d \n",(int) conv_desc.conv3d_ip_pln);
    fprintf(fp,"conv_desc.conv3d_op_pln： %d \n",(int) conv_desc.conv3d_op_pln);
    fprintf(fp,"conv_desc.conv3d_pad： %d \n",(int) conv_desc.conv3d_pad);
    fprintf(fp,"conv_desc.conv3d_stride： %d \n",(int) conv_desc.conv3d_stride);
    fprintf(fp,"conv_desc.conv3d_ft_h： %d \n",(int) conv_desc.conv3d_ft_h);
    fprintf(fp,"conv_desc.conv3d_ft_w： %d \n",(int) conv_desc.conv3d_ft_w);
    fprintf(fp,"conv_desc.conv3d_fsz2： %d \n",(int) conv_desc.conv3d_fsz2);
    fprintf(fp,"conv_desc.conv3d_op_rows： %d \n",(int) conv_desc.conv3d_op_rows);
    fprintf(fp,"conv_desc.conv3d_istg_row_cnt： %d \n",(int) conv_desc.conv3d_istg_row_cnt);
    fprintf(fp,"conv_desc.conv3d_outshift： %d \n",(int) conv_desc.conv3d_outshift);
    fprintf(fp,"conv_desc.conv3d_relu_en： %d \n",(int) conv_desc.conv3d_relu_en);
    fprintf(fp,"conv_desc.batch1_int8_en： %d \n",(int) conv_desc.batch1_int8_en);
    fprintf(fp,"conv_desc.max_pool_en： %d \n",(int) conv_desc.max_pool_en);
    fprintf(fp,"conv_desc.avg_pool_en： %d \n",(int) conv_desc.avg_pool_en);
    fprintf(fp,"conv_desc.pool_fuse_en： %d \n",(int) conv_desc.pool_fuse_en);
    fprintf(fp,"conv_desc.avg_pool_mul： %d \n",(int) conv_desc.avg_pool_mul);
    fprintf(fp,"conv_desc.conv3d_rndval： %d \n",(int) conv_desc.conv3d_rndval);
    fprintf(fp,"conv_desc.fc_enable： %d \n",(int) conv_desc.fc_enable);
    fprintf(fp,"conv_desc.pool_ew： %d \n",(int) conv_desc.pool_ew);
    fprintf(fp,"conv_desc.scale_offset： %d \n",(int) conv_desc.scale_offset);
    fprintf(fp,"conv_desc.offline_quant_en： %d \n",(int) conv_desc.offline_quant_en);
}



void printInputDesc(input_struct & input_desc, FILE* fp)
{
    fprintf(fp,"input_desc.width： %d \n",(int) input_desc.width);
    fprintf(fp,"input_desc.height： %d \n",(int) input_desc.height);
    fprintf(fp,"input_desc.size： %d \n",(int) input_desc.size);
    fprintf(fp,"input_desc.planes： %d \n",(int) input_desc.planes);
    fprintf(fp,"input_desc.compute_planes： %d \n",(int) input_desc.compute_planes);
    fprintf(fp,"input_desc.mean_sub_flag： %d \n",(int) input_desc.mean_sub_flag);
    fprintf(fp,"input_desc.in_size_x_plane16： %d \n",(int) input_desc.in_size_x_plane16);
    fprintf(fp,"input_desc.row8_x_iw： %d \n",(int) input_desc.row8_x_iw);
}

void printOutputDesc(output_struct & output_desc, FILE* fp)
{
    fprintf(fp,"output_desc.width： %d \n",(int) output_desc.width);
    fprintf(fp,"output_desc.height： %d \n",(int) output_desc.height);
    fprintf(fp,"output_desc.size： %d \n",(int) output_desc.size);
    fprintf(fp,"output_desc.planes： %d \n",(int) output_desc.planes);
    fprintf(fp,"output_desc.out_offset： %d \n",(int) output_desc.out_offset);
    fprintf(fp,"output_desc.bias_offset： %d \n",(int) output_desc.bias_offset);
    fprintf(fp,"output_desc.out_size_x_plane16： %d \n",(int) output_desc.out_size_x_plane16);
    fprintf(fp,"output_desc.crelu_offset_ptr0： %d \n",(int) output_desc.crelu_offset_ptr0);
    fprintf(fp,"output_desc.crelu_offset_ptr1： %d \n",(int) output_desc.crelu_offset_ptr1);
    fprintf(fp,"output_desc.pix_cnt： %d \n",(int) output_desc.pix_cnt);
}


void printWeightDesc(weight_struct & weight_desc, FILE* fp)
{
    fprintf(fp,"weight_desc.num_kernels： %d \n",(int) weight_desc.num_kernels);
    fprintf(fp,"weight_desc.filter_size： %d \n",(int) weight_desc.filter_size);
    fprintf(fp,"weight_desc.filter_size_square： %d \n",(int) weight_desc.filter_size_square);
    fprintf(fp,"weight_desc.filter_stride： %d \n",(int) weight_desc.filter_stride);
    fprintf(fp,"weight_desc.nkpf： %d \n",(int) weight_desc.nkpf);
    fprintf(fp,"weight_desc.slk_en： %d \n",(int) weight_desc.slk_en);
    fprintf(fp,"weight_desc.slk_counter： %d \n",(int) weight_desc.slk_counter);
    fprintf(fp,"weight_desc.weight_offset： %d \n",(int) weight_desc.weight_offset);
    fprintf(fp,"weight_desc.dilation_factor： %d \n",(int) weight_desc.dilation_factor);
    fprintf(fp,"weight_desc.filter_size_dilated： %d \n",(int) weight_desc.filter_size_dilated);
    fprintf(fp,"weight_desc.wts_loop_cnt： %d \n",(int) weight_desc.wts_loop_cnt);
    fprintf(fp,"weight_desc.wts_offset_scale： %d \n",(int) weight_desc.wts_offset_scale);
}



void printGroupDesc(group_conv_struct & group_desc, FILE* fp)
{
    fprintf(fp,"group_desc.input_1st_offset %d \n",(int) group_desc.input_1st_offset);
    fprintf(fp,"group_desc.input_other_offset %d\n",(int) group_desc.input_other_offset);
    fprintf(fp,"group_desc.weight_offset %d \n",(int) group_desc.weight_offset);
    fprintf(fp,"group_desc.output_offset %d \n",(int) group_desc.output_offset);
    fprintf(fp,"group_desc.bias_offset %d \n",(int) group_desc.bias_offset);
}


void printDesc(
                conv_struct & conv_desc,
                input_struct & input_desc,
                output_struct & output_desc,
                weight_struct & weight_desc,
                group_conv_struct & group_desc,
                FILE* fp)
{
    printConvDesc(conv_desc,fp);
    printInputDesc(input_desc,fp);
    printOutputDesc(output_desc,fp);
    printWeightDesc(weight_desc,fp);
    printGroupDesc(group_desc, fp);
}