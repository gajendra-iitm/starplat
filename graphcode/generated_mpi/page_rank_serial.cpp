#include <iostream>
#include <vector>
#include <fstream>
#include <climits>
#include <cmath>

class Graph
{
public:
    int nodesTotal;
    int startNode;
    int endNode;
    int edgeCount = 0;
    std::vector<std::vector<int>> adjList;
    std::vector<std::vector<int>> inAdjList;

    // TODO: Refactor this class into a reusable component
    Graph(const char *filename)
    {
        startNode = INT_MAX;
        endNode = INT_MIN;

        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Unable to open file\n";
            return;
        }

        int s, d, w;
        while (file >> s >> d >> w)
        {
            startNode = std::min(std::min(s, d), startNode);
            endNode = std::max(std::max(s, d), endNode);
            if (s >= adjList.size() || d >= adjList.size())
            {
                adjList.resize(std::max(s, d) + 1);
                inAdjList.resize(std::max(s, d) + 1);
            }
            adjList[s].push_back(d);
            inAdjList[d].push_back(s);
            edgeCount++;
        }
        nodesTotal = endNode - startNode + 1;

        file.close();

        std::cout << "Nodes: " << nodesTotal << "\n";
        std::cout << "Edges: " << edgeCount << "\n";
    }

    std::vector<int> getInNeighbors(int v)
    {
        return inAdjList[v];
    }

    int num_out_nbrs(int v)
    {
        return adjList[v].size();
    }
};

void ComputePageRank(Graph &g, float beta, float delta, int maxIter,
                     std::vector<float> &pageRank)
{
    int numNodes = g.nodesTotal;
    std::vector<float> pageRankNext(numNodes, 0.0f);
    std::fill(pageRank.begin(), pageRank.end(), 1.0f / numNodes);
    int iterCount = 0;
    float diff = 0.0f;

    do
    {
        diff = 0.0f;
        for (int v = g.startNode; v <= g.endNode; v++)
        {
            float sum = 0.0f;
            for (int nbr : g.getInNeighbors(v))
            {
                sum += pageRank[nbr] / g.num_out_nbrs(nbr);
            }

            float newPageRank = (1 - delta) / numNodes + delta * sum;
            diff += std::fabs(newPageRank - pageRank[v]);
            pageRankNext[v] = newPageRank;
        }

        pageRank = pageRankNext;
        iterCount++;
    } while ((diff > beta) && (iterCount < maxIter));
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    Graph graph(argv[1]);
    std::vector<float> pageRank(graph.nodesTotal, 0.0f);
    float beta = 0.0001f;
    float delta = 0.85f;
    int maxIter = 100;

    clock_t start = clock();
    ComputePageRank(graph, beta, delta, maxIter, pageRank);
    clock_t end = clock();

    std::cout << "PageRank Values:\n";
    for (int v = graph.startNode; v <= graph.endNode; v++)
    {
        std::cout << "Node " << v << ": " << pageRank[v] << "\n";
    }

    std::cout << "TIME: [" << (double)(end - start) / CLOCKS_PER_SEC << "]" << std::endl;
    return 0;
}
