#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <climits>

class Graph
{
public:
    int nodesTotal;
    int startNode;
    int endNode;
    int edgeCount = 0;
    std::vector<std::vector<int>> adjList;

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
            }
            adjList[s].push_back(d);
            adjList[d].push_back(s);
            edgeCount++;
        }
        nodesTotal = endNode - startNode + 1;

        file.close();

        std::cout << "Nodes: " << nodesTotal << "\n";
        std::cout << "Edges: " << edgeCount << "\n";
    }

    std::vector<int> getNeighbors(int v)
    {
        return adjList[v];
    }

    bool check_if_nbr(int u, int v)
    {
        for (int w : adjList[u])
        {
            if (w == v)
            {
                return true;
            }
        }
        return false;
    }
};

long Compute_TC(Graph &g)
{
    long triangle_count = 0;
    for (int v = g.startNode; v <= g.endNode; v++)
    {
        for (int u : g.getNeighbors(v))
        {
            if (u < v)
            {
                for (int w : g.getNeighbors(v))
                {
                    if (w > v)
                    {
                        if (g.check_if_nbr(u, w))
                        {
                            triangle_count++;
                        }
                    }
                }
            }
        }
    }
    return triangle_count;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    Graph graph(argv[1]);
    clock_t start = clock();
    long triangle_count = Compute_TC(graph);
    clock_t end = clock();
    std::cout << "Triangle Count: " << triangle_count << "\n";
    std::cout << "TIME: [" << (double)(end - start) / CLOCKS_PER_SEC << "]" << std::endl;
    return 0;
}
