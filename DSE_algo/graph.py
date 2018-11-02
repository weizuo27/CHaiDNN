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
        input_params: The input dimension of the layer [batch, channel, height, width]
        output_params: The output dimension of the layer[batch, channel, height, width]
        mappedIP: The mapped ip of this layer
    Methods:
        set_input_params: Set input dimentions
        set_output_params: set output related parameters, e.g., dimensions
        set_IP: set the mapped IP for this Layer
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

    def set_input_params(self):
        """
        The function sets the input parameters
        """
        self.input_params = map(int,line.split("x")) #[batch, channel, height, width]
    def set_output_params(self):
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

    def compute_Latency(self, n_row, IP):
        """
        Compute the latency of this layer using one IP.
        Args:
            n_rows: the number of rows to compute
            IP: the specific IP to compute this layer
        """
        None
        #return IP.latency()
        

    def get_pipeline_Latency(self, num_rows):
        """
        Compute the time that is needed to compute num_rows of rows.
        This is used as starting time for the next layer
        """
        None
    def generateOneOutrow(self):
        """
        The number of rows need to complete to generate one output row
        #FIXME: Maybe can directly return the latency once it is 
        """


class graph:
    """
    The class that contains the graph
    Atrributes:
            G: The graph
            mapping: A dictionary between the nodes in the graph and a name. This is for drawing the pictures.
            layerQueue: The dictionary. key: The layer type. Value: The list of layers that fall into that category
    Methods:
            construct: to construct the graph from a file that is dumped from CHaiDnn

    """
    def __init__(self, filename):
        self.G = nx.DiGraph()
        self.mapping = dict()
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

            if l.find("--Blobs--") >= 0 :
                for l in f:
                    if l == "":
                        break
                    blobname, dims =l.split(":")
                    if blobname in bottom_table:
                        bottom_table[blobname].set_input_params(dims)
                    if blobname in top_table:
                        top_table[blobname].set_output_params(dims)
        f.close()

        #Build Edges
        for bb in bottom_table:
            if bb in top_table:
                for bbb in bottom_table[bb]:
                    for ttt in top_table[bb]:
                        self.G.add_edge(bbb, ttt)
        
        def drawGraph(self):
            h = nx.relabel_nodes(self.G, g.mapping)
            nx.draw(h, with_labels=True, font_weight = 'bold')
