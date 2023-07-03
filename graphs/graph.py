class Graph:
    def __init__(self):
        self.nodes = []
        self.edges = []

    def add_node(self, node):
        self.nodes.append(node)

    def add_nodes(self, *args):
        for node in args:
            self.nodes.append(node)


class DirGraph(Graph):
    def add_edge(self, start_node, end_node):
        self.edges.append((start_node, end_node))


class UndirGraph(Graph):
    def add_edge(self, start_node, end_node):
        self.edges.append((start_node, end_node))
        self.edges.append((end_node, start_node))


ug = UndirGraph()
ug.add_nodes(1,2,3,4)
ug.add_edge(1, 2)
print(ug.nodes)
print(ug.edges)
