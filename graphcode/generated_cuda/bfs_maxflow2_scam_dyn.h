// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_BFS_MAXFLOW2_SCAM_H
#define GENCPP_BFS_MAXFLOW2_SCAM_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void computeBC(graph& g,int n,int source,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_height,int* d_reverse_edge);

__device__ int n ;
__device__ int source ;


__global__ void computeBC_kernel_1(int V, int E,bool* d_scanned){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_scanned[v] == true){ // if filter begin 

  } // if filter end
} // end KER FUNC
__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_height,int* d_reverse_edge) {
  unsigned v1 = blockIdx.x * blockDim.x + threadIdx.x;
  if(v1 >= n) return;
  if(d_level[v1] == *d_hops_from_source) {
    for (int edge = d_meta[v1]; edge < d_meta[v1+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == -1) {
        d_level[w] = *d_hops_from_source + 1;
        *d_finished = false;
      }
      if(d_level[w] == *d_hops_from_source + 1) {
        int e = edge; // DEVICE ASSTMENT in .h

        if (d_rev_residual_capacity[e] > 0){ // if filter begin 
          d_scanned[w] = true;

        } // if filter end

      }
    }
    for (int edge = d_rev_meta[v1]; edge < d_rev_meta[v1+1]; edge++)
    {
      int w = d_src[edge] ;
      if(d_level[w] == -1) {
        d_level[w] = *d_hops_from_source + 1;
        *d_finished = false;
      }
      if(d_level[w] == *d_hops_from_source + 1) {
        int rev = edge; // DEVICE ASSTMENT in .h

        int e = d_reverse_edge[rev]; // DEVICE ASSTMENT in .h

        if (e != -1){ // if filter begin 
          if (d_residual_capacity[e] > 0){ // if filter begin 

          } // if filter end

        } // if filter end

      }
    }
  } // end if d lvl
} // kernel end

