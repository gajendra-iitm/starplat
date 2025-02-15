// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_PUSH_RELABEL_CHECK_H
#define GENCPP_PUSH_RELABEL_CHECK_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void relabel(graph& g,int u,int* residue,int* label,
  thrust::host_vector<int> count);



__device__ int x ; // DEVICE ASSTMENT in .h

__device__ int new_label ; // DEVICE ASSTMENT in .h

for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int v = d_data[edge];
  void fixGap(thrust::host_vector<int> count,int* label);



  __device__ int gap ; // DEVICE ASSTMENT in .h

  __global__ void fixGap_kernel(int V, int E,int* d_label){ // BEGIN KER FUN via ADDKERNEL
    float num_nodes  = V;
    unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
    if(v >= V) return;
    if (v != src && v != snk && d_label[v] >= gap){ // if filter begin 
      d_label[v] = g.num_nodes + 2;

    } // if filter end
  } // end KER FUNC

  #endif
