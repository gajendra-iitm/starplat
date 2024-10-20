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

  WHILE()
{}
      GNN.FORWARD()
      GNN.BACKWARD()       
  }
    }

}








/* forward(){

    if (graphsage){
    for(int i = 0; i < num_nodes/batchsize; i++){
        graphsage(batchSize);
        }
    }


  }*/