def Compute_TC(g):
    triangle_count = 0
    
    print(type(filter(lambda u: u<v, g.neighbors(v))))
    
    for v in g.nodes():
        for u in filter(lambda u: u<v, g.neighbors(v)):
            for w in filter(lambda w: w>v, g.neighbors(v)):
                if g.is_an_edge(u,w):
                    triangle_count += 1
                    
    return triangle_count