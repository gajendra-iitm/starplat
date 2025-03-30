// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "dyn4_modify.h"
#define THREADS_PER_BLOCK 1024
void checkCudaError( int  i)
{       
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess)  
  {   
    printf("%d , CUDA error: %s\n", i, cudaGetErrorString(error));
    exit(0);
  } 
} 
__global__ void print_excess(int V,int*d_excess){
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if(d_excess[v]!=0){
    printf("Active vertex:%d excess[%d]=%d\n",v,v,d_excess[v]);
  }
}

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
  const unsigned threadsPerBlock = THREADS_PER_BLOCK;
  unsigned numThreads   = (V < threadsPerBlock)? V: THREADS_PER_BLOCK;
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
    
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    staticMaxFlow_kernel_7<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height,cycle);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source2, ::source2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink2, ::sink2, sizeof(int), 0, cudaMemcpyDeviceToHost);
    
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
  printf("STATIC GPU Time: %.6f ms\n", milliseconds);

} //end FUN
void Incremental(graph& g, int V,int E,int* d_meta,int* d_data,int* d_rev_meta,int* d_src,int* d_weight, int source0, int sink0, int* d_residual_capacity, 
  int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess, int cycle
)
{

  const unsigned threadsPerBlock = THREADS_PER_BLOCK;
  unsigned numThreads   = (V < threadsPerBlock)? V: THREADS_PER_BLOCK;
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
    
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    incremental_kernel_12<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_excess,d_height,cycle);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source0, ::source0, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink0, ::sink0, sizeof(int), 0, cudaMemcpyDeviceToHost);
  
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

  const unsigned threadsPerBlock = THREADS_PER_BLOCK;
  unsigned numThreads   = (V < threadsPerBlock)? V: THREADS_PER_BLOCK;
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
    cudaMemcpyToSymbol(::sink1, &sink1, sizeof(int), 0, cudaMemcpyHostToDevice);
  
    cudaMemcpyToSymbol(::source1, &source1, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag3, &flag3, sizeof(bool), 0, cudaMemcpyHostToDevice);
    decremental_kernel_20<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_src,d_rev_meta,d_excess,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity,d_height2,cycle);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&sink1, ::sink1, sizeof(int), 0, cudaMemcpyDeviceToHost);
    
    cudaMemcpyFromSymbol(&source1, ::source1, sizeof(int), 0, cudaMemcpyDeviceToHost);
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
  unsigned threadsPerBlock   = (V < THREADS_PER_BLOCK)? V : THREADS_PER_BLOCK;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));

  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_excess,(int)0);

  staticMaxFlow(g,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,cycle);

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

    OnAdd_kernel<<<updateBlocks,updateThreads>>>(d_updateBatch,batchElements, d_meta, d_data, d_weight, d_rev_residual_capacity, d_residual_capacity);
    Decremental(g,V,E,d_meta,d_data,d_rev_meta,d_src,d_weight,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,cycle);

    Incremental(g,V,E,d_meta,d_data,d_rev_meta,d_src,d_weight,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,cycle);



  }
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);

   print_excess<<<numBlocks, threadsPerBlock>>>(V,d_excess);
  int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);
  cudaMemcpy(h_excess,d_excess,sizeof(int)*V,cudaMemcpyDeviceToHost);
  int val = h_excess[sink];
  free(h_excess);
  cudaFree(d_excess);
  printf("DYNAMIC FLOW: %d GPU Time: %.6f ms\n",val, milliseconds);
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
int  main( int  argc, char** argv) {
  char* originalgraph=argv[1];
  char* updatesinp = argv[2];
  int source = atoi(argv[3]);
  int sink = atoi(argv[4]);
  int kernel = atoi(argv[5]);
  int percent = atoi(argv[6]);
  graph G1(originalgraph,"cuda",true);
  G1.parseGraph();
  int batchsize = (percent*G1.num_edges())/100;

  std::vector<update> updateEdges=G1.parseUpdates(updatesinp);
  printf("batchsize:%d\n, updatesSize:%d\n",batchsize,updateEdges.size());
  recalculate_max_flow(G1,updateEdges,batchsize,source,sink,kernel);
  return 0;
}