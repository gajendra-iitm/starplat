from graphs.graph import *
from SSSP import Compute_SSSP

# input in input.txt
# first line is max_node (0 to max_node inclusive)
# following lines are edges: src dest weight

filepath = r'/home/viswesh/Desktop/StarPlat/Py-Starplat/graphs/input.txt'
my_graph = DirGraph()
my_graph.parseGraph(filepath)
src_node = 0

result = Compute_SSSP(my_graph, src_node)

print(result)