// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "bc_dslV2.h"

#define CUDA_CHECK(err) { \
  if (err != cudaSuccess) { \
      fprintf(stderr, "CUDA error at %s: line:%d: Error number: %d. Error name: %s. Error description: %s.\n", __FILE__, __LINE__, (int)err, cudaGetErrorName(err), cudaGetErrorString(err)); \
      exit(err); \
  } \
}

// #define CUDA_CHECK(err) { \
//   if (err != cudaSuccess) { \
//       fprintf(stderr, "CUDA error at %d: %s. Error number: %d. Error name: %s. Error description: %s.\n", __FILE__, __LINE__, cudaGetErrorString(err), (int)err, cudaGetErrorName(err)); \
//       exit(err); \
//   } \
// }

//cudaError_t err = cudaGetLastError();

void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet)

{
  cudaError_t err = cudaGetLastError();
  CUDA_CHECK(err);
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_weight;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_weight;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);

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
  float* d_BC;
  cudaMalloc(&d_BC, sizeof(float)*(V));
  err = cudaGetLastError();
  CUDA_CHECK(err);

  //BEGIN DSL PARSING 
  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_BC,(float)0);
  err = cudaGetLastError();
  CUDA_CHECK(err);
  //MILESTONE 1: reached, initKernel device function throwing CUDA error "named symbol not found"
  //print numblocks here
  float* d_sigma;
  cudaMalloc(&d_sigma, sizeof(float)*(V));

  float* d_delta;
  cudaMalloc(&d_delta, sizeof(float)*(V));

  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    cudaError_t err = cudaGetLastError();
    CUDA_CHECK(err);
    int src = *itr;
    //print numblocks here

    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_delta,(float)0);
    //print numblocks here
    err = cudaGetLastError();
    CUDA_CHECK(err);
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_sigma,(float)0);
    err = cudaGetLastError();
    CUDA_CHECK(err);

    initIndex<float><<<1,1>>>(V,d_sigma,src,(float)1); //InitIndexDevice
    err = cudaGetLastError();
    CUDA_CHECK(err);

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source=0;
    bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
    int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));
    int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

    //EXTRA vars INITIALIZATION
    printf("numBlocks: %d, threadsPerBlock: %d\n");
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_level,-1);
    // Check for errors during kernel launch
    err = cudaGetLastError();
    CUDA_CHECK(err);

    initIndex<int><<<1,1>>>(V,d_level,src, 0);
    // Check for errors during kernel launch
    //err = cudaGetLastError();
    CUDA_CHECK(err);

    // long k =0 ;// For DEBUG
    do {
      finished = true;
      cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,d_BC); ///DONE from varList
      // Check for errors during kernel launch
      //cudaError_t err = cudaGetLastError();
      CUDA_CHECK(err);

      incrementDeviceVar<<<1,1>>>(d_hops_from_source);
      // Check for errors during kernel launch
      CUDA_CHECK(err);
  
            // Synchronize the device to catch errors that occur during kernel execution
      err = cudaDeviceSynchronize();
      CUDA_CHECK(err); //MUST - rupesh

      ++hops_from_source; // updating the level to process in the next iteration
      // k++; //DEBUG

      cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
    }while(!finished);

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);

    //BACKWARD PASS
    while(hops_from_source > 1) {

      //KERNEL Launch
      back_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished
        ,d_BC); ///DONE from varList
      // Check for errors during kernel launch
      //cudaError_t err = cudaGetLastError();
      CUDA_CHECK(err);

      //err = cudaDeviceSynchronize();
      CUDA_CHECK(err);

      hops_from_source--;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
    }
    //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);

  }

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_delta);
  cudaFree(d_sigma);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(      BC,     d_BC, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN
