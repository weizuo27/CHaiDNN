#The Resource budget
BRAM_budget = 20
DSP_budget = 30
FF_budget = 30
LUT_budget = 30

BW_budget = 100 

#The class of pipeline node
class pipelineNode():
    __init__(self, latency):
        assert (latency < 0), "The latency of pipelineNode should be negative!"
        self.latency = latency


#IP_tables, this is just a dummy example, later will be converted 
#to function to read in the characterization file

IP1 = IP("conv",[10,10, 10, 10, 10], 20)
IP2 = IP("conv", [15, 15, 15, 15, 15], 15)
IP3 = IP("conv", [20, 20, 20, 20, 15], 10)

IP4 = IP("pool",[10,10, 10, 10, 10], 20)
IP5 = IP("pool", [15, 15, 15, 15, 15], 15)
IP6 = IP("pool", [20, 20, 20, 20, 20], 10)

IPs = [IP1, IP2, IP3, IP4, IP5, IP6]
