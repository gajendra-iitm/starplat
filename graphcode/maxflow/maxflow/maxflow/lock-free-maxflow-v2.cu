// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "lock-free-maxflow-v2.h"
// #include "graph.hpp"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>

int do_max_flow(graph& g,int source,int sink)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_weight; //  ADDITION

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  // Need to sort out weight
  h_weight = (int *)malloc( (E)*sizeof(int));//  ADDITION

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
    h_weight[i] = edgeLen[i]; // ADDED
  }


  int* d_meta;
  int* d_data;
  int* d_weight; // ADDED

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));// ADDED

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_weight,   h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice); // ADDED

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
  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)g.num_nodes()); //InitIndexDevice
  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

  do_max_flow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_reverse_edge,d_weight);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  do_max_flow_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_reverse_edge,d_excess);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  bool flag = true; // asst in .cu

  do{
    flag = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag, &flag, sizeof(bool), 0, cudaMemcpyHostToDevice);
    do_max_flow_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_excess,d_reverse_edge,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag, ::flag, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag);

  //cudaFree up!! all propVars in this BLOCK!


  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
  int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);
  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost);
  int val = h_excess[sink];
  cudaFree(d_excess);
  cudaFree(d_height);
  free(h_excess);
  return val;

} //end FUN
int  main( int  argc, char** argv) {
  char* totalgraph=argv[1];
  int source = atoi(argv[2]);
  int sink = atoi(argv[3]);
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  int maxflow = do_max_flow(G1,source,sink);
  cudaDeviceSynchronize();
  printf("Maxflow:%d\n",maxflow);
  return 0;
}