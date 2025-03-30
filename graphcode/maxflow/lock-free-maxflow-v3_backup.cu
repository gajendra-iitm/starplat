// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
// #include "graph.hpp"
#include "lock-free-maxflow-v3.h"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>



__global__ void update_kernel(int src, int dest,int new_capacity, int* d_residual_capacity,int *d_reverse_edge,int *d_weight, int* d_meta,int* d_data){
  int backward_edge = -1;
  int forward_edge = -1;
  for( int  i=d_meta[src];i<d_meta[src+1];i++)
  {
    if(d_data[i]==dest)
    {
      forward_edge = i;
      break;
    }
  }
  backward_edge = d_reverse_edge[forward_edge];
  printf("src:%d dest:%d new_capacity:%d old_capacity:%d residual[src,dst]:%d residual[dst,src]:%d\n",src,dest,new_capacity,d_weight[forward_edge],d_residual_capacity[forward_edge],d_residual_capacity[backward_edge]);

  if(new_capacity<d_weight[forward_edge]){
    if(d_residual_capacity[backward_edge]>new_capacity){
      d_residual_capacity[backward_edge] = new_capacity;
    }
    d_residual_capacity[forward_edge] = new_capacity - d_residual_capacity[backward_edge];
    
  } else{
     d_residual_capacity[forward_edge] =  d_residual_capacity[forward_edge] + new_capacity - d_weight[forward_edge];
  }
  d_weight[forward_edge] = new_capacity;
}

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
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
  int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);
  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost);
  int val = h_excess[sink];
  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_excess);
  cudaFree(d_height);
  free(h_excess);
  return val;
} //end FUN



int recalculate_max_flow(graph& g, char *updatesinp,int source2,int sink2)

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

    int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);

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

  initIndex<int><<<1,1>>>(V,d_height,source2,(int)g.num_nodes()); //InitIndexDevice
  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

  recalculate_max_flow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_reverse_edge,d_weight);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
  recalculate_max_flow_kernel_2<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_reverse_edge,d_excess);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  bool flag2 = true; // asst in .cu

  do{
    flag2 = false;
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_3<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_excess,d_reverse_edge,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag2, ::flag2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag2);

  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost);
  printf("Static Maxflow:%d\n",h_excess[sink2]);
  printf("After static calculation\n");
  print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_data);
  // recalculate_max_flow_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_residual_capacity,d_excess,d_meta,d_weight);
  // cudaDeviceSynchronize();

  std::vector<update> updateEdges=g.parseUpdates(updatesinp);

  for(auto &u:updateEdges){
    update_kernel<<<1,1>>>(u.source,u.destination,u.weight,d_residual_capacity,d_reverse_edge,d_weight,d_meta,d_data);
  }
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);
  recalculate_max_flow_kernel_4<<<numBlocks, threadsPerBlock>>>(V,E,d_residual_capacity,d_excess,d_meta,d_weight);
  cudaDeviceSynchronize();
  
  printf("After updates \n");
  print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_data);
  int* d_height2;
  cudaMalloc(&d_height2, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height2,(int)0);

  initIndex<int><<<1,1>>>(V,d_height2,sink2,(int)g.num_nodes()); //InitIndexDevice
  cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
  recalculate_max_flow_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_reverse_edge,d_residual_capacity,d_excess);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  flag2 = true;
  do{
    flag2 = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_6<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_excess,d_reverse_edge,d_residual_capacity,d_height2);
    cudaDeviceSynchronize();

    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag2, ::flag2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag2);
  printf("After negative max flow calculation\n");
  print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);


  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source2,(int)g.num_nodes()); //InitIndexDevice
  flag2 = true;
  do{
    flag2 = false;
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_excess,d_reverse_edge,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag2, ::flag2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag2);
  printf("After +ve maxflow calculation\n");
  print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost);
  int val = h_excess[sink2];
  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_excess);
  cudaFree(d_height2);
  cudaFree(d_height);
  free(h_excess);
  return val;

} //end FUN

int  main( int  argc, char** argv) {

  char* totalgraph=argv[1];
  char* updatedgraph = argv[2];
  char* updatesinp = argv[3];
  int source = atoi(argv[4]);
  int sink = atoi(argv[5]);
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();

   int reval = recalculate_max_flow(G1,updatesinp,source,sink);
  cudaDeviceSynchronize();
  graph G2(updatedgraph,"cuda",true);
  G2.parseGraph();
  int eval = do_max_flow(G2,source,sink);
  if(reval!=eval){
    printf("reval:%d eval%d\n",reval,eval);
    printf("=====================PANIC INCORRECT========================\n");
  } else {
    printf("=====================ANSWER MATCHES========================\n");
  }
  return 0;
}