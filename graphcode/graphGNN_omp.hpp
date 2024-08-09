#include "graph.hpp"

void gcn_preprocessing_omp(GNN &gnn)
{
  // Symmettric notmalization of the adjacency matrix (present in CSR form)
  // Add self loops to the adjacency matrix

  graph &g = gnn.getGraph();
  // change weight of all edges to 1
  int num_edges = g.num_edges();
#pragma omp parallel for
  for (int nod = 0; nod < g.num_nodes(); nod++)
  {
    for (auto &edge : g.getNeighbors(nod))
    {
      g.changeWeight(edge.source, edge.destination, 1);
    }
  }

  int num_nodes = g.num_nodes();
  for (int i = 0; i < num_nodes; i++)
  {
    g.addEdge(i, i, 1);
  }

  // calculate degree matrix
  std::vector<float> degree(num_nodes, 0);
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    for (auto edge : g.getNeighbors(nod))
    {
      degree[nod] += edge.weight;
    }
  }

  // calculate inverse of degree matrix
  std::vector<float> inv_degree(num_nodes, 0);
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    inv_degree[nod] = 1 / sqrt(degree[nod]);
  }

  // calculate the normalized adjacency matrix
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    for (auto &edge : g.getNeighbors(nod))
    {
      g.changeWeight(edge.source, edge.destination, edge.weight * inv_degree[edge.source] * inv_degree[edge.destination]);
    }
  }
}