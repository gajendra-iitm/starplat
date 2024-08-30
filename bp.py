def bp(g, k, weight):
    # Initialize node properties cur_prob and prior_prob to 1
    cur_prob = {node: 1 for node in g.nodes()}
    prior_prob = {node: 1 for node in g.nodes()}
    iter = 0

    while iter < k:
        iter += 1
        new_prob = {node: 0 for node in g.nodes()}
        
        # Update new_prob based on neighbors' weights and prior_prob
        for v in g.nodes():
            for nbr, w in g.neighbors(v):
                new_prob[v] += w * prior_prob[nbr]
        
        # Update cur_prob for each node
        for v in g.nodes():
            cur_prob[v] = new_prob[v]
        
        # Copy cur_prob to prior_prob for the next iteration
        for v in g.nodes():
             prior_prob[v] = cur_prob[v]
             