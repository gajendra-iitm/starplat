// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_DYN4_UD_TEST_H
#define GENCPP_DYN4_UD_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void staticMaxFlow(graph& g,int source2,int sink2,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_parallel_edge,
  int kernel_cycle);

__device__ int source2 ;
__device__ int sink2 ;
__device__ int kernel_cycle ;


__device__ int V ; // DEVICE ASSTMENT in .h

__global__ void staticMaxFlow_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,int* d_reverse_edge,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_parallel_edge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e1 = edge;
    d_residual_capacity[e1] = d_weight[e1];
    d_rev_residual_capacity[e1] = 0;
    d_parallel_edge[e1] = -1;
    for (int edge = d_meta[dst]; edge < d_meta[dst+1]; edge++) { // FOR NBR ITR 
      int pnbr = d_data[edge];
      int p = edge;
      if (pnbr == v){ // if filter begin 
        d_parallel_edge[e1] = p;
        d_rev_residual_capacity[e1] = d_weight[p];

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } //  end FOR NBR ITR. TMP FIX!
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int src = d_src[edge] ;
    int rev = edge;
    for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int e = edge;
      if (vv == v){ // if filter begin 
        d_reverse_edge[rev] = e;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int dst = d_data[edge];
      int fe = edge;
      if (dst == src){ // if filter begin 
        d_reverse_edge[rev] = -1;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void staticMaxFlow_kernel_7(int V, int E, int* d_meta, int* d_data){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source2){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
