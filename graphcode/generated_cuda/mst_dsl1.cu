// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "mst_dsl1.h"

void Boruvka(graph& g)

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
  int* d_nodeId;
  cudaMalloc(&d_nodeId, sizeof(int)*(V));

  int* d_color;
  cudaMalloc(&d_color, sizeof(int)*(V));

  bool* d_isMSTEdge;
  cudaMalloc(&d_isMSTEdge, sizeof(bool)*(E));

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_nodeId,(int)-1);

  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_color,(int)-1);

  Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color,d_nodeId);
  cudaDeviceSynchronize();



  bool* d_modified;
  cudaMalloc(&d_modified, sizeof(bool)*(V));

  bool* d_modified_next;
  cudaMalloc(&d_modified_next, sizeof(bool)*(V));

  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V,d_modified,(bool)false);

  int* d_minEdgeOfComp;
  cudaMalloc(&d_minEdgeOfComp, sizeof(int)*(V));

  int* d_minEdge;
  cudaMalloc(&d_minEdge, sizeof(int)*(V));

  bool noNewComp = false; // asst in .cu

  // FIXED POINT variables
  //BEGIN FIXED POINT
  initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
  int k=0; // #fixpt-Iterations
  while(!noNewComp) {

    noNewComp = true;
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);

    Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_meta,d_data,d_color,d_minEdge);
    cudaDeviceSynchronize();



    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);

    bool finishedMinEdge = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finishedMinEdge) {

      finishedMinEdge = true;
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finishedMinEdge, &finishedMinEdge, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color,d_minEdge,d_minEdgeOfComp);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu

      ; // asst in .cu


      cudaMemcpyFromSymbol(&finishedMinEdge, ::finishedMinEdge, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT

    Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color,d_nodeId,d_minEdgeOfComp);
    cudaDeviceSynchronize();



    Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color,d_nodeId,d_minEdgeOfComp,d_isMSTEdge);
    cudaDeviceSynchronize();



    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color,d_nodeId,d_minEdgeOfComp);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);



    bool finished = false; // asst in .cu

    // FIXED POINT variables
    //BEGIN FIXED POINT
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    int k=0; // #fixpt-Iterations
    while(!finished) {

      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      Boruvka_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_color);
      cudaDeviceSynchronize();
      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);



      ; // asst in .cu

      ; // asst in .cu


      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
      initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
      k++;
    } // END FIXED POINT


    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaMemcpy(d_modified, d_modified_next, sizeof(bool)*V, cudaMemcpyDeviceToDevice);
    initKernel<bool> <<<numBlocks,threadsPerBlock>>>(V, d_modified_next, false);
    k++;
  } // END FIXED POINT


  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_minEdge);
  cudaFree(d_minEdgeOfComp);
  cudaFree(d_modified);
  cudaFree(d_nodeId);
  cudaFree(d_color);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
