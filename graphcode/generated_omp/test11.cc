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
  // graph G("../sample_graphs/Cora/cora_edgelist.txt");
  graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_edgelist.txt");

  G.parseGraph();

  // GNN gnn(G, "../sample_graphs/Cora/cora_features.txt", "../sample_graphs/Cora/cora_labels.txt");
  GNN gnn(G, "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_features.txt", "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_labels.txt");

  gnn.gcnPreprocessing();

  // print all the edges with weights
  // for (int nod = 0; nod < G.num_nodes(); nod++)
  // {
  //   for (auto edge : G.getNeighbors(nod))
  //   {
  //     cout << edge.source << "\t" << edge.destination << "\t" << edge.weight << endl;
  //   }
  // }

  printf("%s\n", environment.get_backend());

  std::vector<int32_t> neuronsPerHiddenLayer;
  // neuronsPerHiddenLayer.push_back(64);
  neuronsPerHiddenLayer.push_back(16);

  // void GNN::initializeLayers(std::vector<int> neuronsPerLayer, char *initType)
  gnn.initializeLayers(neuronsPerHiddenLayer, "xaviers");
  std::vector<Layers> &layers = gnn.getLayers();

  // graph &g = gnn.getGraph();
  // for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
  // {
  //   for (int i = 0; i < gnn.numFeatures(); i++)
  //   {
  //     std::cout << layers[0].inputFeatures[j][i] << "\t";
  //   }
  //   std::cout << std::endl;
  // }
  graph &g = gnn.getGraph();
  int num_epochs = 100;
  printf("Training started\n");
  for (int epoch = 1; epoch < num_epochs; epoch++)
  {
    for (int j = 0; j < layers.size(); j++)
    {
      for (int k = 0; k < g.num_nodes(); k++)
      {
        gnn.forwardPass(k, j, 1);
      }
    }
    for (int j = layers.size() - 1; j > 0; j--)
    {
      gnn.backPropogation(j);
    }
    gnn.adamOptimizer(epoch, 0.01, 0.9, 0.999, 1e-8,0.001);
    gnn.predict();

    cout << epoch << endl;
  }

  for (int j = 0; j < layers.size(); j++)
  {
    for (int k = 0; k < g.num_nodes(); k++)
    {
      gnn.forwardPass(k, j,1);
    }
  }
  gnn.predict();

  // print all the matrices associated with all the layers
  // for (int j = 1; j < layers.size(); j++)
  // {
  //   cout << "Layer " << j << " weights" << endl;
  //   for (int k = 0; k < layers[j - 1].num_features; k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].weights[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }
  //   cout << "Layer " << j << " aggregated features" << endl;
  //   for (int k = 0; k < g.num_nodes(); k++)
  //   {
  //     for (int l = 0; l < layers[j - 1].num_features; l++)
  //     {
  //       cout << layers[j].aggregatedFeatures[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }
  //   cout << "Layer " << j << " pre activated features" << endl;
  //   for (int k = 0; k < g.num_nodes(); k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].preActivatedFeatures[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }
  //   cout << "Layer " << j << " post activated features" << endl;
  //   for (int k = 0; k < g.num_nodes(); k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].postActivatedFeatures[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }

  //   cout << "Layer " << j << " grad pre act output" << endl;
  //   for (int k = 0; k < g.num_nodes(); k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].grad_pre_act_output[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }

  //   cout << "Layer " << j << " grad post act output" << endl;
  //   for (int k = 0; k < g.num_nodes(); k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].grad_post_act_output[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }

  //   cout << "Layer " << j << " grad weights" << endl;
  //   for (int k = 0; k < layers[j - 1].num_features; k++)
  //   {
  //     for (int l = 0; l < layers[j].num_features; l++)
  //     {
  //       cout << layers[j].grad_weights[k][l] << "\t";
  //     }
  //     cout << endl;
  //   }
  // }

  // print all outputfeatures of all nodes of layer 2
  // for (int j = 0; j < g.num_nodes(); j++)
  // {
  //   for (int i = 0; i < layers[layers.size() - 1].num_features; i++)
  //   {
  //     std::cout << layers[layers.size() - 1].postActivatedFeatures[j][i] << "\t";
  //   }
  //   std::cout << std::endl;
  // }

  // for (int j = 0; j < num_nodes; j++)
  // {
  //   //layers[0].preActivatedFeatures[0][j] = 0;
  //   for (int i = 0; i < layers[0].num_features; i++)
  //   {
  //     std::cout<<layers[0].preActivatedFeatures[0][j]<<"\t";
  //   }
  //   std::cout<<std::endl;
  //   // layers[0].preActivatedFeatures[0][j] = relu(layers[0].preActivatedFeatures[0][j] + layers[0].bias[j]);
  // }
}