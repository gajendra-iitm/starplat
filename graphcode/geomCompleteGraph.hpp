#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <string.h>
#include <climits>
#include <cmath>
#include "./abstractGraph.hpp"
#include "./graph.hpp"

class geomCompleteGraph : public AbstractGraph
{
private:
    int32_t nodesTotal;
    int32_t edgesTotal;
    char *filePath;
    bool isVRP;

    int capacity;


public:
    std::vector<std::vector<int>> coordinates;
    std::vector<int> demand;

    graph copyGraph()
    {
        // TODO implemente this method
        geomCompleteGraph g_copy(filePath);
        return g_copy;
    }

    void randomShuffle(){
        // TODO Implement this method
        return;
    }

    void printGraph(){
        // TODO: Implement this method
    }


    // proprietary function to calculate distance from depot
    int calculateDepotDistance(int i, int j){
        int dist = 0;
        for (int k = 0; k < coordinates[i].size(); k++)
        {
            dist += (coordinates[i][k]) * (coordinates[i][k]);
        }
        dist = std::sqrt(dist);
        
        int dist2 = 0;
        for (int k = 0; k < coordinates[j].size(); k++)
        {
            dist2 += (coordinates[j][k]) * (coordinates[j][k]);
        }
        dist2 = std::sqrt(dist2);
        dist += dist2;

        return dist;
    }

    // Proprietary function to calculate distance between two nodes
    int calculateDistance(int i, int j)
    {
        int dist = 0;
        for (int k = 0; k < coordinates[i].size(); k++)
        {
            dist += (coordinates[i][k] - coordinates[j][k]) * (coordinates[i][k] - coordinates[j][k]);
        }
        dist = std::sqrt(dist);
        return dist;
    }


    geomCompleteGraph(char *file)
    {
        this->nodesTotal = 0;
        this->edgesTotal = 0;
        this->filePath = file;
        std::string fileName(file);
        if (fileName.find(".vrp") != std::string::npos || fileName.find("vrp") != std::string::npos) {
            this->isVRP = true;
        }
    }

    ~geomCompleteGraph()
    {
        // delete[] edgeLen;
        // delete[] edgeList;
        // delete[] indexofNodes;
        // delete[] rev_indexofNodes;
    }

    std::map<int, std::vector<edge>> getEdges()
    {
        std::map<int32_t, std::vector<edge>> edges;
        for (int i = 0; i < nodesTotal; i++)
        {
            for (int j = 0; j < nodesTotal; j++)
            {
                if(i==j) continue;
                edge e;
                e.source = i;
                e.destination = j;
                e.weight = calculateDistance(i, j);
                e.id = i;
                e.dir = 1;
                edges[i].push_back(e);
            }
        }
        return edges;
    }

    int *getEdgeLen()
    {
        int32_t *edgeLen;
        edgeLen = new int32_t[edgesTotal];
        int count = 0;
        for (int i = 0; i < nodesTotal; i++)
        {
            for (int j = 0; j < nodesTotal; j++)
            {
                if(i==j) continue;
                edgeLen[count++] = calculateDistance(i, j);
            }
        }
        return edgeLen;
    }

    int num_nodes()
    {
        return nodesTotal;
    }

    int num_edges()
    {
        // if (diff_edgeList != NULL)
        // {
        //     return (edgesTotal + diff_indexofNodes[nodesTotal + 1]);
        // }
        // else
        // {
        return edgesTotal;
        // }
    }

    // Propretary method to parse adjacency list from a map
    int getNeighbourFromEdge(int src, int edge){
        if(edge >= src){
            return edge + 1;
        }else{
            return edge;
        }
    }

    std::vector<edge> getInOutNbrs(int v)
    {
        std::vector<edge> resVec;
        std::vector<edge> inEdges = getInNeighbors(v);
        resVec.insert(resVec.end(), inEdges.begin(), inEdges.end());
        std::vector<edge> Edges = getNeighbors(v);
        resVec.insert(resVec.end(), Edges.begin(), Edges.end());
        return resVec;
    }

    edge getEdge(int s, int d)
    {
        edge e;
        e.source = s;
        e.destination = d;
        e.weight = calculateDistance(s, d);
        e.id = s;
        e.dir = 1;
        return e;
    }

    bool inRouteFromSource(int candidate, int dest, int *parent)
    {

        while (parent[dest] != -1)
        {
            if (parent[dest] == candidate)
                return true;

            dest = parent[dest];
        }

        return false;
    }

    bool check_if_nbr(int s, int d)
    {
        return true;
    }

    int common_nbrscount(int node1, int node2)
    {
        return true;
    }

    int getOutDegree(int v)
    {
        return nodesTotal;
    }

    int getInDegree(int v)
    {

        return nodesTotal;
    }

    void addEdge(int src, int dest, int weight)
    {
        return;
    }

    void delEdge(int src, int dest)
    {
        return;
    }

    std::vector<update> parseUpdates(char *updateFile)
    {
        std::vector<update> update_vec = parseUpdateFile(updateFile);
        return update_vec;
    }

