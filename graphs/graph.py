from .edge import Edge
# from .node import Node

class Graph:
    def __init__(self, file):
        ### PRIVATE VARS ###
        self.filepath = file
        self.nodesTotal = 0 
        self.edgesTotal = 0
        self.edgeLen = []
        
        # Node-wise edges
        self.edges = {}
        ### PRIVATE VARS ###
        
        
        self.edgeList = []
        self.indexOfNodes = []
        self.rev_indexOfNodes = []
        # All edges of graph
        self.graph_edge = []
        
        
    def getEdges(self):
        return self.edges
    
    def getEdgeLen(self):
        return self.edgeLen
    
    def num_nodes(self):
        return self.nodesTotal + 1
    

    def parseEdges(self):
        file = open(self.filepath)

        self.nodesTotal = int(file.readline())
        self.edges = {i:[] for i in range(self.nodesTotal+1)}


        # Parsing edges
        edge_line = file.readline()
        while edge_line:
            self.edgesTotal +=1
            
            source, destination, weightVal = list(map(int,edge_line.split()))
            
            if source > self.nodesTotal:
                self.nodesTotal = source
                
            if destination > self.nodesTotal:
                self.nodesTotal = destination

            e = Edge(source, destination, weightVal)

            self.edges[source].append(e)
            self.graph_edge.append(e)

            edge_line = file.readline()
            
        file.close()
    
    

    def parseGraph(self):
        self.parseEdges()

        self.indexOfNodes = [0] * (self.nodesTotal+2)
        self.rev_indexOfNodes = [0] * (self.nodesTotal+2)
        
        self.edgeLen = [0] * self.edgesTotal
        self.edgeList = [0] * self.edgesTotal
        self.srcList = [0] * self.edgesTotal

        edge_no = 0
        
        # Sort the edges of each node
        for i in range(self.nodesTotal+1):
            edgeOfVertex = self.edges[i]
            edgeOfVertex.sort(key = lambda edge: edge.dest)


        # Prefix sum computation for out neighbours.
        # Loads indexOfNodes and EdgeList
        for i in range(self.nodesTotal+1):

            self.indexOfNodes[i] = edge_no

            for j in range(len(edgeOfVertex)):

                self.edgeList[edge_no] = edgeOfVertex[j].dest
                self.edgeLen[edge_no] = edgeOfVertex[j].weight
                
                edge_no +=1

        self.indexOfNodes[self.nodesTotal+1] = edge_no
        
        
        # Prefix sum computation for in neighbours.
        # Loads rev_indexOfNodes and srcList
        
        
        
        



    def getOutNeighbors(self, node):
        return [edge.dest for edge in self.edges[node]]
    
    def nodes_to(self, node):
        pass
        
    
    def nodes(self):
        return [i for i in range(self.nodesTotal+1)]
    
    
    def get_edge(self, src, dest):
        for edge in self.edges[src]:
            if edge.dest == dest:
                return edge
            
    
    def attachNodeProperty(self, *args, **kwargs):
        for key,value in kwargs.items():
            setattr(self, key, {i:value for i in range(self.nodesTotal+1)})
    
    
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