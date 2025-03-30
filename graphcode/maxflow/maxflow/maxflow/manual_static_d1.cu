// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "manual_static.h"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>

#define THREADS_PER_BLOCK 512

void checkCudaError( int  i)
{       
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess)  
  {   
    printf("%d , CUDA error: %s\n", i, cudaGetErrorString(error));
    exit(0);
  } 
} 
__global__ void update_edges_kernel(int batchelements,update* d_batch_updates,int *d_weight, int* d_meta,int* d_data){
  
  int id  = blockIdx.x * blockDim.x + threadIdx.x;
  if(id>=batchelements) return;
  update u = d_batch_updates[id];
  int src = u.source;
  int dest = u.destination;
  int new_capacity = u.weight;
  char type = u.type;
  int edge = -1;
  for( int  i=d_meta[src];i<d_meta[src+1];i++)
  {
    if(d_data[i]==dest)
    {
      edge = i;
      break;
    }
  }
  d_weight[edge] = new_capacity;
}
int calculate_max_flow(graph& g, std::vector<update> updateBatch, int batchSize, int source, int sink)

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
  // UPDATE THE WEIGHTS
  update *d_batch_updates;//ADDED
  cudaMalloc(&d_batch_updates,sizeof(update)*batchSize);  //ADDED
  int batchElements = 0;
  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += batchSize){
    if((updateIndex + batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else batchElements = batchSize ;
    cudaMemcpy(d_batch_updates,&updateBatch[updateIndex],batchElements*sizeof(update),cudaMemcpyHostToDevice);
    unsigned numThreadsupdate   = (batchElements <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: batchElements;
    unsigned numBlocksupdate    = (batchElements+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;

    // HERE WRITING AS A SINGLE KERNEL --> CAN HAVE TWO KERNELS FOR ADD AND DELETE
    update_edges_kernel<<<numBlocksupdate,numThreadsupdate>>>(batchElements,d_batch_updates,d_weight,d_meta,d_data);
    cudaDeviceSynchronize();
    checkCudaError(0);
  }


  //LAUNCH CONFIG
  const unsigned threadsPerBlock = THREADS_PER_BLOCK;
  unsigned numThreads   = (V < threadsPerBlock)? THREADS_PER_BLOCK: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  int *h_excess; // ADDED BY SHRUTHI
  h_excess = (int *)malloc(sizeof(int)*V); // ADDED BY SHRUTHI


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



  
    // TIMER START
  cudaEvent_t start1, stop1;
  cudaEventCreate(&start1);
  cudaEventCreate(&stop1);
  float milliseconds1 = 0;
  cudaEventRecord(start1,0);
  calculate_max_flow_kernel_10<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(10);



  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  calculate_max_flow_kernel_14<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_weight,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(14);
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);




  bool flag = true; // asst in .cu

  do{
    flag = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag, &flag, sizeof(bool), 0, cudaMemcpyHostToDevice);
    calculate_max_flow_kernel_16<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    checkCudaError(16);
    print_excess<<<numBlocks,numThreads>>>(V,d_excess);
    print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag, ::flag, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  }while(flag);

  cudaEventRecord(stop1,0);
  cudaEventSynchronize(stop1);
  cudaEventElapsedTime(&milliseconds1, start1, stop1);
  printf("STATIC GPU Time: %.6f ms\n", milliseconds1);

  cudaMemcpy(h_excess, d_excess, V * sizeof(int), cudaMemcpyDeviceToHost); 
  int val = h_excess[sink];

  cudaFree(d_excess);
  cudaFree(d_height);

  //TIMER STOP
    free(h_excess);
  return val;

} //end FUN
int  main( int  argc, char** argv) {
  char* totalgraph=argv[1];
  char* updatesinp = argv[2];
  int source = atoi(argv[3]);
  int sink = atoi(argv[4]);
  int batchsize = atoi(argv[5]);
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  std::vector<update> updateEdges=G1.parseUpdates(updatesinp);
  int reval = calculate_max_flow(G1,updateEdges,batchsize,source,sink);
  cudaDeviceSynchronize();
  printf("EVAL FLOW VALUE:%d\n",reval);
  return 0;
}
