def Compute_TC(g):
    triangle_count = 0
    
    for v in g.nodes():
        lesser = list(filter(lambda u: u<v, g.neighbors(v)))
        for u in lesser:
            greater = list(filter(lambda w: w>v, g.neighbors(v)))
            for w in greater:
                if g.is_an_edge(u,w):
                    triangle_count += 1
                    
    return triangle_count