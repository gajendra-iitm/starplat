#include"GCN_test.h"

void GCN(graph& g , string gnn , vector<int> neuronsPerHiddenLayer)
{
  gnn.initialize_layers(neuronsPerHiddenLayer,"xaviers");

  int num_epoch = 1;
  bool x = true;
  while (x){
    if (num_epoch == 100 )
      {
      x = false;
    }
    num_epoch = num_epoch + 1;
    int layerr = 0;
    bool y = 0;
    while (y){
      layerr = layerr + 1;
      #pragma omp parallel for
      for (int nod = 0; nod < g.num_nodes(); nod ++) 
      {
        if (layerr == neuronsPerHiddenLayer.size() )
          {
          y = false;
        }
        int num_neurons = layers.getIdx(x);
        gnn.forwardPass(nod,n,features);

      }
    }
    while (layerr){
      layerr--;
      gnn.backPropagation(layerr);

    }
  }

}
