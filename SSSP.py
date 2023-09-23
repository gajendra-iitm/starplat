from math import inf

def Compute_SSSP(g, src_node):

    def isFinished(modified):
        for node in modified.keys():
            if modified[node]:
                return False
        return True
       

    g.attachNodeProperty(distance=inf, modified=False, modified_next=False)
    g.modified[src_node] = True
    g.distance[src_node] = 0

    finished = False

    while not finished:

        for v in filter(lambda node: g.modified[node] == True, g.nodes()):

            # Changed loop to accessing via nodes
            for nbr in g.getOutNeighbors(v):

                e = g.get_edge(v, nbr)

                new_distance = g.distance[v] + e.weight
                if new_distance < g.distance[nbr]:
                    g.distance[nbr] = new_distance

                    g.modified_next[nbr] = True
                
                # g.distance[nbr] = min(g.distance[nbr], g.distance[v] + e.weight)
                # g.modified_next[nbr] = True

        
        # Making a deep copy
        g.modified = g.modified_next.copy()

        g.attachNodeProperty(modified_next=False)

        finished = isFinished(g.modified)

    return g.distance