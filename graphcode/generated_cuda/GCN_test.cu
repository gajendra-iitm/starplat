// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "GCN_test.h"

void GCN(graph& g,GNN  gnn,thrust::device_vector<int> neuronsPerHiddenLayer)

{
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();




  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V > threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  initializeLayers_cuda(neuronsPerHiddenLayer,"xaviers");

  int num_epoch = 1;
  bool x = true;
  while(x){
    if (num_epoch == 100){ // if filter begin 
      x = false;

    } // if filter end
    num_epoch = num_epoch + 1;
    int layerr = 0;
    bool y = 0;
    while(y){
      layerr = layerr + 1;
      forwardPass_cuda(layerr);



    }while(layerr){
      layerr--;
      backPropogation_cuda(layerr);



    }

  }
  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
