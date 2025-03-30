// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_LOCK-FREE-MAXFLOW-V2_H
#define GENCPP_LOCK-FREE-MAXFLOW-V2_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include <cooperative_groups.h>
#include "graphstatic.hpp"
#include "libcuda.cuh"

int do_max_flow(graph& g,int source,int sink);

__device__ int source ;
__device__ int sink ;


__global__ void do_max_flow_kernel_1(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_reverse_edge, int * d_weight){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e1 = edge;
    d_residual_capacity[e1] = d_weight[e1];
    for (int edge = d_meta[dst]; edge < d_meta[dst+1]; edge++) { // FOR NBR ITR // BUG need neighbours of dst not v
      int vv = d_data[edge];
      if (vv == v){ // if filter begin 
        int rev = edge;
        d_reverse_edge[e1] = rev;
      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void do_max_flow_kernel_2(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_reverse_edge,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      int backward_edge = d_reverse_edge[forward_edge]; // DEVICE ASSTMENT in .h


      // BUG NEED ATOMICS
      // d_excess[vv] = d_excess[vv]+ d;
      atomicAdd(&d_excess[vv],d);
      // d_excess[v] = d_excess[v]- d;
      atomicSub(&d_excess[v],d);
      // d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
      atomicSub(&d_residual_capacity[forward_edge],d);
      // d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;
      atomicAdd(&d_residual_capacity[backward_edge],d);
    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag ; // DEVICE ASSTMENT in .h

__global__ void do_max_flow_kernel_3(int V, int E, int* d_meta, int* d_data,int* d_excess,int* d_reverse_edge,int* d_residual_capacity,int* d_height){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source && v != sink){ // if filter begin 
    flag = true;
    int hh = INT_MAX; // DEVICE ASSTMENT in .h

    int v_0 = -1; // DEVICE ASSTMENT in .h

    int forward_edge = -1; // DEVICE ASSTMENT in .h

    int backward_edge = -1; // DEVICE ASSTMENT in .h

    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int e = edge;
      int xxx = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height[vv];
        forward_edge = e;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_height[v] > hh && v_0 != -1){ // if filter begin 
      int backward_edge = d_reverse_edge[forward_edge]; // DEVICE ASSTMENT in .h

      int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      int d = fec; // DEVICE ASSTMENT in .h

      if (d_excess[v] < fec){ // if filter begin 
        d = d_excess[v];

      } // if filter end
      // d_excess[v] = d_excess[v]- d;
      atomicSub(&d_excess[v],d);
      // d_excess[v_0] = d_excess[v_0]+ d;
      atomicAdd(&d_excess[v_0],d);
      // d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
      atomicSub(&d_residual_capacity[forward_edge],d);
      // d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;
      atomicAdd(&d_residual_capacity[backward_edge],d);

    } // if filter end
    else
    if (v_0 != -1){ // if filter begin 
      d_height[v] = hh + 1;

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
