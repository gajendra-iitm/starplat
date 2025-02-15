// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MAXFLOW_H
#define GENCPP_MAXFLOW_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void maxflow(graph& g,int s,int t);

__device__ int s ;
__device__ int t ;


__device__ int ExcessTotal ; // DEVICE ASSTMENT in .h

__device__ int N ; // DEVICE ASSTMENT in .h

__global__ void maxflow_kernel(int V, int E, int* d_meta, int* d_data,int* d_cf,int* d_e,int* d_h){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == s){ // if filter begin 
    d_h[v] = N;
    d_e[v] = 0;

  } // if filter end
  else
  d_h[v] = 0;
  d_e[v] = 0;

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e1 = edge;
    d_cf[e1] = d_weight[e1];
    if (d_cf[e1] == -1){ // if filter begin 
      d_cf_backward[e1] = 0;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__device__ int v ; // DEVICE ASSTMENT in .h

for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int nbr = d_data[edge];
  __device__ bool finished ; // DEVICE ASSTMENT in .h

  __global__ void maxflow_kernel(int V, int E, int* d_meta, int* d_data,int* d_h,int* d_e){ // BEGIN KER FUN via ADDKERNEL
    float num_nodes  = V;
    unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
    if(v >= V) return;
    int cycle = 100; // DEVICE ASSTMENT in .h

    do{
      int e1 = d_e[v]; // DEVICE ASSTMENT in .h

      int h1 = INT_MAX; // DEVICE ASSTMENT in .h

      int v1 = -1; // DEVICE ASSTMENT in .h

      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int nbr = d_data[edge];
        int h2 = d_h[nbr]; // DEVICE ASSTMENT in .h

        if (h2 < h1){ // if filter begin 
          v1 = nbr;
          h1 = h2;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      if (d_h[v] > h1){ // if filter begin 
        int ee = edge;
        int d = -1; // DEVICE ASSTMENT in .h

        if (e1 < d_cf_forward[ee]){ // if filter begin 
          d = e1;

        } // if filter end
        else
        d = d_cf_forward[ee];

        d_cf_backward[ee] = d_cf_backward[ee]- d;

      } // if filter end

    }while(cycle > 0);
  } // end KER FUNC

  #endif