    std::vector<update> getDeletesFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
    {
        std::vector<update> deleteVec = getDeletions(updateIndex, batchSize, updateVec);
        return deleteVec;
    }

    std::vector<update> getAddsFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
    {
        std::vector<update> addVec = getAdditions(updateIndex, batchSize, updateVec);
        return addVec;
    }

    void propagateNodeFlags(bool *modified)
    {
        bool finished = false;
        while (!finished)
        {
            finished = true;
            for (int v = 0; v <= nodesTotal; v++)
            {
                for (edge e : getNeighbors(v))
                {
                    if (!modified[e.destination])
                    {
                        modified[e.destination] = true;
                        finished = false;
                    }
                }
            }
        }
    }

    void parseEdges()
    {
        if(this->isVRP){
            coordinates.clear();
            demand.clear();
            std::ifstream in(filePath);
            std::string line;
            for (int i = 0; i < 3; ++i)
                getline(in, line);
             getline(in, line);
            auto size = stof(line.substr(line.find(":") + 2));
            getline(in, line);
            auto type = line.find(":");
            getline(in, line);
            auto capacity = stof(line.substr(line.find(":") + 2));
            this->capacity = capacity;
            getline(in, line);

            for (size_t i = 0; i < size; ++i) {
                getline(in, line);
                std::stringstream iss(line);
                size_t id;
                std::string xStr, yStr;
                iss >> id >> xStr >> yStr;
                std::vector<int> coord;
                coord.push_back(stoi(xStr));
                coord.push_back(stoi(yStr));
                coordinates.push_back(coord);
            }
            getline(in, line);
            for (size_t i = 0; i < size; ++i) {
                getline(in, line);
                std::stringstream iss(line);
                size_t id;
                std::string dStr;
                iss >> id >> dStr;
                demand.push_back(stoi(dStr));
                //assert(i==(id-1));
            }
            in.close();
            nodesTotal = coordinates.size();
            edgesTotal = nodesTotal * (nodesTotal - 1);
        }else{
            std::ifstream infile;
            infile.open(filePath);
            std::string line;
            while (std::getline(infile, line))
            {
                std::stringstream ss(line);
                int32_t node;
                std::vector<int> coord;
                ss >> node;
                int32_t x;
                ss >> x;
                while (ss >> x)
                {
                    coord.push_back(x);
                }
                coordinates.push_back(coord);
            }
            infile.close();
            nodesTotal = coordinates.size();
            edgesTotal = nodesTotal * (nodesTotal - 1) ;
        }       
    }

    inline int* getDemands(){
        return this->demand.data();
    }

    int getCapacity(){
        return this->capacity;
    }

    void parseEdgesResidual()
    {
        // TODO: Implement this
        return;
    }

    void parseGraphResidual()
    {
        // TODO: Implement this
        return;
    }

    void parseGraph()
    {
        parseEdges();
    }

    std::vector<edge> getNeighbors(int node)
    {
        std::vector<edge> out_edges;
        for (int i = 0; i < nodesTotal; i++)
        {
            if (i != node)
            {
                edge e;
                e.source = node;
                e.destination = i;
                e.weight = calculateDistance(node, i);
                e.id = i;
                e.dir = 1;
                out_edges.push_back(e);
            }
        }
        return out_edges;
    }

    std::vector<edge> getInNeighbors(int node)
    {
        std::vector<edge> in_edges;
        for (int i = 0; i < nodesTotal; i++)
        {
            if (i != node)
            {
                edge e;
                e.source = i;
                e.destination = node;
                e.weight = calculateDistance(i, node);
                e.id = i;
                e.dir = 1;
                in_edges.push_back(e);
            }
        }
        return in_edges;
    }

    void updateCSRDel(std::vector<update> &batchUpdate, int k, int size)
    {
        // TODO: Implement this
        return;
    }

    void updateCSRAdd(std::vector<update> &batchUpdate, int k, int size)
    {
        // TODO: Implement this
        return;
    }

    graph getMST() {
    int V = nodesTotal + 1;
    std::vector<bool> inMST(V, false);
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);

    typedef std::pair<int, int> P;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    key[0] = 0;
    pq.push({0, 0});

    graph mstGraph((char*)"");

    std::map<int, std::vector<edge>> tempMST;
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u])
            continue;
        inMST[u] = true;
        if (parent[u] != -1) {
            edge edgeTemp;
            edgeTemp.source = u;
            edgeTemp.destination = parent[u];
            edgeTemp.weight = key[u];
            tempMST[u].push_back(edgeTemp);

            edge edgeTemp2;
            edgeTemp2.destination = u;
            edgeTemp2.source = parent[u];
            edgeTemp2.weight = key[u];
            tempMST[parent[u]].push_back(edgeTemp2);
        }
        std::vector<edge> neighbors = this->getNeighbors(u);
        for (auto &e : neighbors) {
            int v = e.destination;
            int w = e.weight;
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }
    mstGraph.setNodes(tempMST.size());
    mstGraph.parseAdjacencyList(tempMST);

    // mstGraph.printGraph();    

    return mstGraph;
}
};