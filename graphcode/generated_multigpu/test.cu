// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "test.h"

void bp(graph& g,int k)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

   FILE* fptr = fopen("num_devices.txt","r"); 
  int devicecount;
   if(fptr == NULL){ 
     cudaGetDeviceCount(&devicecount); 
     } 
   else{ 
     fscanf(fptr," %d ",&devicecount); 
     fclose(fptr); 
  }
  int* h_vertex_partition;
  int *h_edges;//data
  int *h_weight;
  int *h_offset;//meta
  int* h_src;
  int *h_rev_meta;
  int h_vertex_per_device;

  h_edges = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_offset = (int *)malloc( (V+1)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_vertex_partition = (int*)malloc((devicecount+1)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));
  h_vertex_per_device = V/devicecount;

  for(int i=0; i<= V; i++) {
    h_offset[i] = g.indexofNodes[i];
    h_rev_meta[i] = g.rev_indexofNodes[i];
  }

  int index = 0;
  h_vertex_partition[0]=0;
  h_vertex_partition[devicecount]=V;
  for(int i=1;i<devicecount;i++){
    if(i<=(V%devicecount)){
       index+=(h_vertex_per_device+1);
    }
    else{
       index+=h_vertex_per_device;
    }
    h_vertex_partition[i]=index;
  }
  for(int i=0; i< E; i++){
    h_edges[i]= g.edgeList[i];
    h_src[i]=g.srcList[i];
    h_weight[i] = edgeLen[i];
  }


  int** d_offset;
  int** d_edges;
  int** d_weight;
  int** d_src;
  int** d_rev_meta;
  d_offset = (int**) malloc(devicecount*sizeof(int*));
  d_edges = (int**) malloc(devicecount*sizeof(int*));
  d_weight = (int**) malloc(devicecount*sizeof(int*));
  d_src = (int**) malloc(devicecount*sizeof(int*));
  d_rev_meta = (int**) malloc(devicecount*sizeof(int*));

  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaMalloc(&d_offset[i], (V+1)*sizeof(int) );
    cudaMalloc(&d_edges[i], (E)*sizeof(int) );
    cudaMalloc(&d_weight[i], (E)*sizeof(int) );
    cudaMalloc(&d_src[i], (E)*sizeof(int) );
    cudaMalloc(&d_rev_meta[i], (V+1)*sizeof(int) );
  }
  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaMemcpyAsync(d_offset[i], h_offset, (V+1)*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpyAsync(d_edges[i], h_edges, (E)*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpyAsync(d_weight[i], h_weight, (E)*sizeof(int),cudaMemcpyHostToDevice );
    cudaMemcpyAsync(d_src[i], h_src, (E)*sizeof(int),cudaMemcpyHostToDevice );
    cudaMemcpyAsync(d_rev_meta[i], h_rev_meta, (V+1)*sizeof(int),cudaMemcpyHostToDevice );
  }
  for(int i=0;i<devicecount;i++)	
  {
    cudaSetDevice(i);
    cudaDeviceSynchronize();
  }

  const unsigned threadsPerBlock = 1024;
  unsigned numThreads   = (V < threadsPerBlock)? V: 1024;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
  unsigned numBlocksKernel    = (V+threadsPerBlock-1)/threadsPerBlock;
  unsigned numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLARE DEVICE AND HOST vars in params
  int** d_k;
  d_k = (int**)malloc(sizeof(int*)*devicecount);
  for (int i = 0; i < devicecount; i++) {
    cudaSetDevice(i);
    cudaMalloc(&d_k[i], sizeof(int));
    initKernel<int> <<<1,1>>>(1,d_k[i],k);
  }

  int** h_k = (int**)malloc(sizeof(int*)*(devicecount+1));
  for(int i=0;i<=devicecount;i++){
    h_k[i] = (int*)malloc(sizeof(int));
  }

  //BEGIN DSL PARSING 
  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
