
import os


def geneDefineMacro( ibuffersize, obuffersize, wbuffersize, ker, proc):
    f = open('./conv_pipe/include/defines.h',"w");
    f.write("#define INBUFFERSIZE\t"+str(ibuffersize)+"\n" );
    f.write("#define OUTBUFFERSIZE\t"+str(obuffersize)+"\n"  );
    f.write("#define WEIGHTBUFFERSIZE\t"+str(wbuffersize)+"\n"  );
    f.write("#define KERSIZE\t"+str(ker)+"\n" );
    f.write("#define PROCSIZE\t"+str(proc)+"\n"  );
    f.close();

def genTclScript(ibuffersize, obuffersize, wbuffersize, ker, proc):
    f = open('./scriptgen.tcl',"w");
    f.write("open_project CHaiPipeline\n")
    f.write("set_top XiConvolutionTop\n")
    # f.write("add_files ./conv_pipe/tb/dbg.h\n");
    f.write("add_files ./conv_pipe/tb/testbench_defines.h\n");
    f.write("add_files ./conv_pipe/tb/verification.cpp\n");
    f.write("add_files ./conv_pipe/src/xi_convolution.hpp\n");
    f.write("add_files ./conv_pipe/src/xi_convolution_top.cpp\n");
    f.write("add_files ./conv_pipe/include/defines.h\n");
    f.write("add_files ./conv_pipe/include/xi_conv_config.h\n");
    f.write("add_files ./conv_pipe/include/xi_conv_desc.h\n");
    f.write("open_solution \"Sol_"+str(ibuffersize)+"_"+str(obuffersize)+"_"+str(wbuffersize)+"_"+str(ker)+"_"+str(proc)+"\"\n");
    f.write("set_part {xczu9eg-ffvb1156-2-i}\n");
    f.write("create_clock -period 10 -name default\n");
    f.write("csynth_design\n")
    f.write("export_design -flow syn -rtl verilog -format ip_catalog\n");
    f.close()

ibufferArray=[512,2048,8192]
obufferArray=[512,2048]
wbufferArray=[512,1024]
kelsize = [8,16]
procsize = [8,32]


geneDefineMacro(512,512,512,8,8)
genTclScript(512,512,512,8,8)
os.system("/opt/xilinx/sdsoc/2018.2/Vivado/2018.2/bin/./vivado_hls scriptgen.tcl");


