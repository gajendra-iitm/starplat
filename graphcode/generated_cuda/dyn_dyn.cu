// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "dyn_dyn.h"
#define THREADS_PER_BLOCK 1024
void recalculate_max_flow(graph& g, std::vector<update> updateBatch, int batchSize, int source, 
  int sink)
{
  int E = g.num_edges();
  int *d_meta, *d_data,*d_weight;
  int* d_rev_residual_capacity;
  cudaMalloc(&d_rev_residual_capacity, sizeof(int)*(E));

  int* d_residual_capacity;
  cudaMalloc(&d_residual_capacity, sizeof(int)*(E));

  int _batchSize = batchSize;
  update *d_updateBatch;
  cudaMalloc(&d_updateBatch,sizeof(update)*_batchSize);
  int batchElements = 0;
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

    OnAdd_kernel<<<updateBlocks,updateThreads>>>(d_updateBatch,batchElements, d_meta, d_data, d_weight, d_residual_capacity, d_rev_residual_capacity);


  }


}
