import os
ibuffersize=[512, 1024, 2048, 4096, 8192]
obufferArray=[512,1024,2048]
wbufferArray=[512,1024]
kelsize = [8,16]
procsize = [8,16,32]

for idx, ibuffersize in enumerate(ibuffersize):
    for obuffersize in obufferArray:
        for wbuffersize in wbufferArray:
            for ker in kelsize:
                for proc in procsize:
                    sol_name = "Sol_" + str(ibuffersize)+"_"+str(obuffersize)+"_"+str(wbuffersize)+"_"+str(ker)+"_"+str(proc)
                    report_path = "../profile"+str(idx + 1) + "/CHaiPipeline/" + sol_name + "/impl/report/verilog/XiConvolutionTop_export.rpt"
                    rename_report = sol_name+ "_export.rpt"
                    cmd = "cp " + report_path + " " + rename_report
		    #print cmd
                    os.system(cmd)
