from graphs.graph import *
from SSSP import Compute_SSSP

# input in input.txt

filepath = ''
my_graph = DirGraph()
my_graph.parseGraph(filepath)
src_node = 0

result = Compute_SSSP(my_graph, src_node)

print(result)