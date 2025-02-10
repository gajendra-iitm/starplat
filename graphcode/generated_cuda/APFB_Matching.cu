// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "APFB_Matching.h"

void APFB(graph& g,int nc)

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
  unsigned numThreads   = (V > threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  bool* d_modified_next;
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  int* d_rmatch;
  cudaMalloc(&d_rmatch, sizeof(int)*(V));

  int* d_cmatch;
  cudaMalloc(&d_cmatch, sizeof(int)*(V));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_rmatch,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_cmatch,(int)-1);

  bool noNewPaths = false; // asst in .cu

  int* d_bfsArray;
  cudaMalloc(&d_bfsArray, sizeof(int)*(V));

  int* d_predeccesor;
  cudaMalloc(&d_predeccesor, sizeof(int)*(V));

  bool* d_compress;
  cudaMalloc(&d_compress, sizeof(bool)*(V));

  bool* d_compress_next;
  cudaMalloc(&d_compress_next, sizeof(bool)*(V));

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  int k=0; // #fixpt-Iterations
  while(!noNewPaths) {

    noNewPaths = true;
    cudaMemcpyToSymbol(::noNewPaths, &noNewPaths, sizeof(bool), 0, cudaMemcpyHostToDevice);
    int L0 = 0; // asst in .cu

    int NOT_VISITED = L0 - 1; // asst in .cu

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_bfsArray,(int)NOT_VISITED);

    cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(::L0, &L0, sizeof(int), 0, cudaMemcpyHostToDevice);
    APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_cmatch,d_bfsArray);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);
    cudaMemcpyFromSymbol(&L0, ::L0, sizeof(int), 0, cudaMemcpyDeviceToHost);



    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_predeccesor,(int)-1);

    int bfsLevel = L0; // asst in .cu

    bool noNewVertices = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!noNewVertices) {

      noNewVertices = true;
      cudaMemcpyToSymbol(::noNewVertices, &noNewVertices, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::NOT_VISITED, &NOT_VISITED, sizeof(int), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::bfsLevel, &bfsLevel, sizeof(int), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::noNewPaths, &noNewPaths, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::noNewVertices, &noNewVertices, sizeof(bool), 0, cudaMemcpyHostToDevice);
      APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_bfsArray,d_rmatch,d_predeccesor);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&NOT_VISITED, ::NOT_VISITED, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&bfsLevel, ::bfsLevel, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&noNewPaths, ::noNewPaths, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&noNewVertices, ::noNewVertices, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      bfsLevel = bfsLevel + 1;

      cudaMemcpyFromSymbol(&noNewVertices, ::noNewVertices, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_compress,(bool)false);

    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_compress_next,(bool)false);

    cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
    APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_rmatch,d_compress);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);



    bool compressed = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!compressed) {

      compressed = true;
      cudaMemcpyToSymbol(::compressed, &compressed, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::compressed, &compressed, sizeof(bool), 0, cudaMemcpyHostToDevice);
      APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_compress,d_predeccesor,d_cmatch,d_compress_next,d_rmatch);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);
      cudaMemcpyFromSymbol(&compressed, ::compressed, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
      APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_compress_next,d_compress);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);




      cudaMemcpyFromSymbol(&compressed, ::compressed, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    cudaMemcpyToSymbol(::nc, &nc, sizeof(int), 0, cudaMemcpyHostToDevice);
    APFB_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_rmatch,d_cmatch);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&nc, ::nc, sizeof(int), 0, cudaMemcpyDeviceToHost);



    ; // asst in .cu


    cudaMemcpyFromSymbol(&noNewPaths, ::noNewPaths, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT


  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_compress_next);
  cudaFree(d_compress);
  cudaFree(d_rmatch);
  cudaFree(d_cmatch);
  cudaFree(d_modified);
  cudaFree(d_bfsArray);
  cudaFree(d_predeccesor);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
