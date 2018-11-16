from IP import IP
import math
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
    #NOTE: Assume ALL the IPs in the table, the types appear in the the application.
    #Namely, there is no type of the IP that cannot be used in the application

    IP_table = dict()

    minResourceIP = dict() #The minimum resource for each IP. key: IP_type. value [BRAM, DSP, FF, LUT, BW]

    for ip in IPs:
        if ip.type not in minResourceIP:
            minResourceIP[ip.type] = [ip.BRAM, ip.DSP, ip.FF, ip.LUT, ip.BW]
        else:
            minBRAM, minDSP, minFF, minLUT, minBW = minResourceIP[ip.type]
            minResourceIP[ip.type] = [min(minBRAM, ip.BRAM), min(minDSP, ip.DSP), min(minFF, ip.FF), min(minLUT, ip.LUT), min(minBW, ip.BW)]

    minBRAM_total = 0;
    minDSP_total = 0;
    minFF_total = 0;
    minLUT_total = 0;
    minBW_total = 0;
    
    for t in minResourceIP:
        minBRAM_total += minResourceIP[t][0]
        minDSP_total += minResourceIP[t][1]
        minFF_total += minResourceIP[t][2]
        minLUT_total += minResourceIP[t][3]
        minBW_total += minResourceIP[t][4]
    
    print minBRAM_total, minDSP_total, minFF_total, minLUT_total, minBRAM_total

    for ip in IPs:
        # TODO: currently the number of IPs is calculated as: if only this IP is instantiated, what is the number
        # An optimization is, assume the smallest IP of each category is generated once, and the number of IPs of each
        # Category. E.g., 1 smallest Pool and 1 smallest Conv have to be instantiated for functionality. So the number
        # of Pools should be (total_resource - smallest_conv_resource)/pool_resource. This should reduce the number
        # of variables
        BRAM_budget_local = BRAM_budget - (minBRAM_total - minResourceIP[ip.type][0])
        DSP_budget_local = DSP_budget - (minDSP_total - minResourceIP[ip.type][1])
        FF_budget_local = FF_budget - (minFF_total - minResourceIP[ip.type][2])
        LUT_budget_local = LUT_budget - (minLUT_total - minResourceIP[ip.type][3])
        BW_budget_local = BW_budget - (minBW_total - minResourceIP[ip.type][4])

        IP_num = min(BRAM_budget_local/ip.BRAM, DSP_budget_local/ip.DSP, FF_budget_local/ip.FF, LUT_budget_local/ip.LUT, BW_budget_local/ip.BW)
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
        IP_name, layer_type, BRAM, DSP, FF, LUT, BW, baseLat, XI_KER_PROC, XI_PIX_PROC, \
        XI_IBUFF_DEPTH, XI_OBUFF_DEPTH, XI_WEIGHTBUFF_DEPTH  = (l.replace(" ", "")).split(",")
        IP_inst = IP(IP_name, layer_type, map(int, [BRAM, DSP, FF, LUT, BW]), int(baseLat), 
                [XI_KER_PROC, XI_PIX_PROC, XI_IBUFF_DEPTH, XI_OBUFF_DEPTH, XI_WEIGHTBUFF_DEPTH])
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

def resourceConstr(layer, ip):
    """
        Function to generate CHaiDNN specific constraints about the buffer size.
        The following must be satisified for correct functionality
        
        inWidth*ceil(indepth/32)*(filterheight+stride) < XI_ISTGBUFFDEPTH*2
        outWidth*ceil(outdepth/32) < XI_OSTGBUFFDEPTH*2
    """

    assert(layer.type == "Convolution" or layer.type == "Convolution_g"), "Unsupported layer type"

    const = [] 
    in_depth, in_height, in_width = map(int, layer.input_params[1:4])
    out_depth, out_height, out_width = map(int, layer.output_params[1:4])
    cout, cin, kw, kh = map(int, (layer.params[0].split("=")[1]).split("x"))
    S = int(layer.params[1].split("=")[1])

    #FIXME: This 32 may need to be changed later, should not be fixed
    const.append(math.ceil(in_width * math.ceil(float(in_depth)/32) * (kh + S) / 2))
    const.append(math.ceil(out_width * math.ceil(float(out_depth)/32) /2))

    return const
    

