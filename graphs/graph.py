from edge import Edge

class Graph:
    def __init__(self, filepath):
        file = open(filepath)
        self.nodesTotal = int(file.readline())
        
        self.indexOfNodes = [0] * (self.nodesTotal+2)
        self.edgesTotal = 0
        self.edgeList = None
        self.edgeLen = None

        # All edges of graph
        self.graph_edge = []

        # Edges for each node
        self.edges = {i:[] for i in range(self.nodesTotal+1)}


        edge_line = file.readline()
        while edge_line:
            self.edgesTotal +=1

            new_edge = Edge(*map(int,edge_line.split()))

            self.edges[new_edge.src].append(new_edge)
            self.graph_edge.append(new_edge)

            edge_line = file.readline()
        file.close()


        self.edgeLen = [0] * self.edgesTotal
        self.edgeList = [0] * self.edgesTotal

        # Sort the edges
        edge_no = 0
        for vertex in self.edges.keys():
            edgeOfVertex = self.edges[vertex]
            edgeOfVertex.sort(key = lambda edge: edge.dest)

            
            # Prefix sum computation
            self.indexOfNodes[vertex] = edge_no

            for j in range(len(edgeOfVertex)):

                self.edgeList[edge_no] = edgeOfVertex[j].dest
                self.edgeLen[edge_no] = edgeOfVertex[j].weight
                
                edge_no +=1

        self.indexOfNodes[self.nodesTotal+1] = edge_no


    def add_edge(self, src, dest, weight = 0):
        new_edge = Edge(src, dest, weight)

        self.edges[new_edge.src].append(new_edge)
        self.graph_edge.append(new_edge)


        startIndex = self.indexOfNodes[src]
        endIndex = self.indexOfNodes[src+1]
        nbrsCount = endIndex - startIndex
        insertAt = 0

        # Find position to insert - maintain sortedness
        if (self.edgeList[startIndex] >= dest) or (nbrsCount == 0):
            insertAt = startIndex

        elif self.edgeList[endIndex-1] <= dest:
            insertAt = endIndex

        else:
            for i in range(startIndex, endIndex-1):
                if self.edgeList[i] <= dest and self.edgeList[i+1] >= dest:
                    insertAt = i+1
                    break
        self.edgeList.insert(insertAt, dest)
        self.edgeLen.insert(insertAt, weight)

        for i in range(src+1, self.nodesTotal+2):
            self.indexOfNodes[i] +=1

    
        self.edgesTotal +=1
        self.indexOfNodes[self.nodesTotal + 1] += 1



class DirGraph(Graph):
    pass


class UndirGraph(Graph):
    def add_edge(self, src, dest, weight=0):
        super().add_edge(src, dest, weight)

        # Append reverse edge
        reverse_edge = Edge(dest, src, weight)
        self.edges.append(reverse_edge)