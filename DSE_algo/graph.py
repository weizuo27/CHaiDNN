import matplotlib.pyplot as plt
import networkx as nx
import cvxpy as cvx

class layer:
    """
    The class to discribe attributes relate to layers

    self.name: The name of the layer
    self.type: The type of the layer
    self.params: The parameter list of the layers, according to different type, the list
    has different interpretation.
    self.input_params: The input dimension of the layer [batch, channel, height, width]
    self.output_params: The output dimension of the layer[batch, channel, height, width]
    """
    def __init__(self, line):
        """
        Constructor
        Args: 
            line: the str that contains information of each layer
        """
        n_t = line.split(":")[0]
        self.name, self.type = n_t.split("-")

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

class graph:
    """
    The class that contains the graph
    """
    def __init__(self, filename):
        self.G = nx.DiGraph()
        self.mapping = dict()
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

        #Build Edge
        for bb in bottom_table:
            if bb in top_table:
                for bbb in bottom_table[bb]:
                    for ttt in top_table[bb]:
                        self.G.add_edge(bbb, ttt)

def drawGraph(G):
    nx.draw(G, with_labels=True, font_weight='bold')
    plt.show()

g = graph("./log")
h = nx.relabel_nodes(g.G, g.mapping)
drawGraph(h)

