import matplotlib.pyplot as plt
import networkx as nx
import cvxpy as cvx


class resourceILPBuilder():
    """
    Attributes:
        BRAM_budget: The budget of BRAM given
        DSP_budget: The budget of DSP
        LUT_budget: The budget of LUT
        FF_budget: The budget of FF
        BW_budget: The budget of Bandwidth
        mappingVariables: Dictionary. Key: layer type. Values: 2-dim array B_ij: i-th layer mapps to j-th IP
        constraints: FIXME
        resourceVariables: Dictionary. key: layer type. Valus: 1-dim array R_j: j-th IP is used 
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
        self.resourceVariables = []

    def createVs(self, IP_table, layerQueue):
        """
        create all variables of the problem.
        Args:
            layerQueue: The dicionary. Key: the layer type. Value: The list of layers in the NN, that is that type
            IP_table: The dictionary. Key: The layer type. Value: The candidate IPs fall into that category

        """
        #1. create the mapping B_ij: i-th layer maps to j-th IP
        for layer_type in layerQueue:
            queue = layerQueue[layer_type] #The queue of the layer_type
            IPs = IP_table[layer_type] #The IPs available for the layer type

            self.mappingVariables[layer_type] = dict()
            mapping_vars = []

            for i in queue: #Traverse the queue to add the varible
                row = []
                for j in IPs:
                    row.append(cvx.Variable(boolean = True))
                mapping_vars.append(row)
            self.mappingVariables[layer_type] = mapping_vars

        #2. create resource varible R_j: j-th IP is used in the implementation
        for layer_type in IP_table:
            self.resourceVariables[layer_type]=[]
            IP_queue = IP_table[layer_type]
            for j in range(len(IP_queue)):
                self.resourceVariables[layer_type].append(cvx.Variable(boolean = True))

    def createConstraints(self, IP_table, layerQueue):
        """
        Create constraints:
        Args:
            IP_table: The dictionary. Key: The layer type. Value: The candidate IPs fall into that category
        """
        #1. one layer is mapped to one IP
        for layer_type in self.mappingVariables:
            queue = self.mappingVariables[layer_type]
            for row in self.mappingVariables:
                self.constraints.append(sum(row)==1)

        #2. resource constraints, resourceVariable[j]: how many times
        for layer_type in self.mappingVariables:
            IP_queue= IP_table[layer_type]
            queue = layerQueue[layer_type]
            for j in range(len(IP_queue)):
                exp = self.mappingVariables[layer_type][0][j]
                for i in range(1, len(queue)):
                    exp += self.mappingVariables[i][j] 

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
                exp_BRAM += IP_queue[j].BRAM * self.resourceVariables[j] 
                exp_DSP += IP_queue[j].DSP* self.resourceVariables[j]
                exp_FF += IP_queue[j].FF* self.resourceVariables[j]
                exp_LUT += IP_queue[j].LUT* self.resourceVariables[j]
                exp_BW += IP_queue[j].BW * self.resourceVariables[j]

        self.constraints.append(exp_BRAM <= self.BRAM_budget)
        self.constraints.append(exp_DSP <= self.DSP_budget)
        self.constraints.append(exp_FF <= self.FF_budget)
        self.constraints.append(exp_LUT <= self.LUT_budget)
        self.constraints.append(exp_BW <= self.BW_budget)

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
        self.obj = cvx.Maximize(sum(self.resourceVariables)) #Only check the feasibility
        self.prob = cvx.Problem(self.obj, self.constraints)

    def solveProblem(self):
        self.prob.solve(solver = "GUROBI")
        print("status:", self.prob.status)
        print("optimal value", self.prob.value)
        for i, row in enumerate(self.mappingVariables):
            for j, elem in enumerate(row):
                print "mapping", i, j, elem.name(), elem.value
            for j, res in enumerate(self.resourceVariables):
                print "resource", j, res.value

rb = resourceILPBuilder(BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget)
rb.createVs(IP_table, conv_queue)
rb.createConstraints()
rb.createProblem()
rb.solveProblem()
