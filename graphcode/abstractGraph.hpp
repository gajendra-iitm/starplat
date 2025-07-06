#ifndef STARPLAT_ABSTRACTGRAPH_HPP
#define STARPLAT_ABSTRACTGRAPH_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>
#include "update.hpp"
#include "graph_ompv2.hpp"

class edge
{
public:
    int32_t source;
    int32_t destination;
    int32_t weight;
    int32_t id; /* -unique Id for each edge.
                   -useful in adding properties to edges. */
    int dir;
};

class AbstractGraph
{
public:
    virtual graph copyGraph() = 0;
    virtual std::map<int, std::vector<edge>> getEdges() = 0;
    virtual int *getEdgeLen() = 0;
    virtual int num_nodes() = 0;
    virtual int num_edges() = 0;
    virtual std::vector<edge> getInOutNbrs(int v) = 0;
    virtual edge getEdge(int s, int d) = 0;
    virtual void randomShuffle() = 0;
    virtual bool inRouteFromSource(int candidate, int dest, int *parent) = 0;
    virtual bool check_if_nbr(int s, int d) = 0;
    virtual int common_nbrscount(int node1, int node2) = 0;
    virtual int getOutDegree(int v) = 0;
    virtual int getInDegree(int v) = 0;
    virtual void addEdge(int src, int dest, int aks) = 0;
    virtual void delEdge(int src, int dest) = 0;
    virtual std::vector<update> parseUpdates(char *updateFile) = 0;
    virtual std::vector<update> getDeletesFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec) = 0;
    virtual std::vector<update> getAddsFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec) = 0;
    virtual void propagateNodeFlags(bool *modified) = 0;
    virtual void parseEdges() = 0;
    virtual void parseEdgesResidual() = 0;
    virtual void parseGraphResidual() = 0;
    virtual void parseGraph() = 0;
    virtual void updateCSRDel(std::vector<update> &batchUpdate, int k, int size) = 0;
    virtual void updateCSRAdd(std::vector<update> &batchUpdate, int k, int size) = 0;
    virtual std::vector<edge> getNeighbors(int node) = 0;
    virtual std::vector<edge> getInNeighbors(int node) = 0;
    virtual graph getMST() = 0;
    virtual void printGraph() = 0;
};

#endif