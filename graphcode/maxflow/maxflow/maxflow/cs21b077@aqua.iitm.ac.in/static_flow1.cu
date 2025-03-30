// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "static_flow.h"
#define THREADS_PER_BLOCK 1024
void checkCudaError( int  i)
{       
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess)  
  {   
    printf("%d , CUDA error: %s\n", i, cudaGetErrorString(error));
  } 
} 

__global__ void print_excess(int V,int*d_excess){
   unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  printf("excess[%d]=%d\n",v,d_excess[v]);

}
__global__ void print_residual(int V, int *d_meta,int *d_residual_capacity,int *d_rev_residual_capacity, int *d_data){
     unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for(int edge = d_meta[v];edge<d_meta[v+1];edge++){
    int dst = d_data[edge];
    printf("residual[%d to %d ]=%d \n residual[%d to %d ]=%d \n ",v,dst,d_residual_capacity[edge],dst,v,d_rev_residual_capacity[edge]);
  }
}

int staticMaxFlow(graph& g,int source2,int sink2,int kernel)

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
  const unsigned threadsPerBlock = THREADS_PER_BLOCK;
  unsigned numThreads   = (V < threadsPerBlock)? V: THREADS_PER_BLOCK;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;





  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

    // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source2,(int)g.num_nodes()); //InitIndexDevice
  staticMaxFlow_kernel_1<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(1);


  cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
  staticMaxFlow_kernel_5<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(5);
  cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
  print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
  cudaDeviceSynchronize();

  ; // asst in .cu

  bool flag1 = true; // asst in .cu

  do{
    flag1 = false;
    cudaMemcpyToSymbol(::source2, &source2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink2, &sink2, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::kernel, &kernel, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    staticMaxFlow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_reverse_edge,d_excess,d_height,d_rev_residual_capacity,d_residual_capacity);
    cudaDeviceSynchronize();
    checkCudaError(7);
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&kernel, ::kernel, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
    print_residual<<<numBlocks, threadsPerBlock>>>(V, d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
    cudaDeviceSynchronize();

    break;
  }while(flag1);



  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);
    //cudaFree up!! all propVars in this BLOCK!
  int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);
  cudaMemcpy(h_excess,d_excess,sizeof(int)*V,cudaMemcpyDeviceToHost);
  int val = h_excess[sink2];
  free(h_excess);
  cudaFree(d_height);
  cudaFree(d_excess);
  return val;
} //end FUN
int  main( int  argc, char** argv) {

  char* totalgraph=argv[1];
  int source = atoi(argv[2]);
  int sink = atoi(argv[3]);
  int kernel = atoi(argv[4]);
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  int eval = staticMaxFlow(G1,source,sink,kernel);
  cudaDeviceSynchronize();
  printf("STATIC EVAL VALUE:%d\n",eval);
  return 0;
}