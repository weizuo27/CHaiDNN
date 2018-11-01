#The Resource budget
BRAM_budget = 20
DSP_budget = 30
FF_budget = 30
LUT_budget = 30

BW_budget = 100 

#The class of IP
class IP():
    def __init__(self, type, resource_list, latency):
        self.type = type
        self.BRAM, self.DSP, self.FF, self.LUT, self.BW= resource_list
        '''
        Latency parameters need to be added in later
        '''


#IP_tables, this is just a dummy example, later will be converted 
#to function to read in the characterization file

IP1 = IP("conv",[10,10, 10, 10, 10], 20)
IP2 = IP("conv", [15, 15, 15, 15, 15], 15)
IP3 = IP("conv", [20, 20, 20, 20, 15], 10)

IP4 = IP("pool",[10,10, 10, 10, 10], 20)
IP5 = IP("pool", [15, 15, 15, 15, 15], 15)
IP6 = IP("pool", [20, 20, 20, 20, 15], 10)

IPs = [IP1, IP2, IP3, IP4, IP5, IP6]

#
def constructIPTable(IPs):
    '''
    To construnct the IP_table.
    Args: 
        IPs: The list of IPs which are available to choose from
    return: 
        IP_table: The dictionary. Key is the IP_type. Value is a list of IP * (number of IPs that can fit into the FPGA)
    '''
    IP_table = dict()

    for IP in IPs:
        IP_num = min(BRAM_budget/IP.BRAM, DSP_budget/IP.DSP, FF_budget/IP.FF, LUT_budget/IP.LUT, BW_budget/IP.BW)
        IP_type = IP.type
        IP_table[IP_type] = [IP] * IP_num if IP_type not in IP_table else IP_table[IP_type] + [IP] * IP_num

    return IP_table

