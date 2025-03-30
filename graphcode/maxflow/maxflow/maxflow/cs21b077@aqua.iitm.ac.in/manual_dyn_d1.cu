// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "manual_dyn.h"
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
__global__ void update_edges_kernel(int batchelements,update* d_batch_updates, int* d_residual_capacity,int *d_rev_residual_capacity,int *d_weight, int* d_meta,int* d_data){
  
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
  printf("update src:%d dst%d newcap:%d old weight:%d\n",src,dest,new_capacity,d_weight[edge]);
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
__global__ void update_excess(int V, int *d_residual_capacity, int *d_rev_residual_capacity, int *d_meta, int *d_excess ,int *d_reverse_edge, int*d_rev_meta){
  int id  = blockIdx.x * blockDim.x + threadIdx.x;
  if(id>=V) return;
  int e = 0;
  for(int fedge = d_meta[id];fedge<d_meta[id+1];fedge++){
    e-=d_rev_residual_capacity[fedge];
    if(id==4){
      printf("sub d_rev_residual_cap:%d\n",d_rev_residual_capacity[fedge]);
    }
  }
  for(int bedge = d_rev_meta[id];bedge<d_rev_meta[id+1];bedge++){
      int edge = d_reverse_edge[bedge];
      e+=d_rev_residual_capacity[edge];
      if(id==4){
      printf("add d_rev_residual_cap:%d\n",d_rev_residual_capacity[edge]);
    }
  }
  d_excess[id] = e;
}
void staticMaxFlow(graph& g,int source,int sink,int* residual_capacity,int* rev_residual_capacity,int* reverse_edge,int* excess)
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
  
  unsigned numThreads   = (V <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: V;
  unsigned numBlocks    = (V+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;



  int *d_excess ; // ADDED
  cudaMalloc(&d_excess,sizeof(int)*V); //ADDED
  int* d_residual_capacity; //ADDED
  cudaMalloc(&d_residual_capacity,sizeof(int)*E);//ADDED
  int* d_rev_residual_capacity;//ADDED
  cudaMalloc(&d_rev_residual_capacity,sizeof(int)*E);//ADDED
  int* d_reverse_edge;//ADDED
  cudaMalloc(&d_reverse_edge,sizeof(int)*E);//ADDED

  int* d_height;
  cudaMalloc(&d_height, sizeof(int)*(V));

  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);



  initKernel<int> <<<numBlocks, numThreads>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)g.num_nodes()); //InitIndexDevice
  initKernel<int> <<<numBlocks, numThreads>>>(V,d_excess,(int)0);

  staticMaxFlow_kernel_1<<<numBlocks,  numThreads>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_residual_capacity,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(1);

    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
  staticMaxFlow_kernel_3<<<numBlocks,  numThreads>>>(V,E,d_meta, d_data, d_weight,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
      checkCudaError(3);
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
// printf("push from source:%d sink:%d\n",source,sink);
    //  print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  // print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
  
  bool flag1 = true; // asst in .cu

  do{
    flag1 = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag1, &flag1, sizeof(bool), 0, cudaMemcpyHostToDevice);
    staticMaxFlow_kernel_5<<<numBlocks,  numThreads>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_excess,d_height);
    cudaDeviceSynchronize();
    checkCudaError(5);
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag1, ::flag1, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  }while(flag1);

  //cudaFree up!! all propVars in this BLOCK!
   print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Initial Static GPU Time: %.6f ms\n", milliseconds);


  cudaMemcpy(residual_capacity, d_residual_capacity, sizeof(int)*(E), cudaMemcpyDeviceToHost);
  cudaMemcpy(rev_residual_capacity, d_rev_residual_capacity, sizeof(int)*(E), cudaMemcpyDeviceToHost);
  cudaMemcpy(reverse_edge, d_reverse_edge, sizeof(int)*(E), cudaMemcpyDeviceToHost);
  cudaMemcpy(  excess, d_excess, sizeof(int)*(V), cudaMemcpyDeviceToHost);

  // Property declared within function is cleared on its own!
  cudaFree(d_height);
  

  // Graph arrays not cleared.
  cudaFree(d_meta);
  cudaFree(d_data);
  cudaFree(d_src);
  cudaFree(d_weight);
  cudaFree(d_rev_meta);


  // properties passed in as parameters! not cleared as not malloced
  cudaFree(d_excess); //ADDED
  cudaFree(d_residual_capacity);//ADDED
  cudaFree(d_rev_residual_capacity);//ADDED
  cudaFree(d_reverse_edge);//ADDED

  
} //end FUN
void Incremental(int V, int E, int* d_meta,int* d_data,int* d_src,int* d_weight,int* d_rev_meta,int source, int sink, int* d_residual_capacity, int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess,int* d_height)
{
  //LAUNCH CONFIG
  unsigned numThreads   = (V <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: V;
  unsigned numBlocks    = (V+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;

  initKernel<int> <<<numBlocks, numThreads>>>(V,d_height,(int)0);

  initIndex<int><<<1,1>>>(V,d_height,source,(int)V); //InitIndexDevice
  cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
  incremental_kernel_8<<<numBlocks,  numThreads>>>(V,E,d_meta,d_data,d_weight,d_residual_capacity,d_excess,d_rev_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(8);
  cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
  printf("AFTER INCREMENTAL PUSH FROM SRC:%d SINK:%d\n",source,sink);
  print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);

  bool flag2 = true;// how to deal with this?

  do{
    flag2 = false;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag2, &flag2, sizeof(bool), 0, cudaMemcpyHostToDevice);
    incremental_kernel_10<<<numBlocks,  numThreads>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_excess,d_reverse_edge,d_rev_residual_capacity,d_residual_capacity,d_height);
    cudaDeviceSynchronize();
    checkCudaError(10);
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag2, ::flag2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    break;
  }while(flag2);
  printf("AFTER INCREMENTAL LOOP SRC:%d SINK:%d\n",source,sink);
  print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
}
void Decremental( int V, int E, int* d_meta,int* d_data,int* d_src,int* d_weight,int* d_rev_meta, int source, int sink, int* d_residual_capacity, int* d_rev_residual_capacity, int* d_reverse_edge, int* d_excess,int* d_height2)
{
  //LAUNCH CONFIG
  unsigned numThreads   = (V <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: V;
  unsigned numBlocks    = (V+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;

  initKernel<int> <<<numBlocks, numThreads>>>(V,d_height2,(int)0);

  initIndex<int><<<1,1>>>(V,d_height2,sink,(int)V); //InitIndexDevice
  cudaDeviceSynchronize();

  cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
  decremental_kernel_16<<<numBlocks,  numThreads>>>(V,E,d_src,d_weight,d_rev_meta,d_reverse_edge,d_excess,d_rev_residual_capacity,d_residual_capacity);
  cudaDeviceSynchronize();
  checkCudaError(16);
  printf("AFTER DK16\n");
  cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);

  bool flag3 = true; // asst in .cu
  int cnt = 0;
  do{
    flag3 = false;
    cnt++;
    cudaMemcpyToSymbol(::source, &source, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::sink, &sink, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::flag3, &flag3, sizeof(bool), 0, cudaMemcpyHostToDevice);
    decremental_kernel_18<<<numBlocks,  numThreads>>>(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_residual_capacity,d_excess,d_reverse_edge,d_rev_residual_capacity,d_height2);
    cudaDeviceSynchronize();
    checkCudaError(18);
    printf("AFTER DK18\n");
    print_excess<<<numBlocks,numThreads>>>(V,d_excess);
    print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&source, ::source, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&sink, ::sink, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&flag3, ::flag3, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  }while(flag3);

}



int recalculate_max_flow(graph& g, std::vector<update> &updateBatch,  int source, int sink,int batchSize)
{
  int V = g.num_nodes();// ADDED
  int E = g.num_edges();//ADDED

 

  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_reverse_edge;
  cudaMalloc(&d_reverse_edge, sizeof(int)*(E));
  int* d_excess;
  cudaMalloc(&d_excess, sizeof(int)*(V));
  
  cudaMemset(d_excess,0,sizeof(int)*V);
  cudaMemset(d_residual_capacity,0,sizeof(int)*E);
  cudaMemset(d_rev_residual_capacity,0,sizeof(int)*E);

  int *residual_capacity,*rev_residual_capacity,*reverse_edge,*excess;//ADDED

  residual_capacity = (int *)malloc(sizeof(int)*E);//ADDED
  rev_residual_capacity = (int *)malloc(sizeof(int)*E);//ADDED
  reverse_edge = (int *)malloc(sizeof(int)*E);//ADDED
  excess = (int *) malloc(sizeof(int)*E);//ADDED
  cudaMemcpy(residual_capacity,d_residual_capacity,sizeof(int)*E,cudaMemcpyDeviceToHost);//ADDED
  cudaMemcpy(rev_residual_capacity,d_rev_residual_capacity,sizeof(int)*E,cudaMemcpyDeviceToHost);//ADDED
  cudaMemcpy(reverse_edge,d_reverse_edge,sizeof(int)*E,cudaMemcpyDeviceToHost);//ADDED
  cudaMemcpy(excess,d_excess,sizeof(int)*V,cudaMemcpyDeviceToHost);//ADDED

  staticMaxFlow(g,source,sink,residual_capacity,rev_residual_capacity,reverse_edge,excess);

  cudaMemcpy(d_residual_capacity,residual_capacity,sizeof(int)*E,cudaMemcpyHostToDevice);//ADDED
  cudaMemcpy(d_rev_residual_capacity,rev_residual_capacity,sizeof(int)*E,cudaMemcpyHostToDevice);//ADDED
  cudaMemcpy(d_reverse_edge,reverse_edge,sizeof(int)*E,cudaMemcpyHostToDevice);//ADDED
  cudaMemcpy(d_excess,excess,sizeof(int)*V,cudaMemcpyHostToDevice);//ADDED
  printf("OUT OF STATIC\n");

  unsigned numThreads   = (V <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: V;
  unsigned numBlocks    = (V+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;


  int batchElements = 0;


  update *d_batch_updates;//ADDED
  cudaMalloc(&d_batch_updates,sizeof(update)*batchSize);  //ADDED

  // I WANT THE GRAPH ATTRIBUTES PROCESSED BEFORE LIKE THE USED BEFORE COPIED BEFORE
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
  int *d_height;
  cudaMalloc(&d_height,sizeof(int)*V);
  printf("AFTER ALL BATCH MALLOCS BATCH SIZE:%d\n",batchSize);
  print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
  // TIMER START
  cudaEvent_t start2, stop2;
  cudaEventCreate(&start2);
  cudaEventCreate(&stop2);
  float milliseconds2 = 0;
  cudaEventRecord(start2,0);
  printf("update batch size:%d\n",updateBatch.size());
  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += batchSize){
    printf("UPDATE INDEX:%d\n",updateIndex);
    if((updateIndex + batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else batchElements = batchSize ;
    cudaMemcpy(d_batch_updates,&updateBatch[updateIndex],batchElements*sizeof(update),cudaMemcpyHostToDevice);
    
    unsigned numThreadsupdate   = (batchElements <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: batchElements;
    unsigned numBlocksupdate    = (batchElements+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;

    // HERE WRITING AS A SINGLE KERNEL --> CAN HAVE TWO KERNELS FOR ADD AND DELETE
    update_edges_kernel<<<numBlocksupdate,numThreadsupdate>>>(batchElements,d_batch_updates,d_residual_capacity,d_rev_residual_capacity,d_weight,d_meta,d_data);
    cudaDeviceSynchronize();
    printf("AFTER UPDATE EDGES KERNEL\n");
    unsigned numThreadexcess   = (V <  THREADS_PER_BLOCK)? THREADS_PER_BLOCK: V;
    unsigned numBlocksexcess    = (V+ THREADS_PER_BLOCK-1)/ THREADS_PER_BLOCK;
    update_excess<<<numBlocksexcess,numThreadexcess>>>(V,d_residual_capacity,d_rev_residual_capacity,d_meta,d_excess,d_reverse_edge,d_rev_meta);
    cudaDeviceSynchronize();
    printf("AFTER UPDATE EXCESS KERNEL\n");
  print_excess<<<numBlocks,numThreads>>>(V,d_excess);
  print_residual<<<numBlocks,numThreads>>>(V,d_meta,d_residual_capacity,d_rev_residual_capacity,d_data);
  // break;
    Decremental(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_height);
    printf("AFTER DECREMENTAL\n");
    // break;
    Incremental(V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,source,sink,d_residual_capacity,d_rev_residual_capacity,d_reverse_edge,d_excess,d_height);
    printf("AFTER INCREMENTAL\n");
  }
  cudaEventRecord(stop2,0);
  cudaEventSynchronize(stop2);
  cudaEventElapsedTime(&milliseconds2, start2, stop2);
  printf("Batchwise processing GPU Time: %.6f ms\n", milliseconds2);

  int *h_excess;
  h_excess = (int *)malloc(sizeof(int)*V);
  cudaMemcpy(h_excess,d_excess,sizeof(int)*V,cudaMemcpyDeviceToHost);
  int val = h_excess[V-1];
  free(h_excess);

  cudaFree(d_meta);
  cudaFree(d_data);
  cudaFree(d_src);
  cudaFree(d_weight);
  cudaFree(d_rev_meta);


  cudaFree(d_residual_capacity);
  cudaFree(d_rev_residual_capacity);
  cudaFree(d_reverse_edge);
  cudaFree(d_excess);
  return val;
}

int  main( int  argc, char** argv) {
  char* totalgraph=argv[1];
  char* updatesinp = argv[2];
  int source = atoi(argv[3]);
  int sink = atoi(argv[4]);
  int batchsize = atoi(argv[5]);
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  std::vector<update> updateEdges=G1.parseUpdates(updatesinp);
  int reval = recalculate_max_flow(G1,updateEdges,source,sink,batchsize);
  cudaDeviceSynchronize();
  printf("REVAL FLOW VALUE:%d\n",reval);
  return 0;
}
