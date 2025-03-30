// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "dyn4_ud_simple.h"

void staticMaxFlow(graph& g,int source,int sink,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_parallel_edge,
  int kernel_cycles)

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
  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)g.num_nodes()); //InitIndexDevice
  int V = g.num_nodes(); // asst in .cu

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  staticMaxFlow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity,d_parallel_edge);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  staticMaxFlow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_reverse_edge,d_residual_capacity,d_excess,d_rev_residual_capacity,d_parallel_edge);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  bool flag1 = true; // asst in .cu

  do{
    flag1 = false;
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)V);

    d_height[sink] = 0;

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source=0;
    bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
    int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));
    int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

    //EXTRA vars INITIALIZATION
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_level,-1);
    initIndex<int><<<1,1>>>(V,d_level,sink, 0);

    // long k =0 ;// For DEBUG
    do {
      finished = true;
      cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_parallel_edge); ///DONE from varList

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
        ,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_parallel_edge); ///DONE from varList

      hops_from_source--;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
    }
    //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::V, &V, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::kernel_cycles, &kernel_cycles, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    staticMaxFlow_kernel_12<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_residual_capacity,d_height,d_rev_residual_capacity,d_parallel_edge,d_reverse_edge,d_excess);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&V, ::V, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&kernel_cycles, ::kernel_cycles, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



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


  }while(flag1);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_height);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
void Incremental(graph& g, int V,int E,int* d_meta,int* d_data,int* d_rev_meta,int* d_src,int* d_weight, int source, int sink, int* d_parallel_edge, 
  int* d_residual_capacity, int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess, 
  int* d_height, int kernel_cycles)
{

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)g.num_nodes()); //InitIndexDevice
  int V = g.num_nodes(); // asst in .cu

  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  incremental_kernel_15<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_residual_capacity,d_reverse_edge,d_excess,d_rev_residual_capacity,d_parallel_edge);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  bool flag1 = true; // asst in .cu

  do{
    flag1 = false;
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::kernel_cycles, &kernel_cycles, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::V, &V, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    incremental_kernel_18<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_rev_residual_capacity,d_excess,d_reverse_edge,d_height,d_residual_capacity,d_parallel_edge);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&kernel_cycles, ::kernel_cycles, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&V, ::V, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



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


  }while(flag1);


}
void Decremental(graph& g, int V,int E,int* d_meta,int* d_data,int* d_rev_meta,int* d_src,int* d_weight, int source, int sink, int* d_parallel_edge, 
  int* d_residual_capacity, int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess, 
  int* d_height, int cycle)
{

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,sink,(int)g.num_nodes()); //InitIndexDevice
  int V = g.num_nodes(); // asst in .cu

  cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
  decremental_kernel_21<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_excess,d_residual_capacity,d_parallel_edge);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  bool flag1 = true; // asst in .cu

  do{
    flag1 = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::V, &V, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    decremental_kernel_24<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_residual_capacity,d_height,d_parallel_edge,d_reverse_edge,d_rev_residual_capacity);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&V, ::V, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);



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


  }while(flag1);


}
void recalculate_max_flow(graph& g, std::vector<update> updateBatch, int batchSize, int source, 
  int sink, int cycle)
{
   int V = g.num_nodes();
   int E = g.num_edges();
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
  int *d_meta;
  int *d_data;
  int *d_src;
  int *d_weight;
  int *d_rev_meta;
  cudaMalloc(&d_meta,sizeof(int)*(V+1));
  cudaMalloc(&d_data,sizeof(int)*(E));
  cudaMalloc(&d_src,sizeof(int)*(E));
  cudaMalloc(&d_weight,sizeof(int)*(E));
  cudaMalloc(&d_rev_meta,sizeof(int)*(V+1));
  cudaMemcpy(d_meta,h_meta,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  cudaMemcpy(d_data,h_data,sizeof(int)*(E),cudaMemcpyHostToDevice);
  cudaMemcpy(d_src,h_src,sizeof(int)*(E),cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight,h_weight,sizeof(int)*(E),cudaMemcpyHostToDevice);
  cudaMemcpy(d_rev_meta,h_rev_meta,sizeof(int)*(V+1),cudaMemcpyHostToDevice);
  unsigned threadsPerBlock   = (V < THREADS_PER_BLOCK)? V: THREADS_PER_BLOCK;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  int* d_parallel_edge;
  cudaMalloc(&d_parallel_edge, sizeof(int)*(E));

  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  staticMaxFlow(g,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_parallel_edge,cycle);

  int _batchSize = batchSize;
  update *d_updateBatch;
  cudaMalloc(&d_updateBatch,sizeof(update)*_batchSize);
  int batchElements = 0;
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);

  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += _batchSize){
    if((updateIndex + _batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else
    batchElements = _batchSize ;
    cudaMemcpy(d_updateBatch,&updateBatch[updateIndex],batchElements*sizeof(update),cudaMemcpyHostToDevice);
    unsigned updateThreads = (batchElements < THREADS_PER_BLOCK)? batchElements: THREADS_PER_BLOCK;
    unsigned updateBlocks = (batchElements+updateThreads-1)/updateThreads;

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    OnAdd_kernel<<<updateBlocks,updateThreads>>>(d_updateBatch,batchElements, d_meta, d_data, d_weight, d_rev_residual_capacity, d_parallel_edge, d_residual_capacity);
    recalculate_max_flow_kernel_28<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_excess);
    cudaDeviceSynchronize();



    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    ; // asst in .cu

    Decremental(g,V,E,d_meta,d_data,d_rev_meta,d_src,d_weight,source,sink,d_parallel_edge,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_height,cycle);

    Incremental(g,V,E,d_meta,d_data,d_rev_meta,d_src,d_weight,source,sink,d_parallel_edge,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_height,cycle);



  }
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
  cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
  recalculate_max_flow_kernel_32<<<numBlocks, threadsPerBlock>>>(V,E,d_excess);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);





  cudaMemcpy(h_meta,d_meta,sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
  cudaMemcpy(h_data,d_data,sizeof(int)*(E),cudaMemcpyDeviceToHost);
  cudaMemcpy(h_src,d_src,sizeof(int)*(E),cudaMemcpyDeviceToHost);
  cudaMemcpy(h_weight,d_weight,sizeof(int)*(E),cudaMemcpyDeviceToHost);
  cudaMemcpy(h_rev_meta,d_rev_meta,sizeof(int)*(V+1),cudaMemcpyDeviceToHost);
  cudaFree(d_meta);
  cudaFree(d_data);
  cudaFree(d_src);
  cudaFree(d_weight);
  cudaFree(d_rev_meta);
}
