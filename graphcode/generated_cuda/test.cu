// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "test.h"

void yjhjnj(graph& g,int k)

{
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

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);

  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V > threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  thrust::device_vector<char>  s = "Hello";
  thrust::device_vector<char>  s1;
  s1 = "World";
  message_passs("STRRR",1);


  yjhjnj_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data);
  cudaDeviceSynchronize();




  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
