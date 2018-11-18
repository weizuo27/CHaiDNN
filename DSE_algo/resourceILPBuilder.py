import matplotlib.pyplot as plt
import networkx as nx
import cvxpy as cvx
from utils import *


class resourceILPBuilder():
    """
    Attributes:
        BRAM_budget: The budget of BRAM given
        DSP_budget: The budget of DSP
        LUT_budget: The budget of LUT
        FF_budget: The budget of FF
        BW_budget: The budget of Bandwidth
        mappingVariables: Dictionary. Key: layer type. Values: 2-dim array B_ij: i-th layer mapps to j-th IP
        constraints: The list of constraints that should satisfy
        violation_constraints, the list of constraints that only represent the path violating a latency
        resourceVariables: Dictionary. key: layer type. Value: 1-dim array R_j: j-th IP is used 
    """

    def __init__(self, BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget):
        #The set of resource budgets
        self.BRAM_budget = BRAM_budget
        self.DSP_budget = DSP_budget
        self.LUT_budget = LUT_budget
        self.FF_budget = FF_budget
        self.BW_budget = BW_budget

        #the set of variables
        self.mappingVariables = dict() 
        self.constraints = []
        self.violation_constraints = []
        self.resourceVariables = dict()

    def createVs(self, IP_table, IP_table_per_layer, layerQueue, hw_supported_layers):
        """
        create all variables of the problem.
        Args:
            layerQueue: The dicionary. Key: the layer type. Value: The list of layers in the NN, that is that type
            IP_table: The dictionary. Key: The layer type. Value: The candidate IPs fall into that category
            IP_table_per_layer: The dictionary to record the IP can be used per layer.
                Key: The layer. Value: list of length IP_table[layer.type]. Each element is either 0 or 1.
                    0 means that IP is not used for this layer, 1 otherwise.
        """
        #1. create the mapping B_ij: i-th layer maps to j-th IP
        for layer_type in layerQueue:
            if layer_type in hw_supported_layers:
                queue = layerQueue[layer_type] #The queue of the layer_type
                IP_queue = IP_table[layer_type]
                self.mappingVariables[layer_type] = dict()
                mapping_vars = []

                for i in queue: #Traverse the queue to add the varible
                    IPs = IP_table_per_layer[i] #The IPs available for the layer type
                    row = []
                    for j in range(len(IPs)):
                        if IPs[j] == 1:
                            row.append(cvx.Variable(boolean=True, name=i.name + "_" + IP_queue[j].name))
                        else:
                            row.append(0.0)
                    mapping_vars.append(row)
                self.mappingVariables[layer_type] = mapping_vars

        #2. create resource varible R_j: j-th IP is used in the implementation
        for layer_type in IP_table:
            self.resourceVariables[layer_type]=[]
            IP_queue = IP_table[layer_type]
            for j in range(len(IP_queue)):
                self.resourceVariables[layer_type].append(cvx.Variable(boolean=True, name=IP_queue[j].name))

    def createConstraints(self, IP_table, layerQueue):
        """
        Create constraints:
        Args:
            IP_table: The dictionary. Key: The layer type. Value: The candidate IPs fall into that category
            layerQueue: The dicionary. Key: the layer type. Value: The list of layers in the NN, that is that type
        """
        #1. one layer is mapped to one IP
        for layer_type in self.mappingVariables:
            queue = self.mappingVariables[layer_type]
            for row in queue:
                self.constraints.append(sum(row)==1)

        #2. resource constraints, resourceVariable[j]: how many times
        for layer_type in self.mappingVariables:
            IP_queue= IP_table[layer_type]
            queue = layerQueue[layer_type]
            for j in range(len(IP_queue)):
                exp = self.mappingVariables[layer_type][0][j]
                self.constraints.append(self.resourceVariables[layer_type][j]>= self.mappingVariables[layer_type][0][j])
                for i in range(1, len(queue)):
                    exp += self.mappingVariables[layer_type][i][j] 
                    #print self.mappingVariables[layer_type][i][j]
                    self.constraints.append(self.resourceVariables[layer_type][j]>= self.mappingVariables[layer_type][i][j])

                #FIXME: Currently only has reousrceVariable[layer_type][j] <= sum(mappVariable[layer_type][i][j];
                #but the correct formulation is resourceVariables[layer_type][j] = 0 if sum(mappVariable[layer_type][i][j] == 0 else 1
                #since it is to indicate whether that is used. 
                #How to formulate it in a better way?

                #The reason I can formulate it in this way is because in the problem solving, 
                #The objective function is maximize the sum of resource variables
                self.constraints.append(self.resourceVariables[layer_type][j]<=exp) 

        #3. The sum of all resource variables should not exceed the resource budget
        exp_BRAM, exp_DSP, exp_FF, exp_LUT, exp_BW = 0,0,0,0,0
        for layer_type in IP_table:
            IP_queue = IP_table[layer_type]
            for j in range(len(IP_queue)):
                exp_BRAM += IP_queue[j].BRAM * self.resourceVariables[layer_type][j] 
                exp_DSP += IP_queue[j].DSP* self.resourceVariables[layer_type][j]
                exp_FF += IP_queue[j].FF* self.resourceVariables[layer_type][j]
                exp_LUT += IP_queue[j].LUT* self.resourceVariables[layer_type][j]
                exp_BW += IP_queue[j].BW * self.resourceVariables[layer_type][j]

        #print exp_BRAM

        self.constraints.append(exp_BRAM <= self.BRAM_budget)
        self.constraints.append(exp_DSP <= self.DSP_budget)
        self.constraints.append(exp_FF <= self.FF_budget)
        self.constraints.append(exp_LUT <= self.LUT_budget)
        self.constraints.append(exp_BW <= self.BW_budget)

