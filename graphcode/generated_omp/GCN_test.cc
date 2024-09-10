#include"GCN_test.h"

void GCN(graph& g , GNN  gnn , vector<int> neuronsPerHiddenLayer)
{
  initializeLayers_omp(neuronsPerHiddenLayer,"xaviers");

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
        if (layerr == neuronsPerHiddenLayer.size() + 1 )
          {
          y = false;
        }
        forwardPass_omp(nod,layerr);

      }
    }
    while (layerr){
      layerr--;
      backPropogation_omp(layerr);

    }
  }

}
