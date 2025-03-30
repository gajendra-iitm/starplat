// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_DYN_DYN_H
#define GENCPP_DYN_DYN_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

__global__ void OnAdd_kernel(update* d_updateBatch, int batchelements, int* d_meta, int* d_data, int* d_weight,int* d_residual_capacity,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
  if(tid >= batchelements) return;
  update u = d_updateBatch[tid];
  if( u.type!='a') return;
  int uedge = -1; // DEVICE ASSTMENT in .h

  int src = u.source; // DEVICE ASSTMENT in .h

  int dest = u.destination; // DEVICE ASSTMENT in .h

  int new_capacity = u.weight; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[src]; edge < d_meta[src+1]; edge++) { // FOR NBR ITR 
    int dd = d_data[edge];
    if (dd == dest){ // if filter begin 
      uedge = edge;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  if (new_capacity < d_weight[uedge]){ // if filter begin 
    if (d_rev_residual_capacity[uedge] > new_capacity){ // if filter begin 
      d_rev_residual_capacity[uedge] = new_capacity;

    } // if filter end
    d_residual_capacity[uedge] = new_capacity - d_rev_residual_capacity[uedge];

  } // if filter end
  else {
    d_residual_capacity[uedge] = d_residual_capacity[uedge] + new_capacity - d_weight[uedge];

  }
  d_weight[uedge] = new_capacity;
} // end KER FUNC
#endif