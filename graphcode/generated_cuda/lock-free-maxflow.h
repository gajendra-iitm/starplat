// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_LOCK-FREE-MAXFLOW_H
#define GENCPP_LOCK-FREE-MAXFLOW_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void do_max_flow(graph& g,int source,int sink,int kernel_parameter
);

__device__ int source ;
__device__ int sink ;
__device__ int kernel_parameter ;


__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    d_residual_capacity[e] = d_weight[e];

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_excess,int* d_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int forward_edge = edge;
  int backward_edge = edge;
  int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

  d_excess[v] = d_excess[v]+ d;
  d_excess[source] = d_excess[source]- d;
  d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
  d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;
} // end KER FUNC
__device__ bool flag ; // DEVICE ASSTMENT in .h

__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_excess,int* d_residual_capacity,int* d_height){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source && v != sink){ // if filter begin 
    flag = true;
    int hh = INT_MAX; // DEVICE ASSTMENT in .h

    int v_0 = -1; // DEVICE ASSTMENT in .h

    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int e = edge;
      int xxx = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height[vv];

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_height[v] > hh && v_0 != -1){ // if filter begin 
      int forward_edge = edge;
      int backward_edge = edge;
      int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      int d = fec; // DEVICE ASSTMENT in .h

      if (d_excess[v] < fec){ // if filter begin 
        d = d_excess[v];

      } // if filter end
      d_excess[v] = d_excess[v]- d;
      d_excess[v_0] = d_excess[v_0]+ d;
      d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
      d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;

    } // if filter end
    else
    if (lv != -1){ // if filter begin 
      d_height[v] = hh + 1;

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
