import matplotlib.pyplot as plt
import networkx as nx
import cvxpy as cvx

class layer():
    def __init__(self, type):
        self.type = type

#The class of IP
class IP():
    def __init__(self, type, resource_list, latency):
        self.type = type
        self.BRAM, self.DSP, self.FF, self.LUT, self.BW= resource_list ;

G = nx.Graph()

n1 = layer("conv")
G.add_node(n1)
n2 = layer("conv")
G.add_node(n2)
n3 = layer("conv")
G.add_node(n3)

G.add_edge(n1, n2)
G.add_edge(n2, n3)

conv_queue = [n1, n2, n3]

IP_table = dict()


#BRAM, DSP, FF, LUT Budgets

BRAM_budget = 20
DSP_budget = 30
FF_budget = 30
LUT_budget = 30
BW_budget = 100

ip1 = IP("conv",[10,10, 10, 10, 10], 20)
ip2 = IP("conv", [15, 15, 15, 15, 15], 15)
ip3 = IP("conv", [20, 20, 20, 20, 15], 10)

n_ip1 = min(BRAM_budget / ip1.BRAM, DSP_budget / ip1.DSP, FF_budget/ip1.FF, LUT_budget/ip1.LUT)
n_ip2 = min(BRAM_budget / ip2.BRAM, DSP_budget / ip2.DSP, FF_budget/ip2.FF, LUT_budget/ip2.LUT)
n_ip3 = min(BRAM_budget / ip3.BRAM, DSP_budget / ip3.DSP, FF_budget/ip3.FF, LUT_budget/ip3.LUT)

IP_table["conv"] = [ip1] * n_ip1 + [ip2] * n_ip2 + [ip3] * n_ip3



class resourceILPBuilder():
    def __init__(self, BRAM_budget, DSP_budget, FF_budget, LUT_budget, BW_budget):
        #The set of resource budgets
        self.BRAM_budget = BRAM_budget
        self.DSP_budget = DSP_budget
        self.LUT_budget = LUT_budget
        self.FF_budget = FF_budget
	self.BW_budget = BW_budget

        #the set of variables
        self.mappingVariables = []
        self.constraints = []
        self.resourceVariables = []

    def createVs(self, IP_table, queue):
        #1. create the mapping B_ij: i-th layer maps to j-th IP
        for i in queue:
            row = []
            for j in IP_table[i.type]:
                row.append(cvx.Variable(boolean = True))
            self.mappingVariables.append(row)
        #2. create resource varible R_j: j-th IP is used in the implementation
        for j in range(len(IP_table["conv"])):
            self.resourceVariables.append(cvx.Variable(boolean = True))


    def createConstraints(self):
        #1. one layer is mapped to one IP
        for row in self.mappingVariables:
            self.constraints.append(sum(row)==1)
        #2. resource constraints, resourceVariable[j]: how many times
        for j in range(len(IP_table["conv"])):
            exp = self.mappingVariables[0][j]
            for i in range(1, len(queue)):
                exp += self.mappingVariables[i][j]
            #print exp, self.resourceVariables
            self.constraints.append(self.resourceVariables[j]<=exp)

        #3. The sum of all resource variables should not exceed the resource budget
        exp_BRAM, exp_DSP, exp_FF, exp_LUT, exp_BW = 0,0,0,0, 0
        for j in range(len(IP_table["conv"])):
            print j, IP_table["conv"][j].BRAM
            exp_BRAM += IP_table["conv"][j].BRAM * self.resourceVariables[j] 
            exp_DSP += IP_table["conv"][j].DSP* self.resourceVariables[j]
            exp_FF += IP_table["conv"][j].FF* self.resourceVariables[j]
            exp_LUT += IP_table["conv"][j].LUT* self.resourceVariables[j]
	    exp_BW += IP_table["conv"][j].BW * self.resourceVariables[j]
        self.constraints.append(exp_BRAM <= self.BRAM_budget)
        self.constraints.append(exp_DSP <= self.DSP_budget)
        self.constraints.append(exp_FF <= self.FF_budget)
        self.constraints.append(exp_LUT <= self.LUT_budget)
	self.constraints.append(exp_BW <= self.BW_budget)
    def createProblem(self):
        print "len resourceVariables", len(self.resourceVariables)
        self.obj = cvx.Maximize(sum(self.resourceVariables)) #Only check the feasibility
        print self.obj
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


#plt.subplot(121)
def drawGraph(G):
    nx.draw(G, with_labels=True, font_weight='bold')
    plt.show()

