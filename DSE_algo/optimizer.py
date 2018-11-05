from resourceILPBuilder import resourceILPBuilder
from graph import graph
from graph import pipeNode
from utils import *
import itertools

class optimizer:
    def __init__(self, BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget, latency_Budget,
            app_fileName, IP_fileName):
        """
        The top function of the optimizer
        Args:
            BRAM_budget: The budget of BRAM given
            DSP_budget: The budget of DSP
            LUT_budget: The budget of LUT
            FF_budget: The budget of FF
            BW_budget: The budget of Bandwidth
            latency_Budget: the latency budget

            hw_layers:. The dictionary of the layers that are supported by hardware
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
        self.rb = resourceILPBuilder(BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget) #Builder resource solver
        #solve the problem
        self.rb.createVs(self.IP_table, self.g.layerQueue, self.hw_layers)
        self.rb.createConstraints(self.IP_table, self.g.layerQueue)

        # Now starting the loop
        #1. Initalize the variables
        self.latency_lb = 0
        self.latency_ub = latency_Budget
        self.new_latency_target = latency_Budget
        self.latency_achieved = None
        #2. Loop body
        firstIter = True
        #while(self.latency_lb < self.latency_ub):
        oneIter = True
        while(oneIter): #For debugging purpose
            self.rb.createProblem()
            optimal = self.rb.solveProblem()
            if(not optimal):
                assert (not firstIter), "The resource budget is too tight, no feasible mapping solution."
                firstIter = False
                self.latency_lb = self.new_latency_target
                #FIXME: The latency should be integer or floating point?
                self.new_latency_target = (self.latency_lb + self.latency_ub)/2 
                continue
            #self.rb.printSolution()
            #assign the mapping result
            self.assignMappingResult()
            #Now update the latency since the IPs are assigned
            #self.g.drawGraph()
            #print(self.g)
            self.g.computeLatency()
            #add nodes to factor in pipeline
            self.addPipelineNodes()
            #self.g.printNodeLatency()
            #fill-in the IPReuseTable:
            self.IPReuseTable = dict()
            self.constructIPReuseTable()
            #add the the edges to factor in the IP reuse
            #self.addReuseEdges()
            #now update the violation path related ILP, resolve the ILP
            status, ret = self.scheduling(self.new_latency_target)
            if(status == "success"):
                self.latency_ub = ret
                self.new_latency_target = (self.latency_ub + self.latency_lb)/2
            else: #Failed
                self.updateResourceILPBuilder(violation_path)
                self.g.retriveOriginalGraph()
            oneIter = False #For debugging purpose
        
    def constructIPReuseTable(self):
        """
        build the table(dictionary)
            key: The IP
            value: The number of layers that mapped to the IP
        """
        nodes_list = self.g.topological_sort()
        for node in nodes_list:
            if node.mappedIP is None or node.mappedIP == "Software":
                 continue
            if node.mappedIP not in self.IPReuseTable:
               self.IPReuseTable[node.mappedIP] = []
            self.IPReuseTable[node.mappedIP].append(node)
        
    def addReuseEdges(self):
        """
        The function to add edges between two nodes if they map to the same IP
        """
        #Iterate through each pair that reuse one IP, then add edge
        #FIXME: Actually this may add redundant edges
        for IP in self.IPReuseTable:
            for (s, t) in itertools.combinations(self.IPReuseTable[IP], 2):
                print s.name, t.name
                self.g.G.add_edge(s,t)

    def addPipelineNodes(self):
        """
        The function to add pipelined node, if two layers can be pipelined.
        The idea is to insert a node with negative latency, so when do scheduling, 
        The pipelined effect can be factored in.
        """
        #cannot directly iterate on original edges, since need to modify the graph
        pipeNode_list = []
        for (s_node, t_node) in self.g.G.edges():
            if s_node.mappedIP != t_node.mappedIP: #Two layers are pipelinable
                #The neg_latency is the difference between the source node finishes the whole layer
                #and when it generates enough data to compute one layer output of the target node 
                #,(which is the pipeline starting point of the target node)
                #print s_node.name, s_node.latency, s_node.pipelinedLatency
                neg_latency = -s_node.latency + t_node.lat_one_row
                if(neg_latency < 0):
                    node = pipeNode(neg_latency)
                    pipeNode_list.append([node, s_node, t_node])

        for node, s_node, t_node in pipeNode_list:
            self.g.G.remove_edge(s_node, t_node)
            self.g.add_node(node)
            self.g.G.add_edge(s_node, node)
            self.g.G.add_edge(node, t_node)

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

    def scheduling(self, latency_Budget):
        """
        ASAP scheduling for the graph. 
        First it applies topological sorting. Then started assigning
        the starting time according to the order. Along the way recording 
        the path.

        If at one node the overall latency exceeds the latency_Budget, then stop.
        Reverse chasing to find the shortest path that violates the constraints

        Args:
            latency_Budget: FP data, the latency budget 
        Return:
            1. status: The string to indicate a legal schedule can be found or not
            2.
            if fail, return the shortest violation path 
            if succeed, return the shortest latency
        """
        startingTime = dict() # The dictionary. Key: node. Value: Starting time stamp
        path = dict() # The dictionary, to record critical path to the node. Key: node. Value: list of nodes to represent the path
        noteList = self.g.topological_sort()
        for n in noteList:
            max_starting = 0
            max_pred = None
            preds = list(self.g.G.predecessors(n))
            if len(preds) == 0:
                startingTime[n] = 0.0
                path[n] = [n]
            else:
                for pred in preds:
                    if max_starting < startingTime[pred] + pred.pipelinedLatency:
                        max_starting = startingTime[pred] + pred.pipelinedLatency
                        max_pred = pred
                startingTime[n] = max_starting
                path[n] = path[max_pred] + [n]
            #if at one node the overall latency exceeds the budget:
            #need to reverse the path to get the shortest path that 
            #violates the constraints
            # TODO: Currently just using linear, later can use binary search
            endtime = startingTime[n] + n.latency
            if startingTime[n] + n.latency > latency_Budget:
                violation_path = [n]
                if n.latency > latency_Budget:
                    return "failed", violation_path
                for m in path[n][::-1]:
                    if endtime - startingTime[m] > latency_Budget:
                        violation_path += [m]
                        return "failed", violation_path

        #if succeed, return the optimal latency
        return "success", startingTime[n] + n.latency

    def updateResourceILPBuilder(self, violation_path):
        """
        After the scheduling, if there is violation_path, 
        we need to update the constarints and resolve the ILP
        Args:
            violation_path: The list, contains the list of nodes that compose the violation
        """
        self.rb.addViolationPaths(violation_path, self.g.layerQueue, self.IP_table)
