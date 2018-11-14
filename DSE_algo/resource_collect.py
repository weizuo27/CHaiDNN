fileTitle = ["ID", "TYPE", "BRAM", "DSP", "FF", "LUT", "BW", "Latency", "XI_KER_PROC", "XI_PIX_PROC", "XI_WEIGHTBUFF_DEPTH"]
writeList =[fileTitle]
IP_TYPE1 = "Convolution"
IP_TYPE2 = "Convolution_g"

ibufferArray=[2048, 4096, 8192]
obufferArray=[512,1024,2048]
wbufferArray=[512,1024]
kelsize = [8,16]
procsize = [8,16,32]

idx = 0
BW = str(1)
Latency = str(1)

fileName_w = "IP_config_w"

#Collect the list of strings to write into the config file
for ibuffersize in ibufferArray:
    for obuffersize in obufferArray:
        for wbuffersize in wbufferArray:
            for ker in kelsize:
                for proc in procsize:
                    idx += 1;
                    sol_name = "Sol_" + str(ibuffersize)+"_"+str(obuffersize)+"_"+str(wbuffersize)+"_"+str(ker)+"_"+str(proc)
                    report_name = "./reports/" + sol_name+ "_export.rpt"
                    print idx, report_name

                    with open(report_name, 'r') as f:
                        lines = f.read().split("\n") 

                    resource_lines = []
                    word = "Post-Synthesis" 
                    for idxx, line in enumerate(lines):
                        if word in line:
                            resource_lines += lines[idxx+2:idxx+6]
                            break 

                    ID = "IP"+ str(idx)
                    resource_list = []
                    for line in resource_lines:
                        resource_list.append(line.replace(" ", "").split(":")[1]) 

                    LUT, FF, DSP, BRAM = resource_list
                    writeList.append([ID, IP_TYPE1, BRAM, DSP, FF, LUT, BW, Latency, str(ker), str(proc), str(wbuffersize)]) 


for ibuffersize in ibufferArray:
    for obuffersize in obufferArray:
        for wbuffersize in wbufferArray:
            for ker in kelsize:
                for proc in procsize:
                    idx += 1;
                    sol_name = "Sol_" + str(ibuffersize)+"_"+str(obuffersize)+"_"+str(wbuffersize)+"_"+str(ker)+"_"+str(proc)
                    report_name = "./reports/" + sol_name+ "_export.rpt"
                    print idx, report_name

                    with open(report_name, 'r') as f:
                        lines = f.read().split("\n") 

                    resource_lines = []
                    word = "Post-Synthesis" 
                    for idxx, line in enumerate(lines):
                        if word in line:
                            resource_lines += lines[idxx+2:idxx+6]
                            break 

                    ID = "IP"+ str(idx)
                    resource_list = []
                    for line in resource_lines:
                        resource_list.append(line.replace(" ", "").split(":")[1]) 

                    LUT, FF, DSP, BRAM = resource_list
                    writeList.append([ID, IP_TYPE2, str(int(BRAM)*2), str(int(DSP)*2), str(int(FF)*2), str(int(LUT)*2), str(int(BW)*2), str(int(Latency)*2),\
                            str(int(ker) * 2), str(proc*2), str(wbuffersize *2)]) 
#print writeList
fw = open(fileName_w, 'w') 

for l in writeList:
    fw.write(", ".join(l) + "\n")

l = ["IP"+str(idx+1), "Pooling", "81", "34", "9475", "7511", "1", "279", "1", "1", "1"]
fw.write(", ".join(l) + "\n")
fw.close()        