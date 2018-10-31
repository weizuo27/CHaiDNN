#ifndef _PRINT_PARAM_H_
#define _PRINT_PARAM_H_
#include "../include/xi_conv_config.h"
#include "../include/xi_conv_desc.h"
#include <stdio.h>
void printDesc(
                conv_struct & conv_desc,
                input_struct & input_desc,
                output_struct & output_desc,
                weight_struct & weight_desc,
                FILE* fp);

#endif //_PRINT_PAPRM_H_
