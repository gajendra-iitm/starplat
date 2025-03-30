// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "bfs_maxflow2.h"

void computeBC(graph& g,int n,int source,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_height,int* d_reverse_edge)

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
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)n);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)0); //InitIndexDevice
  bool* d_scanned;
  cudaMalloc(&d_scanned, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_scanned,(bool)false);

  initIndex<bool><<<1,1>>>(V,d_scanned,source,(bool)true); //InitIndexDevice

  //EXTRA vars for ITBFS AND REVBFS
  bool finished;
  int hops_from_source=0;
  bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
  int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));
  int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

  //EXTRA vars INITIALIZATION
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_level,-1);
  initIndex<int><<<1,1>>>(V,d_level,source, 0);

  // long k =0 ;// For DEBUG
  do {
    finished = true;
    cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

    //Kernel LAUNCH
    fwd_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,d_residual_capacity,d_rev_residual_capacity,d_height,d_reverse_edge); ///DONE from varList

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    incrementDeviceVar<<<1,1>>>(d_hops_from_source);
    cudaDeviceSynchronize(); //MUST - rupesh
    ++hops_from_source; // updating the level to process in the next iteration
    // k++; //DEBUG

    cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
  }while(!finished);

  hops_from_source--;
  cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);

  //BACKWARD PASS
  while(hops_from_source > 1) {

    //KERNEL Launch
    back_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished
      ,d_residual_capacity,d_rev_residual_capacity,d_height,d_reverse_edge); ///DONE from varList

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
  }
  //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_scanned);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
