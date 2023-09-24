from .edge import Edge

class Graph:
    def __init__(self, file):
        ''' PRIVATE VARS '''
        self.__filePath = file
        self.__nodesTotal = 0 
        self.__edgesTotal = 0
        self.__edgeLen = []
        self.__rev_edgeLen = []
        self.__edges = {}               # Node-wise edges
        ''' PRIVATE VARS '''
        
        ''' PUBLIC VARS '''
        self.edgeList = []
        self.rev_edgeList = []
        self.indexOfNodes = []          # Prefix sum for outneighbours
        self.rev_indexOfNodes = []      # Prefix sum for inneighbours
        self.graph_edge = []            # All edges of graph
        self.edgeMap = []
        self.outDeg =  {}
        self.inDeg = {}
        ''' PUBLIC VARS '''
        
        
    def getEdges(self):
        return self.__edges
    
    def getEdgeLen(self):
        return self.__edgeLen
    
    def num_nodes(self):
        return self.__nodesTotal + 1
    
    def num_edges(self):
        return self.__edgesTotal
    
    def getInOutNbrs(self, v):
        pass

    def getEdge(self, s, d):
        startEdge = self.indexOfNodes[s]
        endEdge = self.indexOfNodes[s+1] - 1
        
        for e in self.getNeigbours(s):
            nbr = e.dest
            if nbr == d:
                return e
    

    def parseEdges(self):
        file = open(self.__filePath)

        self.__nodesTotal = int(file.readline())
        self.__edges = {i:[] for i in range(self.__nodesTotal+1)}


        # Parsing edges
        edge_line = file.readline()
        while edge_line:
            self.__edgesTotal +=1
            
            source, destination, weightVal = list(map(int,edge_line.split()))
            
            if source > self.__nodesTotal:
                self.__nodesTotal = source
                
            if destination > self.__nodesTotal:
                self.__nodesTotal = destination

            e = Edge(source, destination, weightVal)

            self.__edges[source].append(e)
            self.graph_edge.append(e)

            edge_line = file.readline()
            
        file.close()
    
    

    def parseGraph(self):
        self.parseEdges()

        # Sort the edges of each node
        for i in range(self.__nodesTotal+1):
            edgeOfVertex = self.__edges[i]
            edgeOfVertex.sort(key = lambda edge: edge.dest)
    

        self.indexOfNodes = [0] * (self.__nodesTotal+2)
        self.rev_indexOfNodes = [0] * (self.__nodesTotal+2)
        
        self.__edgeLen = [0] * self.__edgesTotal
        self.edgeList = [0] * self.__edgesTotal
        self.srcList = [0] * self.__edgesTotal
        
        self.edgeMap = [0] * self.__edgesTotal
        
        edgeMapInter = [0] * self.__edgesTotal
        vertexInter = [0] * self.__edgesTotal
        
        
        edge_no = 0
        # Prefix sum computation for out neighbours.
        # Loads indexOfNodes and EdgeList

        for i in range(self.__nodesTotal+1):

            edgeOfVertex = self.__edges[i]
            self.indexOfNodes[i] = edge_no

            for j in range(len(edgeOfVertex)):

                self.edgeList[edge_no] = edgeOfVertex[j].dest
                self.__edgeLen[edge_no] = edgeOfVertex[j].weight
                
                edge_no +=1

        self.indexOfNodes[self.__nodesTotal+1] = edge_no
        
        
        # Prefix sum computation for in neighbours.
        # Loads rev_indexOfNodes and srcList
        
        # Count indegrees first
        edge_indexinrevCSR = [0] * self.__edgesTotal
        
        for i in range(self.__nodesTotal+1):
            for j in range(self.indexOfNodes[i], self.indexOfNodes[i+1]):
                
                dest = self.edgeList[j]
                temp = self.rev_indexOfNodes[dest]
                self.rev_indexOfNodes[dest] += 1
                edge_indexinrevCSR[j] = temp
                
        # convert to revCSR
        prefix_sum = 0
        for i in range(self.__nodesTotal+1):
            temp = prefix_sum
            prefix_sum += self.rev_indexOfNodes[i]
            self.rev_indexOfNodes[i] = temp
        self.rev_indexOfNodes[self.__nodesTotal+1] = prefix_sum
        
        
        # Store the sources in srcList
        for i in range(self.__nodesTotal+1):
            for j in range(self.indexOfNodes[i], self.indexOfNodes[i+1]):
                dest = self.edgeList[j]
                index_in_srcList = self.rev_indexOfNodes[dest] + edge_indexinrevCSR[j]
                self.srcList[index_in_srcList] = i
                edgeMapInter[index_in_srcList] = j # RevCSR to CSR edge mapping
                vertexInter[index_in_srcList] = self.srcList[index_in_srcList] #store the original content of srcList before sorting srcList.
                
        
        # Sorting sources of each node.
        for i in range(self.__nodesTotal+1):
            sliceUpperLimit = self.rev_indexOfNodes[i+1]
            
            self.srcList[:sliceUpperLimit] = sorted(self.srcList[:sliceUpperLimit])
            
            vectSize = sliceUpperLimit - self.rev_indexOfNodes[i]
            
            for j in range(vectSize):
                srcListIndex = j + self.rev_indexOfNodes[i]
                for k in range(vectSize):
                    if vertexInter[k+self.rev_indexOfNodes[i]] == self.srcList[srcListIndex]:
                        self.edgeMap[srcListIndex] = edgeMapInter[k+self.rev_indexOfNodes[i]]
                        
                        break
                    
        
        for i in range(self.__nodesTotal+1):
            self.inDeg[i] = self.rev_indexOfNodes[i+1] - self.rev_indexOfNodes[i]
            self.outDeg[i] = self.indexOfNodes[i+1] - self.indexOfNodes[i]
                    
             
            

    def getOutNeighbors(self, node):
        return [edge.dest for edge in self.__edges[node]]
    
    def nodes_to(self, node):
        pass
        
    
    def nodes(self):
        return [i for i in range(self.__nodesTotal+1)]
    
    
    def get_edge(self, src, dest):
        for edge in self.__edges[src]:
            if edge.dest == dest:
                return edge
            
    
    def attachNodeProperty(self, **kwargs):
        for key,value in kwargs.items():
            setattr(self, key, {i:value for i in range(self.__nodesTotal+1)})
    
    
    def add_edge(self, src, dest, weight = 0):
        new_edge = Edge(src, dest, weight)

        self.__edges[new_edge.src].append(new_edge)
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
        self.__edgeLen.insert(insertAt, weight)

        for i in range(src+1, self.__nodesTotal+2):
            self.indexOfNodes[i] +=1

    
        self.__edgesTotal +=1
        self.indexOfNodes[self.__nodesTotal + 1] += 1


class DirGraph(Graph):
    pass


class UndirGraph(Graph):
    def add_edge(self, src, dest, weight=0):
        super().add_edge(src, dest, weight)

        # Append reverse edge
        reverse_edge = Edge(dest, src, weight)
        self.__edges.append(reverse_edge)