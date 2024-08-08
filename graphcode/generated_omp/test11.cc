#include "test11.h"

void test1(graph &g, float *features, int *labels)
{
  //   int num_epoch = 100;
  //   int num_layer = 5;
  //   int x = true;
  //   vector<int> layers;
  //   layers.pushback(num_features);

  //   layers.pushback(16);

  //   layers.pushback(num_classes);

  //   int x = 1;
  //   initialize_layers(layers, Xavier_transform);

  //   bool flag = true;
  //   bool flag1 = true;
  //   while (flag1)
  //   {
  //     x = 0;
  //     x = x + 1;
  // #pragma omp parallel for
  //     for (int nod = 0; nod < g.num_nodes(); nod++)
  //     {
  //       for (int edge = g.indexofNodes[nod]; edge < g.indexofNodes[nod + 1]; edge++)
  //       {
  //         int n = g.edgeList[edge];
  //         if (labels[n] == labels[nod])
  //         {
  //           int num_neurons = layers.getIdx(x);
  //           gnn.message_pass(nod, n, features, "sum", num_neurons, 1);
  //         }
  //       }
  //     }
  //   }
}

int main()
{
  graph G("/Users/s.tharun_dyanish/Documents/vscode/StarPlat_Updated/graphcode/sample_graphs/sample_graph.txt");

  G.parseGraph();
  // bool* modified_add ;
  // bool* modified_del;

  // // int triangleC =  Compute_TC(G);
  // int elements = 0;

  // for(int nod = 0; nod < G.num_nodes(); nod ++)
  // {
  //   for(auto edge: G.getEdges()[nod])
  //   {
  //     cout<<edge.source<<" "<<edge.destination<<endl;
  //   }
  // }

  GNN gnn(G, "", "");
  gnn.gcn_preprocessing();

  // print all the edges with weights
  for (int nod = 0; nod < G.num_nodes(); nod++)
  {
    for (auto edge : G.getNeighbors(nod))
    {
      cout << edge.source << " " << edge.destination << " " << edge.weight << endl;
    }
  }

  printf("%s\n", environment.get_backend());
}
