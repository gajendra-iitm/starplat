#include"GCN_test.h"

void GCN(graph& g , vector<int> neuronsPerHiddenLayer)
{
   GNN gnn(g, "../sample_graphs/Cora/cora_features.txt", "../sample_graphs/Cora/cora_labels.txt");
  gnn.gcnPreprocessing();
  gnn.initializeLayers(neuronsPerHiddenLayer,"xaviers");

  int num_epoch = 0;
  bool x = true;
  while (x){
    if (num_epoch == 100 )
      {
      x = false;
    }
    num_epoch = num_epoch + 1;
    int layerr = 0;
    bool y = true;
    while (y){
      layerr = layerr + 1;
      #pragma omp parallel for
      for (int nod = 0; nod < g.num_nodes(); nod ++) 
      {
        if (layerr == neuronsPerHiddenLayer.size() + 1 )
          {
          y = false;
        }
        gnn.forwardPass(nod,layerr);

      }
    }
    while (layerr){
        gnn.backPropogation(layerr);
        layerr--;
    }
    gnn.predict();
  }

}
int main(){
    graph G("../sample_graphs/Cora/cora_edgelist.txt");

  G.parseGraph();
  vector<int> neuronsPerHiddenLayer;
  neuronsPerHiddenLayer.push_back(16);
  GCN(G,neuronsPerHiddenLayer);
  return 0;

}


/*#include "GCN_test.h"

void GCN(graph &g, vector<int> neuronsPerHiddenLayer)
{
  GNN gnn(g, "../sample_graphs/Cora/cora_features.txt", "../sample_graphs/Cora/cora_labels.txt");
  gnn.gcnPreprocessing();

  gnn.initializeLayers(neuronsPerHiddenLayer, "xaviers");

  int num_epoch = 0;
  bool x = true;
  while (x)
  {
    if (num_epoch == 100)
    {
      x = false;
    }
    num_epoch = num_epoch + 1;
    int layerr = 0;
    bool y = true;

    while (y)
    {
      layerr = layerr + 1;
      #pragma omp parllel for
      for (int nod = 0; nod < g.num_nodes(); nod++)
      {
        if (layerr == neuronsPerHiddenLayer.size() + 1)
        {
          y = false;
        }
        gnn.forwardPass(nod, layerr);
      }
    }
    cout << "Backpropogation" << endl;
    // cout << layerr << endl;
    while (layerr)
    {

      // printf("layerr = %d\n",layerr);
      gnn.backPropogation(layerr);
      layerr--;
    }
    gnn.adamOptimizer(num_epoch, 0.01, 0.9, 0.999, 1e-8, 0.001);
    gnn.predict();
    cout << num_epoch << endl;
  }
}
int main()
{
  graph G("../sample_graphs/Cora/cora_edgelist.txt");

  G.parseGraph();
  ;

  std::vector<int32_t> neuronsPerHiddenLayer;
  // neuronsPerHiddenLayer.push_back(64);
  neuronsPerHiddenLayer.push_back(16);

  // void GNN::initializeLayers(std::vector<int> neuronsPerLayer, char *initType)
  GCN(G, neuronsPerHiddenLayer);
  return 0;
}*/