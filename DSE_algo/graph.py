import matplotlib.pyplot as plt
import networkx as nx
import cvxpy as cvx

class layer:
    """
    The class to discribe attributes relate to layers
    Attrs:
        name: The name of the layer
        type: The type of the layer
        params: The parameter list of the layers, according to different type, the list
        has different interpretation.
            Conv:   [Filter(cout x cin x fh x fw), Stride, Padding, DilationFactor, 
                    Group number, ReLU, HasBias]
            Pool:   

        input_params: The input dimension of the layer [batch, channel, height, width]
        output_params: The output dimension of the layer[batch, channel, height, width]
        mappedIP: The mapped ip of this layer
        latency: The latency of finishing this layer using a specific IP
        pipelinedLatency: The pipelined latency of finishing enough rows to compute one row of output
    Methods:
        set_input_params: Set input dimentions
        set_output_params: set output related parameters, e.g., dimensions
        set_IP: set the mapped IP for this layer
    """
    def __init__(self, line):
        """
        Constructor
        Args: 
            line: the str that contains information of each layer
        """
        n_t = line.split(":")[0]
        self.name, self.type = n_t.split("-")
        #FIXME: The following are currently left blank, whether this is the best?
        self.mappedIP = None

        if(self.type == "XPack Layer"):
            self.params = line.split(":")[2]
        else:
            self.params = line.split(":")[1].split(";")

    def set_IP(self, IP):
        """
        set the mapped IP for this layer
        Args:
            IP: The object IP. The IP this layer is mapped to.
        """
        self.mappedIP = IP

    def set_input_params(self, line):
        """
        The function sets the input parameters
        """
        self.input_params = map(int,line.split("x")) #[batch, channel, height, width]
    def set_output_params(self, line):
        """
        The function sets the output parameters
        """
        self.output_params = map(int,line.split("x")) #[batch, channel, height, width]

    #FIXME: Need to implement the following functions
    def assign_IP(self, sol):
        """
        Assign the IP that is mapped to this layer
        """
        self.IP_id = None

    def computeLatency(self):
        """
        Compute the full latency of this layer using one IP and, 
        compute the pipelined latency, which is the time to compute one output row
        """
        assert (self.mappedIP is not None), self.name + " mapped IP is not decided, so no way to compute the latency"

        if self.mappedIP == "Software":
            print self.name, self.latency, self.pipelinedLatency
            return

        in_height, in_width = map(int, self.input_params[2:4])
        out_height, out_width = map(int, self.output_params[2:4])



        if self.type == "Convolution":
            cout, cin, kw, kh = map(int, (self.params[0].split("=")[1]).split("x"))
            #FIXME: what is the best way to pass in the parameters
            self.latency = self.mappedIP.computeLatency(
                    [cout, cin, kw, kh],
                    in_width,
                    in_height
                    ) 
            self.pipelinedLatency = self.mappedIP.computeLatency(
                    [cout, cin, kw, kh],
                    in_width,
                    kh #FIXME: Is is always the k_h rows needed to compute one row)
                    ) 
            return 
        if self.type == "Pooling":
            PoolType = self.params[0].split("=")[1]
            N = int(self.params[1].split("=")[1])
            K = int(self.params[2].split("=")[1])
            S = int(self.params[3].split("=")[1])
            P = int(self.params[4].split("=")[1])
            self.latency = self.mappedIP.computeLatency(
                    [N,K,S,P], in_width, in_height)
            self.pipelinedLatency = self.mappedIP.computeLatency(
                    [N,K,S,P], in_width, K)
            return
        assert 0, "This layer has unsupported type"

class graph:
    """
    The class that contains the graph
    Atrributes:
            G: The graph
            mapping: A dictionary between the nodes in the graph and a name. This is for drawing the pictures.
            SWMapping: If a layer is mapped to software: A dictionary between the node name and the latency
            layerQueue: The dictionary. key: The layer type. Value: The list of layers that fall into that category
    Methods:
            construct: to construct the graph from a file that is dumped from CHaiDnn
    """
    def __init__(self, filename):
        self.G = nx.DiGraph()
        self.mapping = dict()
        self.SWMapping = dict()
        self.layerQueue = dict()
        self.construct(filename)

    def construct(self, filename):
        """
        The function to construct the graph from a file that is dumped from CHaiDNN
        Args:
            filename: the name of the file
        """
        bottom_table = dict()
        top_table = dict()
        f = open(filename)
        for l in f:
            l = l.replace(" ", "")
            if l.find("--Layers--") >= 0:
                for l in f:
                    l = l.replace(" ", "")
                    if l == "Xilinx\n":
                        continue
                    if l == "\n":
                        break
                    bot_str, layer_str, top_str = l.split("-->")
                    bot_str = bot_str[1:-1].split(":")[0]
                    top_str = top_str[1:-1].split(":")[0]

                    layer_str = layer_str[1:-1]
                    layer_tmp = layer(layer_str)

                    if bot_str in bottom_table:
                        bottom_table[bot_str].append(layer_tmp)
                    else:
                        bottom_table[bot_str] = [layer_tmp]

                    if top_str in top_table:
                        top_table[top_str].append(layer_tmp)
                    else:
                        top_table[top_str] = [layer_tmp]

                    self.G.add_node(layer_tmp)
                    self.layerQueue[layer_tmp.type] = [layer_tmp] if layer_tmp.type not in self.layerQueue else self.layerQueue[layer_tmp.type] + [layer_tmp]
                    self.mapping[layer_tmp] = layer_tmp.name
            if l.find("--Blobs--") >= 0:
                for l in f:
                    l = l.replace(" ", "")
                    if l == "\n":
                        break
                    blobname, dims =l.split(":")
                    if blobname in bottom_table:
                        for ll in bottom_table[blobname]:
                            ll.set_input_params(dims)
                    if blobname in top_table:
                        for ll in top_table[blobname]:
                            ll.set_output_params(dims)

            if l.find("--Softwarelayerlatency--") >= 0:
                for l in f:
                    if l == "\n":
                        break
                    l = l.replace(" ", "")[0:-1]
                    layer_name, layer_lat = l.split(":")
                    #FIXME: should it be int??
                    self.SWMapping[layer_name] = int(layer_lat)
        f.close()

        #Build Edges
        for bb in bottom_table:
            if bb in top_table:
                for bbb in bottom_table[bb]:
                    for ttt in top_table[bb]:
                        self.G.add_edge(bbb, ttt)
    def __str__(self):
        retStr = " "
        for layer_type in self.layerQueue:
            Str = layer_type + ": "
            for layer_inst in self.layerQueue[layer_type]:
                mappedIPName = layer_inst.mappedIP.name if (layer_inst.mappedIP is not None and layer_inst.mappedIP is not "Software") else "Software"
                Str += (layer_inst.name+" assigned IP: "+ mappedIPName + " ")
            retStr += (Str+"\n")
        return retStr
    
    def drawGraph(self):
        h = nx.relabel_nodes(self.G, self.mapping)
        nx.draw(h, with_labels=True, font_weight = 'bold')

    def computeLatency(self):
        """
        For each node in the graph, compute the latency and pipelined latency
        """
        print self.SWMapping
        for n in self.G.nodes:
            #If a layer is mapped to software
            if n.name in self.SWMapping: 
                n.set_IP("Software")
                n.latency = int(self.SWMapping[n.name])
                n.pipelinedLatency = int(self.SWMapping[n.name])
            else:
                n.computeLatency()
            
    def printNodeLatency(self):
        for n in self.G.nodes:
            print n.name, " ", n.latency, " ", n.pipelinedLatency
