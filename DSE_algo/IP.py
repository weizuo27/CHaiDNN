#The class of IP
from lat_estimate import computeLatency
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
    def __init__(self, name, type, resource_list, baseLat, 
            #The following are chaiDNN conv specific configs
            paramList,
            ):
        self.name = str(name)
        self.type = str(type)
        self.orig_name = str(name)
        self.BRAM, self.DSP, self.FF, self.LUT, self.BW= map(int, resource_list)
        self.BaseLat = int(baseLat)
        self.paramList =map(int, paramList)

    #This actually needs to be overide by different IP types
    #This function should give latency of the using the IP with a 
    #sepcified application dimensions
    def computeLatency(self, paramList, 
            in_height, in_width, 
            out_height, out_width,
            ):
        """
        Based on the passed-in list, can compute the latency
        Args:
            paramList: The list of parameters representing the dimension of one layer
                if type is Convolution, the list is [cout, cin, kw, kh, S, padding, group]
                if type is Pool: list is [N, kh, S, P]
            in_height: The input height
            in_width : The input width
            out_height: The output height
            out_width: The output width
        Return:
            The latency
        """
        #FIXME: This needs to fill in the real computation later
        if self.type == "Convolution" or self.type == "Convolution_g":
            #dcode the parameter:
            cout, cin, kw, kh, S, padding, group = paramList

            XI_KER_PROC, XI_PIX_PROC, XI_IBUFF_DEPTH, \
            XI_OBUFF_DEPTH, XI_WEIGHTBUFF_DEPTH = self.paramList

            lat = computeLatency(
                    int(in_height),
                    int(in_width), 
                    int(out_height), 
                    int(out_width),
                    int(cout/group),
                    int(cin), 
                    int(S), int(kh), int(kw), int(padding),
                    int(1),  #group
                    1,
                    int(XI_KER_PROC),
                    int(XI_PIX_PROC),
                    int(XI_WEIGHTBUFF_DEPTH)
                    )

            return lat
            #return self.BaseLat * cout * cin * n_row * n_col
        elif self.type == "Pooling":
            return self.BaseLat * out_height

    def __str__(self):
        return "name: "+str(self.name)+" Type: "+str(self.type)+" BRAM: "+str(self.BRAM)+" DSP: "+ \
        str(self.DSP)+" FF: "+str(self.FF)+" LUT: "+str(self.LUT)

class softwareIP:
    def __init__(self, name):
        self.type = "Software"
        self.name = name
