// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "dyn4_test.h"

void staticMaxFlow(graph& g,int source2,int sink2,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int cycle
)

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

  initIndex<int><<<1,1>>>(V,d_height,source2,(int)g.num_nodes()); //InitIndexDevice
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  staticMaxFlow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();



  cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
  staticMaxFlow_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  bool flag1 = true; // asst in .cu

  int kernel = 1000; // asst in .cu

  do{
    flag1 = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::cycle, &cycle, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    staticMaxFlow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&cycle, ::cycle, sizeof(int), 0, cudaMemcpyDeviceToHost);
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
void Incremental(graph& g, int V,int E,int* d_meta,int* d_data,int* d_rev_meta,int* d_src,int* d_weight, int source0, int sink0, int* d_residual_capacity, 
  int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess, int cycle
)
{

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source0,(int)g.num_nodes()); //InitIndexDevice
  cudaMemcpyToSymbol(::source0, &source0, sizeof(int), 0, cudaMemcpyHostToDevice);
  incremental_kernel_10<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source0, ::source0, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  bool flag2 = true; // asst in .cu

  do{
    flag2 = false;
    cudaMemcpyToSymbol(::source0, &source0, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink0, &sink0, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::cycle, &cycle, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    incremental_kernel_12<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_height,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source0, ::source0, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink0, ::sink0, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&cycle, ::cycle, sizeof(int), 0, cudaMemcpyDeviceToHost);
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


}
void Decremental(graph& g, int V,int E,int* d_meta,int* d_data,int* d_rev_meta,int* d_src,int* d_weight, int source1, int sink1, int* d_residual_capacity, 
  int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess, int cycle
)
{

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? V: 512;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  int* d_height2;
  cudaMalloc(&d_height2, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height2,(int)0);

  initIndex<int><<<1,1>>>(V,d_height2,sink1,(int)g.num_nodes()); //InitIndexDevice
  decremental_kernel_15<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_excess);
  cudaDeviceSynchronize();



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  cudaMemcpyToSymbol(::sink1, &sink1, sizeof(int), 0, cudaMemcpyHostToDevice);
  decremental_kernel_18<<<numBlocks, threadsPerBlock>>>(V,E,d_src,d_rev_meta,d_reverse_edge,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&sink1, ::sink1, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  bool flag3 = true; // asst in .cu

  do{
    flag3 = false;
    cudaMemcpyToSymbol(::source1, &source1, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink1, &sink1, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::cycle, &cycle, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag3, &flag3, sizeof(bool), 0, cudaMemcpyHostToDevice);
    decremental_kernel_20<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity,d_height2);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source1, ::source1, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink1, ::sink1, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&cycle, ::cycle, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag3, ::flag3, sizeof(bool), 0, cudaMemcpyDeviceToHost);



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


  }while(flag3);


}
