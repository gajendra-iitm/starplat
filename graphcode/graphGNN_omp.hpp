#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>

#ifndef GNN_OMP_H
#define GNN_OMP_H

class GNN;
class graph;

void gcn_preprocessing_omp(GNN &gnn)
{
  // Symmettric notmalization of the adjacency matrix (present in CSR form)
  // Add self loops to the adjacency matrix

  graph &g = gnn.getGraph();
  // change weight of all edges to 1
  int num_edges = g.num_edges();
#pragma omp parallel for
  for (int i = 0; i < num_edges; i++)
  {
    g.edgeList[i].weight = 1;
  }

  int num_nodes = g.num_nodes();
  for (int i = 0; i < num_nodes; i++)
  {
    g.addEdge(i, i, 1);
  }
}

#endif