from graphs.edge import Edge
from scipy.sparse import csr_matrix
import numpy as np

class Graph:
    def __init__(self, nodesTotal = 0, edgesTotal = 0):
        self.nodesTotal = nodesTotal
        # Nodes from 0 to nodesTotal - 1

        self.edgesTotal = edgesTotal
        self.edgesCSR  = csr_matrix((nodesTotal, nodesTotal), dtype=int)
        self.edgesCSR.data[:] = np.full(self.edgesCSR.data.shape, -1)

        # self.indexOfNodes = [0] * (nodesTotal)
        # self.edgeList = []
        # self.edgeLen = []


    def add_edge(self, src, dest, weight = 0):
        self.edgesCSR[src,dest] = weight


        # startIndex = self.indexOfNodes[src]
        # endIndex = self.indexOfNodes[src+1]
        # nbrsCount = endIndex - startIndex
        # insertAt = 0

        # if (self.edgeList[startIndex] >= dest) or (nbrsCount == 0):
        #     insertAt = startIndex

        # elif self.edgeList[endIndex-1] <= dest:
        #     insertAt = endIndex

        # else:
        #     for i in range(startIndex, endIndex-1):
        #         if self.edgeList[i] <= dest and self.edgeList[i+1] >= dest:
        #             insertAt = i+1
        #             break
        # self.edgeList.insert(insertAt, dest)
        # self.edgeLen.insert(insertAt, weight)

        # for i in range(src+1, self.nodesTotal+2):
        #     self.indexOfNodes[i] +=1

        # newEdge = Edge(src, dest, weight)
        # self.edges[src].append(newEdge)
        # self.edgesTotal +=1



class DirGraph(Graph):
    # Nothing to extend for now
    pass


class UndirGraph(Graph):
    def add_edge(self, src, dest, weight=0):
        super().add_edge(src, dest, weight)

        # Append reverse edge
        reverse_edge = Edge(dest, src, weight)
        self.edges.append(reverse_edge)