#include "test11.h"

void test1(graph &g, GNN &gnn, std::vector<vector<float>> features, int *labels)
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
  graph G("/Users/s.tharun_dyanish/Documents/vscode/StarPlat_Updated/graphcode/sample_graphs/Pubmed/pubmed_edgelist.txt");

  G.parseGraph();

  GNN gnn(G, "/Users/s.tharun_dyanish/Documents/vscode/StarPlat_Updated/graphcode/sample_graphs/Pubmed/pubmed_dataset.txt", "/Users/s.tharun_dyanish/Documents/vscode/StarPlat_Updated/graphcode/sample_graphs/Pubmed/pubmed_labels.txt");
  gnn.gcnPreprocessing();

  // print all the edges with weights
  // for (int nod = 0; nod < G.num_nodes(); nod++)
  // {
  //   for (auto edge : G.getNeighbors(nod))
  //   {
  //     cout << edge.source << " " << edge.destination << " " << edge.weight << endl;
  //   }
  // }

  printf("%s\n", environment.get_backend());

  std::vector<int32_t> neuronsPerHiddenLayer;
  neuronsPerHiddenLayer.push_back(32);
  neuronsPerHiddenLayer.push_back(16);

  // void GNN::initializeLayers(std::vector<int> neuronsPerLayer, char *initType)
  gnn.initializeLayers(neuronsPerHiddenLayer, "xaviers");
  std::vector<layer> &layers = gnn.getLayers();

  graph &g = gnn.getGraph();
  // for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
  // {
  //   for (int i = 0; i < gnn.numFeatures(); i++)
  //   {
  //     std::cout << layers[0].inputFeatures[j][i] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  for (int epoch = 1; epoch < 50; epoch++)
  {
    for (int j = 0; j < layers.size(); j++)
    {
      for (int k = 0; k < g.num_nodes(); k++)
      {
        gnn.forwardPass(k, j);
      }
    }
    for (int j = layers.size() - 1; j >= 0; j--)
    {
      gnn.backPropogation(j);
    }
    gnn.adamOptimizer(epoch, 0.01, 0.9, 0.999, 1e-8);
    gnn.predict();

    cout<<epoch<<endl;
  }

  for (int j = 0; j < layers.size(); j++)
  {
    for (int k = 0; k < g.num_nodes(); k++)
    {
      gnn.forwardPass(k, j);
    }
  } 
gnn.predict();
  // print all outputfeatures of all nodes of layer 2
  // for (int j = 0; j < g.num_nodes(); j++)
  // {
  //   for (int i = 0; i < layers[layers.size() - 1].num_features; i++)
  //   {
  //     std::cout << layers[layers.size() - 1].postActivatedFeatures[j][i] << " ";
  //   }
  //   std::cout << std::endl;
  // }

  // for (int j = 0; j < num_nodes; j++)
  // {
  //   //layers[0].preActivatedFeatures[0][j] = 0;
  //   for (int i = 0; i < layers[0].num_features; i++)
  //   {
  //     std::cout<<layers[0].preActivatedFeatures[0][j]<<" ";
  //   }
  //   std::cout<<std::endl;
  //   // layers[0].preActivatedFeatures[0][j] = relu(layers[0].preActivatedFeatures[0][j] + layers[0].bias[j]);
  // }

  // for (int i = 0; i < layers.size() - 1; i++)
  // {
  //   cout << layers[i].num_features << endl;
  //   // print weights
  //   for (int j = 0; j < layers[i].num_features; j++)
  //   {
  //     for (int k = 0; k < layers[i + 1].num_features; k++)
  //     {
  //       cout << i << "$" << j << "$" << k << " ";
  //     }
  //     cout << endl;
  //   }
  // }
}
