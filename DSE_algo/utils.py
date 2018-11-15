from IP import IP
from copy import deepcopy
def constructIPTable(IPs, BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget):
    '''
    To construnct the IP_table.
    Basically, according to the resource consumption of each IP, the maximum numbers of IPs of that type can be allocated is
    min(floor(*_budget/IP.*_budget))
    Args: 
        IPs: The list of IPs which are available to choose from
        *_budget: The budget of each resource type
    return: 
        IP_table: The dictionary. Key is the IP_type. Value is a list of IP * (number of IPs that can fit into the FPGA)
    '''
    IP_table = dict()

    for ip in IPs:
        # TODO: currently the number of IPs is calculated as: if only this IP is instantiated, what is the number
        # An optimization is, assume the smallest IP of each category is generated once, and the number of IPs of each
        # Category. E.g., 1 smallest Pool and 1 smallest Conv have to be instantiated for functionality. So the number
        # of Pools should be (total_resource - smallest_conv_resource)/pool_resource. This should reduce the number
        # of variables
        IP_num = min(BRAM_budget/ip.BRAM, DSP_budget/ip.DSP, FF_budget/ip.FF, LUT_budget/ip.LUT, BW_budget/ip.BW)
        IP_type = ip.type
        #IP_table[IP_type] = [ip] * IP_num if IP_type not in IP_table else IP_table[IP_type] + [ip] * IP_num
        for i in range(IP_num):
            new_ip = deepcopy(ip)
            new_ip.name += str(i)
            if IP_type in IP_table:
                IP_table[IP_type].append(new_ip)
            else:
                IP_table[IP_type] = [new_ip]
    return IP_table

def generateIPs(fileName):
    """
    To generate the list of IPs from the IP_config file.
    Args:
        fileName: The string, indicate the IP_config file name
    Return:
        IPs: The list of IPs that are to be considered
    """
    IPs = list()
    f = open(fileName, "r")
    next(f) #Skip the first line since the first line is just the name of each column
    for l in f:
        IP_name, layer_type, BRAM, DSP, FF, LUT, BW, baseLat, XI_KER_PROC, XI_PIX_PROC, XI_WEIGHTBUFF_DEPTH \
    = (l.replace(" ", "")).split(",")
        IP_inst = IP(IP_name, layer_type, map(int, [BRAM, DSP, FF, LUT, BW]), int(baseLat), 
                [XI_KER_PROC, XI_PIX_PROC, XI_WEIGHTBUFF_DEPTH])
        IPs.append(IP_inst)
    f.close()
    return IPs

def printViolationPath(vioPath):
    print "Violation path"
    for i in vioPath:
        print i.name, "-->"
        
def isPipelined(s_node, t_node):
    """
    The function to check whether two nodes are pipelined
    Args:
        s_node: The source node 
        t_node: The target node
    Return:
        bool: True if they are pipelined, False otherwise
    """

    if s_node.mappedIP.type == "Software":
        return False
    if t_node.mappedIP.type == "Software":
        return False
    return s_node.mappedIP != t_node.mappedIP
