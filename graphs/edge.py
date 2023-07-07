class Edge:
    def __init__(self, src, dest, weight=None):
        self.src = src
        self.dest = dest
        self.weight = weight

    
    def __repr__(self):
        return '<src: {}, dest: {}, weight: {}>'.format(self.src, self.dest, self.weight)

