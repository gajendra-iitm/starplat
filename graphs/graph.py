from graphs.edge import Edge

class Graph:
    def __init__(self):
        self.nodes = []
        self.edges = []

    def add_node(self, node):
        self.nodes.append(node)

    def add_nodes(self, *nodes):
        for node in nodes:
            self.nodes.append(node)

    def add_edge(self, src, dest, weight = None):
        if (src not in self.nodes) or (dest not in self.nodes):
            return 'Invalid edge'

        new_edge = Edge(src, dest, weight)
        self.edges.append(new_edge)

    
    # Each edge is a tuple
    def add_edges(self, *edges):
        for edge in edges:
            if (edge[0] not in self.nodes) or (edge[1] not in self.nodes):
                return 'Invalid edge'

            new_edge = Edge(*edge)
            self.edges.append(new_edge)


class DirGraph(Graph):
    # Nothing to extend for now
    pass


class UndirGraph(Graph):
    def add_edge(self, src, dest, weight=None):
        super().add_edge(src, dest, weight)

        # Append reverse edge
        reverse_edge = Edge(dest, src, weight)
        self.edges.append(reverse_edge)

    # Each edge is a tuple
    def add_edges(self, *edges):
        super().add_edges(edges)

        # Append reverse edges
        for edge in edges:
            reverse_edge = Edge(*edge)
            self.edges.append(reverse_edge)
