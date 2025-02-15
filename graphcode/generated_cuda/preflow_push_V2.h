// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_PREFLOW_PUSH_V2_H
#define GENCPP_PREFLOW_PUSH_V2_H
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
__device__ int excesstotal ; // DEVICE ASSTMENT in .h

__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int backward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      atomicAdd(& excesstotal, (int)d);
      d_excess[vv] = d_excess[vv]+ d;
      d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
      d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag ; // DEVICE ASSTMENT in .h

__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_excess,int* d_label,int* d_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int x = kernel_parameter; // DEVICE ASSTMENT in .h

  do{
    x = x - 1;
    int hh = 100000000; // DEVICE ASSTMENT in .h

    int lv = -1; // DEVICE ASSTMENT in .h

    if (d_excess[v] > 0 && v != source && v != sink && d_label[v] < V){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int vv = d_data[edge];
        int e = edge;
        int xxx = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

        if (d_label[vv] < hh && xxx > 0){ // if filter begin 
          lv = vv;
          hh = d_label[vv];

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      if (d_label[v] > hh && lv != -1){ // if filter begin 
        int current_edge = edge;
        int cec = d_residual_capacity[current_edge]; // DEVICE ASSTMENT in .h

        if (d_excess[v] > 0 && cec > 0){ // if filter begin 
          int forward_edge = edge;
          int backward_edge = edge;
          int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

          int bec = d_residual_capacity[backward_edge]; // DEVICE ASSTMENT in .h

          int d = fec; // DEVICE ASSTMENT in .h

          if (d_excess[v] < fec){ // if filter begin 
            d = d_excess[v];

          } // if filter end
          d_excess[v] = d_excess[v]- d;
          d_excess[lv] = d_excess[lv]+ d;
          d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
          d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;

        } // if filter end

      } // if filter end
      else
      if (lv != -1){ // if filter begin 
        d_label[v] = hh + 1;

      } // if filter end

    } // if filter end

  }while(x > 0);
} // end KER FUNC
__global__ void do_max_flow_kernel(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_excess,int* d_label){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int vv = d_data[edge];
    int e = edge;
    int ee = edge;
    int ec = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

    if (ec > 0 && d_label[v] > d_label[vv] + 1 && d_excess[v] > 0){ // if filter begin 
      int d = ec; // DEVICE ASSTMENT in .h

      if (d > d_excess[v]){ // if filter begin 
        d = d_excess[v];
      } // if filter end
      d_excess[v] = d_excess[v]- d;
      d_excess[vv] = d_excess[vv]+ d;
      d_residual_capacity[e] = d_residual_capacity[e]- d;
      d_residual_capacity[ee] = d_residual_capacity[ee]+ d;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__device__ int finished ; // DEVICE ASSTMENT in .h

__global__ void do_max_flow_kernel(int V, int E, int* d_src, int *d_rev_meta,int* d_residual_capacity,int* d_label,int* d_visit){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_visit[v] == 1){ // if filter begin 
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int w = d_src[edge] ;
      int e = edge;
      int ec = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_visit[w] == 0 && ec > 0){ // if filter begin 
        d_visit[w] = 1;
        d_label[w] = d_label[v] + 1;
        atomicAdd(& finished, (int)1);

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    d_visit[v] = 2;

  } // if filter end
} // end KER FUNC
__global__ void do_max_flow_kernel(int V, int E,int* d_visit,int* d_excess,int* d_label){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_visit[v] == 0){ // if filter begin 
    d_label[v] = 1000000000;
    int x = d_excess[v]; // DEVICE ASSTMENT in .h

    atomicAdd(& excesstotal, (int)x);
    d_excess[v] = 0;

  } // if filter end
} // end KER FUNC

#endif
