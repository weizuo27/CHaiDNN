import matplotlib.pyplot as plt
from copy import deepcopy
import networkx as nx
import cvxpy as cvx

class pipeNode:
    idx = 0
    def __init__(self, neg_latency):
        self.name = "pipeNode" + str(pipeNode.idx)
        self.type = "pipeNode"
        self.mappedIP = None
        pipeNode.idx+=1
        self.latency = neg_latency
        self.lat_one_row = neg_latency

class joinNode:
    idx = 0
    def __init__(self):
        self.name = "joinNode" + str(joinNode.idx)
        self.type = "joinNode"
        self.mappedIP = None
        joinNode.idx += 1
        self.latency = 0
        self.lat_one_row = 0
    def computeLatencyOneRow(self, , fake2):
        return self.lat_one_row

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
        lat_one_row: The latency to compute one output row
        latency: The total latency to compute this layer
        start_time: The time stamp that the layer start execution
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
        self.lat_one_row = None
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

    def computeLatencyIPOneRow(self):
        """
        The latency to compute one row using a mapped IP
        """

        assert (self.mappedIP is not None), self.name + " mapped IP is not decided,\
            so no way to compute the latency"
        #FIXME: If it is mapped to software, it should not be pipelined?
        # So there is not point of computing one row
        if self.mappedIP == "Software":
            return

        in_height, in_width = map(int, self.input_params[2:4])
        out_height, out_width = map(int, self.output_params[2:4])

        #Assumption: S < W, which is the case of most NN
        if self.type == "Convolution":
            cout, cin, kw, kh = map(int, (self.params[0].split("=")[1]).split("x"))
            S = int(self.params[1].split("=")[1])
            padding = int(self.params[2].split("=")[1])
            group = int(self.params[4].split("=")[1])
            #FIXME: what is the best way to pass in the parameters

            #TODO: Assumption: Now here we do not consider the warm-up phase of computation.
            #E.g., a convolution. Stride =4, kh =11. For the first output row, it needs
            #to compute 11 input rows, but for the remaining rows,  it only need 4. 
            #We neglect the first row, and assume that one output row requires 4 input row.
            IP_latency = self.mappedIP.computeLatency(
                    [cout, cin, kw, kh, S, padding, group],
                    in_height, 
                    in_width, 
                    out_height, 
                    out_width) 

        elif self.type == "Pooling":
            PoolType = self.params[0].split("=")[1]
            N = int(self.params[1].split("=")[1])
            kw = kh = int(self.params[2].split("=")[1])
            S = int(self.params[3].split("=")[1])
            P = int(self.params[4].split("=")[1])
            IP_latency = self.mappedIP.computeLatency(
                    [N,kh,S,P], 
                    in_height, 
                    in_width, 
                    out_height, 
                    out_width)
        else:
            assert 0, "This layer has unsupported type"

    def computeLatencyOneRow(self, prevLayer, pipelined):
        """
        The latency to compute one row
        Args:
            prevLayer: one previous layer
            pipelined: Bool. Whether the previous layer and current layer are pipelined
        """
        #print self.name, self.mappedIP
        assert (self.mappedIP is not None), self.name + " mapped IP is not decided,\
            so no way to compute the latency"
        IP_latency = self.computeLatencyIPOneRow()

        if(prevLayer == None) or (not pipelined):
            self.lat_one_row = IP_latency
        else:
            if self.lat_one_row == None:
                self.lat_one_row = max(IP_latency, prevLayer.computeNRows(S))
            else:
                self.lat_one_row = max(self.lat_one_row, prevLayer.computeNRows(S))

    def computeNRows(self, n):
        """
        return the latency of the compute n rows
        Args:
            n: int. The number of rows to compute
        return:
            the latency to compute n rows
        """
        assert (self.mappedIP is not None), self.name + " mapped IP is not decided,\
            so no way to compute the latency of n rows"
        if self.mappedIP == "Software":
            return
        assert (self.lat_one_row != None), "The lat_one_row is not computed, cannot compute \
        n rows"
        return self.lat_one_row * n

    def computeLatency(self):
        """
        Compute the full latency of this layer using one IP
        """
        assert (self.mappedIP is not None), self.name + " mapped IP is not decided, \
        so no way to compute the latency"

        #The software latency is directly computed from the log file
        if self.mappedIP == "Software":
            return
        out_height, out_width = map(int, self.output_params[2:4])

        self.latency = self.computeNRows(out_height)

    def set_start_time(self, timeStamp):
        """
        Set the start time of this layer
        Args:
            timeStamp: FP data. The starting time
        """
        self.start_time = timeStamp

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
        self.original = nx.create_empty_copy(self.G)
        self.original_nodes = list(self.G.nodes)
        self.original_edges = list(self.G.edges)

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
                        self.G.add_edge(ttt, bbb) 

        self.splitGroupNode()

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
        plt.show()

    def computeLatency(self):
        """
        For each node in the graph, compute the latency and pipelined latency
        """
        print self.SWMapping
        node_list = self.topological_sort()
        for n in node_list:
            #If a layer is mapped to software
            if n.name in self.SWMapping: 
                n.set_IP("Software")
                n.latency = int(self.SWMapping[n.name])
                n.lat_one_row = int(self.SWMapping[n.name]) #FIXME: False name 
            else:
                predList = list(self.G.predecessors(n))
                if len(predList) == 0:
                    n.computeLatencyOneRow(None, False)
                for pred in predList:
                    n.computeLatencyOneRow(pred, (pred.mappedIP != n.mappedIP))

                n.computeLatency()
            
    def printNodeLatency(self):
        for n in self.G.nodes:
            print n.name, " ", n.latency, " ", n.lat_one_row
    def printNodeParameters(self):
        for n in self.G.nodes:
            if(n.type == "Convolution" or n.type == "Pooling"):
                print n.name, " ", n.params, n.input_params, n.output_params

    def add_node(self, node):
        self.G.add_node(node)
        self.mapping[node] = node.name

    def topological_sort(self):
        """
        Return the list of the node with topological sorting
        """
        return nx.topological_sort(self.G)

    def retriveOriginalGraph(self):
        """
        To resume the original graph which is constructed from the ChaiDNN output graph
        """
        self.G.clear()
        self.G.add_edges_from(self.original_edges)
        self.G.add_nodes_from(self.original_nodes)
        for n in self.G.nodes:
            n.lat_one_row = None

    def splitGroupNode(self):
        """
        When the node contains group, we want to split the original node into a subgraph, 
        with one join node added at the top, one join node added at the bottom, 
        and the original node is duplicated into group number of nodes, connecting to top join
        and bottom join: bot-->[n1, n2, ..., ng] --> top

        """
        group_nodes = []
        for n in self.G.nodes:
            if n.type == "Convolution":
                group = int(n.params[4].split("=")[1])
                if group > 1:
                    group_nodes.append(n)
        for n in group_nodes:
            group = int(n.params[4].split("=")[1])
            n.input_params[1] /= group
            n.output_params[1] /= group
            #FIXME, later need to also update the parameters
            bot = joinNode()
            top = joinNode()
            self.add_node(bot)
            self.add_node(top)
            for pred in list(self.G.predecessors(n)):
                self.G.remove_edge(pred, n)
                self.G.add_edge(pred, bot)
            for succ in list(self.G.successors(n)):
                self.G.remove_edge(n, succ)
                self.G.add_edge(top, succ)
            self.G.add_edge(bot, n)
            self.G.add_edge(n, top)

            for i in range(1, group):
                n_new = deepcopy(n)
                n_new.name = n_new.name + "_"+str(i)
                self.G.add_node(n_new)
                self.G.add_edge(bot, n_new)
                self.G.add_edge(n_new, top)
            n.name = n.name + "_0"
