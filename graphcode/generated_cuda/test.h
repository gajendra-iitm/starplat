// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TEST_H
#define GENCPP_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void yjhjnj(graph& g,int k);

__device__ int k ;


// CSR BEGIN

int* d_meta;
int* d_data;



// CSR END
//LAUNCH CONFIG
// TIMER START
__device__ thrust::device_vector<char>  s ;
__device__ thrust::device_vector<char>  s1 ;
__global__ void yjhjnj_kernel(int V, int E, int* d_meta, int* d_data){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nod = d_data[edge];
    int uy = 9;

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC

#endif
