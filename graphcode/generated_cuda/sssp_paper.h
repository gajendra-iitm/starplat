// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_SSSP_PAPER_H
#define GENCPP_SSSP_PAPER_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../dynamic_mst_delete_cuda/libcuda.cuh"
#include <cooperative_groups.h>

void computeSSSP(graph& g,int src);



__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void computeSSSP_kernel_1(int V, int E, int* d_meta, int* d_data,bool* d_modified,bool* d_modified_next){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e = edge;

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC

#endif
