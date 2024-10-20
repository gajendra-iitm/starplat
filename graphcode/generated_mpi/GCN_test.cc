#include"GCN_test.h"

void GCN(Graph& g, Container<int>& neuronsPerHiddenLayer, boost::mpi::communicator world )
{
  gnn.initializeLayers(neuronsPerHiddenLayer,"xaviers");

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
      gnn.forwardPass(layerr,"SAGE");

    }
    while (layerr){
      layerr--;
      gnn.backPropogation(layerr);

    }
  }

}
