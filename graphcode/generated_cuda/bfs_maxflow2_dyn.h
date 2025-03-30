// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_BFS_MAXFLOW2_H
#define GENCPP_BFS_MAXFLOW2_H
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


__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_height,int* d_reverse_edge) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if(d_level[v] == *d_hops_from_source) {
    if (!d_scanned[v]){ // if filter begin 
      d_level[v] = -1;

    } // if filter end
    else {
      int curheight = d_height[v] + 1; // DEVICE ASSTMENT in .h

      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int w = d_data[edge];
        int e = edge; // DEVICE ASSTMENT in .h

        if (d_rev_residual_capacity[e] > 0){ // if filter begin 
          d_scanned[w] = true;
          d_height[w] = curheight;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
      {
        int w = d_src[edge] ;
        int rev = edge; // DEVICE ASSTMENT in .h

        int e = d_reverse_edge[rev]; // DEVICE ASSTMENT in .h

        if (e != -1){ // if filter begin 
          if (d_residual_capacity[e] > 0){ // if filter begin 
            d_scanned[w] = true;
            d_height[w] = curheight;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    }
  } // end if d lvl
} // kernel end

__global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_height,int* d_reverse_edge) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  auto grid = cooperative_groups::this_grid();
  if(d_level[v] == *d_hops_from_source-1) {
  } // end if d lvl
} // kernel end

