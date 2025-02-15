// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "barenya.h"

void do_max_flow(graph& g,int source,int sink,int kernel_parameter
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
  int* d_label;
  cudaMalloc(&d_label, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_label,(int)0);

  initIndex<int><<<1,1>>>(V,d_label,source,(int)g.num_nodes()); //InitIndexDevice
  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  do_max_flow_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity);
  cudaDeviceSynchronize();



  int excesstotal = 0; // asst in .cu

  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  cudaMemcpyToSymbol(::excesstotal, &excesstotal, sizeof(int), 0, cudaMemcpyHostToDevice);
  do_max_flow_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_excess);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
  cudaMemcpyFromSymbol(&excesstotal, ::excesstotal, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  bool flag = true; // asst in .cu

  do{
    int x = kernel_parameter; // asst in .cu

    do{
      x = x + -1;
      cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
      do_max_flow_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_excess,d_residual_capacity,d_label);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu


    }while(x > 0);
    if (excess[source] + excess[sink] == excesstotal){ // if filter begin 
      flag = false;

    } // if filter end

  }while(flag);

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_excess);
  cudaFree(d_label);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
