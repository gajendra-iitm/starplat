// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "lock-free-maxflow-v3_1.h"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>
__global__ void update_kernel(int src, int dest,int new_capacity, int* d_residual_capacity,int *d_rev_residual_capacity,int *d_weight, int* d_meta,int* d_data){
  int edge = -1;
  for( int  i=d_meta[src];i<d_meta[src+1];i++)
  {
    if(d_data[i]==dest)
    {
      edge = i;
      break;
    }
  }
  
  printf("src:%d dest:%d new_capacity:%d old_capacity:%d residual[src,dst]:%d residual[dst,src]:%d\n",src,dest,new_capacity,d_weight[edge],d_residual_capacity[edge],d_rev_residual_capacity[edge]);

  if(new_capacity<d_weight[edge]){
    if(d_rev_residual_capacity[edge]>new_capacity){
      d_rev_residual_capacity[edge] = new_capacity;
    }
    d_residual_capacity[edge] = new_capacity - d_rev_residual_capacity[edge];
    
  } else{
     d_residual_capacity[edge] =  d_residual_capacity[edge] + new_capacity - d_weight[edge];
  }
  d_weight[edge] = new_capacity;
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
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_src, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));
  cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(   d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1)), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START



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

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

    cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);
  do_max_flow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  do_max_flow_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_weight,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  bool flag2 = true; // asst in .cu

  do{
    flag2 = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    do_max_flow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag2, ::flag2, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag2);

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
int recalculate_max_flow(graph& g,char *updatesinp,int source2,int sink2)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_src, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));
  cudaMalloc(&d_rev_meta, sizeof(int)*(V+1));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(   d_src,    h_src, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1)), cudaMemcpyHostToDevice);

  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


    int *h_excess; // ADDED BY SHRUTHI
  h_excess = (int *)malloc(sizeof(int)*V); // ADDED BY SHRUTHI


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

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

    // TIMER START
  cudaEvent_t start1, stop1;
  cudaEventCreate(&start1);
  cudaEventCreate(&stop1);
  float milliseconds1 = 0;
  cudaEventRecord(start1,0);
  recalculate_max_flow_kernel_10<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
  recalculate_max_flow_kernel_14<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_weight,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  bool flag = true; // asst in .cu

  do{
    flag = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag, &flag, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_16<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag, ::flag, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag);





  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  cudaEventRecord(stop1,0);
  cudaEventSynchronize(stop1);
  cudaEventElapsedTime(&milliseconds1, start1, stop1);
  printf("Original Graph GPU Time: %.6f ms\n", milliseconds1); // ADDED BY SHRUTHI

  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost); // ADDED BY SHRUTHI
  printf("Static Maxflow:%d\n",h_excess[sink2]); // ADDED BY SHRUTHI
  printf("After static calculation\n");  // ADDED BY SHRUTHI
  print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);


  std::vector<update> updateEdges=g.parseUpdates(updatesinp);

  for(auto &u:updateEdges){
    update_kernel<<<1,1>>>(u.source,u.destination,u.weight,d_residual_capacity,d_rev_residual_capacity,d_weight,d_meta,d_data);
  }
    
  recalculate_max_flow_kernel_19<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_excess);
  cudaDeviceSynchronize();
      print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);

  int* d_height2;
  cudaMalloc(&d_height2, sizeof(int)*(V));

    cudaEvent_t start2, stop2;
  cudaEventCreate(&start2);
  cudaEventCreate(&stop2);
  float milliseconds2 = 0;
  cudaEventRecord(start2,0);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height2,(int)0);

  initIndex<int><<<1,1>>>(V,d_height2,sink2,(int)g.num_nodes()); //InitIndexDevice
  cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
  recalculate_max_flow_kernel_22<<<numBlocks, threadsPerBlock>>>(V,E,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  flag = true;
  do{
    flag = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag, &flag, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_24<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity,d_excess,d_height2);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag, ::flag, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu


  }while(flag);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source2,(int)g.num_nodes()); //InitIndexDevice
  flag = true;
  do{
    flag = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag, &flag, sizeof(bool), 0, cudaMemcpyHostToDevice);
    recalculate_max_flow_kernel_27<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag, ::flag, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu

    ; // asst in .cu

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
    cudaEventRecord(stop2,0);
  cudaEventSynchronize(stop2);
  cudaEventElapsedTime(&milliseconds2, start2, stop2);
  printf("Dynamic GPU Time: %.6f ms\n", milliseconds2);

    cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost);
  int val = h_excess[sink2];

  cudaFree(d_height2);
  cudaFree(d_excess);
  cudaFree(d_height);

  //TIMER STOP
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
  printf("reval:%d eval%d\n",reval,eval);
  if(reval!=eval){
    
    printf("=====================PANIC INCORRECT========================\n");
  } else {
    printf("=====================ANSWER MATCHES========================\n");
  }
  return 0;
}