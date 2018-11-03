from resourceILPBuilder import resourceILPBuilder
from graph import graph
from utils import *

#FIXME: I am not sure where to insert this yet. But if the scheduling found a conflict pair, then all the identical IPs of one of the pair, should also be eliminated
class optimizer:
    def __init__(self, BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget, 
            app_fileName, IP_fileName):
        """
        The top function of the optimizer
        Args:
            BRAM_budget: The budget of BRAM given
            DSP_budget: The budget of DSP
            LUT_budget: The budget of LUT
            FF_budget: The budget of FF
            BW_budget: The budget of Bandwidth

            hw_layers: The dictionary of the layers that are supported by hardware
            app_fileName: The graph description file from CHaiDNN
            IP_fileName: the file contains the IP candidates and charicterization data
            IP_table: The dictionary that stores the list of candidate IPs
                key: layer_type; value: The list of IPs of that type
            IPReuseTable: The dictionary to describe that for each IP, which layer(s) are using it
                key: IP; Value: The list of layers that use it, in the topological sorted order
        """
        #Hard code the hardware supported layers
        self.hw_layers = {
            "Convolution": 1,
            "Pooling" : 1
        }
        self.g = graph(app_fileName) #generate the graph from CHaiDNN output
        IPs = generateIPs(IP_fileName)
        self.IP_table = constructIPTable(IPs, BRAM_budget, DSP_budget, LUT_budget, FF_budget, BW_budget)
        self.IPReuseTable = dict()
        self.rb = resourceILPBuilder(BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget) #Builder resource solver
        #solve the problem
        self.rb.createVs(self.IP_table, self.g.layerQueue, self.hw_layers)
        self.rb.createConstraints(self.IP_table, self.g.layerQueue)
        self.rb.createProblem()
        self.rb.solveProblem()
        #self.rb.printSolution()
        #assign the mapping result
        self.assignMappingResult()
        #Now update the latency since the IPs are assigned
        print(self.g)
        self.g.computeLatency()
        self.g.printNodeLatency()
        #add nodes to factor in pipeline
        #FIXME: 
        self.addPipelineNodes()
        self.g.drawGraph()
        """ 
        #fill-in the IPReuseTable:
        nodes_list = self.g.topological_sort()
        self.constructIPReuseTable(nodes_list)
        #add the the edges to factor in the IP reuse
        self.addReuseEdges()
        """
        
    def constructIPReuseTable(self, node_list):
        """
        build the table(dictionary)
            key: The IP
            value: The number of layers that mapped to the IP
        Arg:
            node_list: The list of all nodes in the graph in topological sorting order
        """
        for node in node_list:
            if node.mappedIP not in self.IPReuseTable:
                self.IPReuseTable[node.mappedIP] = []
            self.IPReuseTable[node.mappedIP].append(node)
        
    def addReuseEdges(self):
        """
        The function to add edges between two nodes if they map to the same IP
        """
        #Iterate through each pair that reuse one IP, then add edge
        for IP in self.IPReuseTable:
            for (s, t) in itertools.combinations(IPReuseTable[IP], 2):
                self.g.add_edge(s,t)

    def addPipelineNodes(self):
        """
        The function to add pipelined node, if two layers can be pipelined.
        The idea is to insert a node with negative latency, so when do scheduling, 
        The pipelined effect can be factored in.
        """
        #cannot directly iterate on original edges, since need to modify the graph
        edge_list = self.g.G.edges() 
        for (s_node, t_node) in edge_list:
            if s_node.mappedIP != t_node.mappedIP: #Two layers are pipelinable
                #The neg_latency is the difference between the source node finishes the whole layer
                # and when it generates one layer output (which is the input of next layer)
                #print s_node.name, s_node.latency, s_node.pipelinedLatency
                neg_latency = -s_node.latency + s_node.pipelinedLatency
                if(neg_latency < 0):
                    self.g.remove_edge(s_node, t_node)
                    node = pipeNode(neg_latency)
                    self.g.add_node(node)
                    self.g.add_edge(s_node, node)
                    self.g.add_edge(node, t_node)

    def assignMappingResult(self):
        """
        After resource mapping result come out, annotate them to the graph
        """
        for layer_type in self.rb.mappingVariables:
            variables = self.rb.mappingVariables[layer_type]
            for layer_idx in range(len(variables)):
                node = self.g.layerQueue[layer_type][layer_idx]
                for ip_idx in range(len(variables[layer_idx])):
                    #print layer_type, layer_idx, ip_idx, variables[layer_idx][ip_idx].value  #If the mapping result is True, then we set the mapping
                    if (variables[layer_idx][ip_idx].value > 0.5 ): #If the mapping result is True, then we set the mapping
                        node.set_IP(self.IP_table[layer_type][ip_idx]) 
