def v_cover(g, vc):
    # Define the visited property
    propNode = type('propNode', (object,), {'visited': False})
    
    # Attach the 'visited' property to all nodes in the graph
    g.attachNodeProperty(visited=propNode.visited)
    
    # Iterate over all nodes and their neighbors to find the vertex cover
    for v in filter(lambda node: not g.visited[node], g.nodes()):
        # For each unvisited node, check its neighbors
        for nbr in g.neighbors(v):
            # If the neighbor is also unvisited, mark both the node and the neighbor as visited
            if not g.visited[nbr]:
                g.visited[nbr] = True
                g.visited[v] = True
                # Add both the node and the neighbor to the vertex cover
                vc[v] = True
                vc[nbr] = True
    
    return vc