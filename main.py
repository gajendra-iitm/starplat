from graphs.graph import UndirGraph, DirGraph

ug = UndirGraph()
ug.add_nodes(1,2,3,4)

# Edge is passed as tuple of src, dest, weight
ug.add_edge(1, 2)
print(ug.nodes)
print(ug.edges)

dg = DirGraph()
dg.add_nodes(5,6,7,8)

dg.add_edges((7,8,5),(6,8,2))
print(dg.nodes)
print(dg.edges)
