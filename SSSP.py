from math import inf
from constructs import FixedPoint

def Compute_SSSP(g, src_node):
    g.attachNodeProperty(distance=inf, modified=False, modified_next=False)
    g.modified[src_node] = True
    g.distance[src_node] = 0

    finished = False

    FixedPoint('''
for v in filter(lambda node: g.modified[node] == True, g.nodes()):

    # Changed loop to accessing via nodes
    for nbr in g.getOutNeighbors(v):

        e = g.get_edge(v, nbr)

        new_distance = g.distance[v] + e.weight
        if new_distance < g.distance[nbr]:
               
            g.distance[nbr] = new_distance
            g.modified_next[nbr] = True

# Making a deep copy
g.modified = g.modified_next.copy()

g.attachNodeProperty(modified_next=False)
        ''', g, finished)

    return g.distance