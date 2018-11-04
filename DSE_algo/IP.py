#The class of IP
class IP():
    """
    The class that describes the IP 
    Attrs:
        type: The type of the IP
        BRAM, DSP, FF, LUT: The number of resources consumed by this IP
        BaseLat: The latency of the IP to compute one tile
        orig_name: String. The original name of the IP. Since IP later can
        be renamed, we would like to record the original name also
    """
    def __init__(self, name, type, resource_list, baseLat, orig_inst):
        self.name = str(name)
        self.type = str(type)
        self.orig_name = str(name)
        self.BRAM, self.DSP, self.FF, self.LUT, self.BW= map(int, resource_list)
        self.BaseLat = int(baseLat)

    #This actually needs to be overide by different IP types
    #This function should give latency of the using the IP with a 
    #sepcified application dimensions
    def computeLatency(self, paramList, n_col, n_row):
        """
        Based on the passed-in list, can compute the latency
        Args:
            paramList: The list of parameters representing the dimension of one layer
            n_col: The number of output columns to compute
            n_row: The number of output rows to compute
        Return:
            The latency
        """
        #FIXME: This needs to fill in the real computation later
        if self.type == "Convolution":
            #dcode the parameter:
            cout, cin, kw, kh = paramList
            return self.BaseLat * cout * cin * n_row * n_col
        elif self.type == "Pooling":
            return self.BaseLat * n_col * n_row

    def __str__(self):
        return "name: "+str(self.name)+" Type: "+str(self.type)+" BRAM: "+str(self.BRAM)+" DSP: "+ \
        str(self.DSP)+" FF: "+str(self.FF)+" LUT: "+str(self.LUT)
