// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "graph.hpp"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>
#include "mst_dsl_boruvkas-modify.h"
using namespace std;




void mst_boruvkas(graph& g)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
  }


  int* d_meta;
  int* d_data;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int* d_parent;
  cudaMalloc(&d_parent, sizeof(int)*(V));

  int* d_minedge;
  cudaMalloc(&d_minedge, sizeof(int)*(V));

  int* d_minweight;
  cudaMalloc(&d_minweight, sizeof(int)*(V));

  bool* d_isCovered;
  cudaMalloc(&d_isCovered, sizeof(bool)*(E));

  int* d_destination;
  cudaMalloc(&d_destination, sizeof(int)*(E));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_parent,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minedge,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minweight,(int)-1);


  // Initializing arrays
  mst_boruvkas_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_isCovered,d_destination,d_parent);
  cudaDeviceSynchronize();



  bool hasComponent = true; // asst in .cu

  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  bool* d_modified_next;
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  

  do{
    hasComponent = false;
    bool finished = false; // asst in .cu  // need to shift in dsl code
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minedge,(int)-1);

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minweight,(int)-1);

    // finding min weight for each node
    mst_boruvkas_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_minweight,d_isCovered);
    cudaDeviceSynchronize();



    ; // asst in .cu
    // finding min edge for each node   // can club 2 and 3 to one
    mst_boruvkas_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_minweight,d_isCovered,d_minedge);
    cudaDeviceSynchronize();



    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)true);

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finished) {

      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      mst_boruvkas_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_minedge,d_minweight,d_modified,d_parent);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu


      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    mst_boruvkas_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_parent,d_destination,d_minedge);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    cudaMemcpyToSymbol(::hasComponent, &hasComponent, sizeof(bool), 0, cudaMemcpyHostToDevice);
    mst_boruvkas_kernel_6<<<numBlocks, threadsPerBlock>>>(V,E,d_parent,d_destination,d_minedge,d_isCovered);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&hasComponent, ::hasComponent, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    finished = false;
    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finished) {

      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      mst_boruvkas_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_parent);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu


      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT


  }while(hasComponent);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_minweight);
  cudaFree(d_minedge);
  cudaFree(d_modified);
  cudaFree(d_parent);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  bool *h_isCovered;
  h_isCovered = (bool*)malloc(sizeof(bool)*E);
  cudaMemcpy(h_isCovered,d_isCovered,cudaMemcpyDeviceToHost);
  //PANIC Need to create edge weight array 
  long long int mstw = 0; 
  for(int i=0;i<E;i++)
  {
    if(h_isCovered[i]){
      mstw+=edgeLen[i];
    }
  }
  return mstw;
} //end FUN

int  main( int  argc, char** argv) {
  char* totalgraph=argv[1];
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  long long int eval = Boruvka(G1);
  printf("MST weight:%lld\n",eval);
  return 0;
}