// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "maxflow.h"

void maxflow(graph& g,int s,int t)

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
  int* d_cf;
  cudaMalloc(&d_cf, sizeof(int)*(E));

  int* d_h;
  cudaMalloc(&d_h, sizeof(int)*(V));

  int* d_e;
  cudaMalloc(&d_e, sizeof(int)*(V));

  int ExcessTotal = 0; // asst in .cu

  int N = g.num_nodes(); // asst in .cu

  cudaMemcpyToSymbol(::s, &s, sizeof(int), 0, cudaMemcpyHostToDevice);
  cudaMemcpyToSymbol(::N, &N, sizeof(int), 0, cudaMemcpyHostToDevice);
  maxflow_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_cf,d_e,d_h);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&s, ::s, sizeof(int), 0, cudaMemcpyDeviceToHost);
  cudaMemcpyFromSymbol(&N, ::N, sizeof(int), 0, cudaMemcpyDeviceToHost);



  int v = 0; // asst in .cu

  do{
    if (v == s){ // if filter begin 
      d_h[v] = N;
      d_e[v] = 0;

    } // if filter end
    else
    d_h[v] = 0;
    d_e[v] = 0;

    int e1 = edge;
    d_cf[e1] = weight[e1];
    if (cf[e1] == -1){ // if filter begin 
      d_cf_backward[e1] = 0;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  v = v + 1;

}while(v < N);
bool finished = false; // asst in .cu

do{
  cudaMemcpyToSymbol(::v, &v, sizeof(int), 0, cudaMemcpyHostToDevice);
  maxflow_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_h,d_e);
  cudaDeviceSynchronize();
  cudaMemcpyFromSymbol(&v, ::v, sizeof(int), 0, cudaMemcpyDeviceToHost);



  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu

  ; // asst in .cu


}while(finished);

//cudaFree up!! all propVars in this BLOCK!
cudaFree(d_e);
cudaFree(d_h);

//TIMER STOP
cudaEventRecord(stop,0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&milliseconds, start, stop);
printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
