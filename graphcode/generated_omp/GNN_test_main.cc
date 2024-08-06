#ifndef GENCPP_TEST_H
#define GENCPP_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <atomic>
#include <omp.h>
#include "../graph.hpp"
#include "../atomicUtil.h"

using namespace std;
// void test1(graph& g , float* features , int* labels);

#endif

int main()
{
    graph G("/Users/s.tharun_dyanish/Documents/vscode/StarPlat_Updated/graphcode/sample_graphs/sample_graph.txt");

    G.parseGraph();

    int num_nodes = G.num_nodes();
    int num_edges = G.num_edges();

    cout << "Number of nodes: " << num_nodes << endl;
    cout << "Number of edges: " << num_edges << endl;

    for (int nod = 0; nod < G.num_nodes(); nod++)
    {
        for (auto edge : G.getNeighbors(nod))
        {
            cout << edge.source << " " << edge.destination << endl;
        }
    }

    // for (int nod = 0; nod < G.num_nodes(); nod++)
    // {
    //     for (auto edge : G.getEdges()[nod])
    //     {
    //         cout << edge.source << "    " << edge.destination << endl;
    //     }
    // }
}