#    def addViolationPaths(self, violation_path, layerQueue, IP_table):
#        """
#        Violation means that the current mapping cannot be held for all 
#        nodes in the path at the same time. So the constraints should be
#        the sum of the mapping of the violation path should be less the the 
#        length of the path
#
#        Also, all the IPs share the original name should be included
#        """
#        exp = 0
#        for node in violation_path:
#            i_idx = layerQueue[node.type].index(node)
#            for j_idx, ip in enumerate(IP_table[node.type]):
#                if ip.orig_name == node.mappedIP.orig_name:
#                    exp+= self.mappingVariables[node.type][i_idx][j_idx]
#        self.violation_constraints.append(exp <= len(violation_path)-1)

    def addViolationPaths(self, violation_path, layerQueue, IP_table):
        violate_layers = dict()
        all_violate_IPs = dict()
        
        for l in violation_path:
            if l.mappedIP not in violate_layers:
                violate_layers[l.mappedIP] = [l]
                all_violate_IPs[l.mappedIP] = []
            else:
                violate_layers[l.mappedIP].append(l)

        #for each violated IP, collect the IPs that are smaller than this IP
        for vio_ip in all_violate_IPs:
            for idx, ip in enumerate(IP_table[vio_ip.type]):
                if all(ip.paramList[i] <= vio_ip.paramList[i] for i in range(len(ip.paramList))):
                    all_violate_IPs[vio_ip].append((idx,ip))
        print all_violate_IPs
        self.recAddViolationPath(0, all_violate_IPs.keys(), len(violation_path), all_violate_IPs, violate_layers, [], layerQueue)


    def recAddViolationPath(self, idx, keys_list, violation_path_length, all_violate_IPs, violate_layers, exp_list, layerQueue):
        if idx == len(keys_list):
            if sum(exp_list) != 0:
                self.violation_constraints.append(sum(exp_list) <= violation_path_length-1)
            print "aaa", exp_list, sum(exp_list)

        else:
            for j_idx, ip in all_violate_IPs[keys_list[idx]]:
                for l in violate_layers[keys_list[idx]]:
                    i_idx = layerQueue[l.type].index(l)
                    exp_list.append(self.mappingVariables[l.type][i_idx][j_idx])
                self.recAddViolationPath(idx+1, keys_list, violation_path_length, all_violate_IPs, violate_layers, exp_list, layerQueue)
                for l in violate_layers[keys_list[idx]]:
                    exp_list.pop()

    def createProblem(self):
        """
        Formulate the problem, only check the feasibility
        The objective is maximize the sum of resource variables.  
        The reason is that, only by setting the objective to be maximum, 
        the resource vairables constraints is complete.
        However, this discourages the resource reuse, since the solution will
        always maximize the resource maximization variables. 
        #FIXME: Is there are better way to formulate this problem?
        """
        obj_vars = []
        for layer_type in self.resourceVariables:
            obj_vars += self.resourceVariables[layer_type]
        #print "obj_vars ", obj_vars
        self.obj = cvx.Maximize(sum(obj_vars)) #Only check the feasibility
        self.prob = cvx.Problem(self.obj, self.constraints+self.violation_constraints)

    def solveProblem(self):
        """
            return: True if the optimal solution can be found. False otherwise
        """
        paramList = {"MIPFocus":1}
        self.prob.solve(solver = "GUROBI", kwargs =paramList)
        return self.prob.status == "optimal"
    def printSolution(self, level = 1):
        """
            Level of the detail level of the solution
        """
        print("status:", self.prob.status)
        print("optimal value", self.prob.value)
        if(level > 1):
            for layer_type in self.mappingVariables:
                print layer_type, ":"
                for i, row in enumerate(self.mappingVariables[layer_type]):
                    for j, elem in enumerate(row):
                        print "mapping", i, j, elem.name(), elem.value
                    print "\n"
                for j, res in enumerate(self.resourceVariables[layer_type]):
                    print "resource", j, res.value


#rb = resourceILPBuilder(BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget)
#rb.createVs(IP_table, conv_queue)
#rb.createConstraints()
#rb.createProblem()
#rb.solveProblem()
