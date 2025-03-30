// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_GENERATED_DYN_H
#define GENCPP_GENERATED_DYN_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graph.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>

void staticMaxFlow(graph& g,int source2,int sink2,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess);

__device__ int source2 ;
__device__ int sink2 ;


__global__ void staticMaxFlow_kernel_1(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,int* d_reverse_edge,int* d_residual_capacity,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e1 = edge;
    d_residual_capacity[e1] = d_weight[e1];
    d_rev_residual_capacity[e1] = 0;

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

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void staticMaxFlow_kernel_5(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_excess,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source2){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      atomicAdd(& d_excess[vv] , d);
      atomicSub(& d_excess[v] , d);
      atomicSub(& d_residual_capacity[forward_edge] , d);
      atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag1 ; // DEVICE ASSTMENT in .h

__global__ void staticMaxFlow_kernel_7(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_excess,int* d_reverse_edge,int* d_residual_capacity,int* d_height,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source2 && v != sink2){ // if filter begin 
    flag1 = true;
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
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int rev = edge;
      int e = d_reverse_edge[rev];
      int xxx = d_rev_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height[vv];
        backward_edge = e;
        forward_edge = -1;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_height[v] > hh && v_0 != -1){ // if filter begin 
      if (forward_edge != -1){ // if filter begin 
        int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

        int d = fec; // DEVICE ASSTMENT in .h

        if (d_excess[v] < fec){ // if filter begin 
          d = d_excess[v];

        } // if filter end
        atomicSub(& d_excess[v] , d);
        atomicAdd(& d_excess[v_0] , d);
        atomicSub(& d_residual_capacity[forward_edge] , d);
        atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

      } // if filter end
      else {
        int fec = d_rev_residual_capacity[backward_edge]; // DEVICE ASSTMENT in .h

        int d = fec; // DEVICE ASSTMENT in .h

        if (d_excess[v] < fec){ // if filter begin 
          d = d_excess[v];

        } // if filter end
        atomicSub(& d_excess[v] , d);
        atomicAdd(& d_excess[v_0] , d);
        atomicSub(& d_rev_residual_capacity[backward_edge] , d);
        atomicAdd(& d_residual_capacity[backward_edge] , d);

      }

    } // if filter end
    else {
      if (v_0 != -1){ // if filter begin 
        d_height[v] = hh + 1;

      } // if filter end
    }

  } // if filter end
} // end KER FUNC
__device__ int source0 ;
__device__ int sink0 ;
__global__ void incremental_kernel_10(int V, int E, int* d_meta, int* d_data,int* d_residual_capacity,int* d_excess,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source0){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      atomicAdd(& d_excess[vv] , d);
      atomicSub(& d_excess[v] , d);
      atomicSub(& d_residual_capacity[forward_edge] , d);
      atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag2 ; // DEVICE ASSTMENT in .h

__global__ void incremental_kernel_12(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_reverse_edge,int* d_rev_residual_capacity,int* d_residual_capacity,int* d_excess,int* d_height){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source0 && v != sink0){ // if filter begin 
    flag2 = true;
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
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int rev = edge;
      int e = d_reverse_edge[rev];
      int xxx = d_rev_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height[vv];
        backward_edge = e;
        forward_edge = -1;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_height[v] > hh && v_0 != -1){ // if filter begin 
      if (forward_edge != -1){ // if filter begin 
        int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

        int d = fec; // DEVICE ASSTMENT in .h

        if (d_excess[v] < fec){ // if filter begin 
          d = d_excess[v];

        } // if filter end
        atomicSub(& d_excess[v] , d);
        atomicAdd(& d_excess[v_0] , d);
        atomicSub(& d_residual_capacity[forward_edge] , d);
        atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

      } // if filter end
      else {
        int fec = d_rev_residual_capacity[backward_edge]; // DEVICE ASSTMENT in .h

        int d = fec; // DEVICE ASSTMENT in .h

        if (d_excess[v] < fec){ // if filter begin 
          d = d_excess[v];

        } // if filter end
        atomicSub(& d_excess[v] , d);
        atomicAdd(& d_excess[v_0] , d);
        atomicSub(& d_rev_residual_capacity[backward_edge] , d);
        atomicAdd(& d_residual_capacity[backward_edge] , d);

      }

    } // if filter end
    else {
      if (v_0 != -1){ // if filter begin 
        d_height[v] = hh + 1;

      } // if filter end
    }

  } // if filter end
} // end KER FUNC
__device__ int source1 ;
__device__ int sink1 ;
__global__ void decremental_kernel_15(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_reverse_edge,int* d_rev_residual_capacity,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int e1 = 0; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e = edge;
    e1 = e1 - d_rev_residual_capacity[e];

  } //  end FOR NBR ITR. TMP FIX!
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int src = d_src[edge] ;
    int rev = edge; // DEVICE ASSTMENT in .h

    int e = d_reverse_edge[rev]; // DEVICE ASSTMENT in .h

    e1 = e1 + d_rev_residual_capacity[e];

  } //  end FOR NBR ITR. TMP FIX!
  d_excess[v] = e1; //InitIndex
} // end KER FUNC
__global__ void decremental_kernel_18(int V, int E, int* d_src, int *d_rev_meta,int* d_reverse_edge,int* d_residual_capacity,int* d_excess,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == sink1){ // if filter begin 
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int backward_edge = edge;
      int forward_edge = d_reverse_edge[backward_edge];
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      atomicAdd(& d_excess[v] , d);
      atomicSub(& d_excess[vv] , d);
      atomicSub(& d_residual_capacity[forward_edge] , d);
      atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag3 ; // DEVICE ASSTMENT in .h

__global__ void decremental_kernel_20(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_excess,int* d_reverse_edge,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_height2){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] < 0 && v != source1 && v != sink1){ // if filter begin 
    flag3 = true;
    int hh = INT_MAX; // DEVICE ASSTMENT in .h

    int v_0 = -1; // DEVICE ASSTMENT in .h

    int inward_edge = -1; // DEVICE ASSTMENT in .h

    int outward_edge = -1; // DEVICE ASSTMENT in .h

    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int e = edge;
      int xxx = d_rev_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height2[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height2[vv];
        outward_edge = e;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int rev = edge;
      int e = d_reverse_edge[rev];
      int xxx = d_residual_capacity[e]; // DEVICE ASSTMENT in .h

      if (d_height2[vv] < hh && xxx > 0){ // if filter begin 
        v_0 = vv;
        hh = d_height2[vv];
        inward_edge = e;
        outward_edge = -1;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_height2[v] > hh && v_0 != -1){ // if filter begin 
      if (outward_edge != -1){ // if filter begin 
        int d = d_rev_residual_capacity[outward_edge]; // DEVICE ASSTMENT in .h

        int yy = d_excess[v] * -1; // DEVICE ASSTMENT in .h

        if (yy < d){ // if filter begin 
          d = yy;

        } // if filter end
        atomicAdd(& d_excess[v] , d);
        atomicSub(& d_excess[v_0] , d);
        atomicSub(& d_rev_residual_capacity[outward_edge] , d);
        atomicAdd(& d_residual_capacity[outward_edge] , d);

      } // if filter end
      else {
        int d = d_residual_capacity[inward_edge]; // DEVICE ASSTMENT in .h

        int yy = d_excess[v] * -1; // DEVICE ASSTMENT in .h

        if (yy < d){ // if filter begin 
          d = yy;

        } // if filter end
        atomicAdd(& d_excess[v] , d);
        atomicSub(& d_excess[v_0] , d);
        atomicSub(& d_residual_capacity[inward_edge] , d);
        atomicAdd(& d_rev_residual_capacity[inward_edge] , d);

      }

    } // if filter end
    else {
      if (v_0 != -1){ // if filter begin 
        d_height2[v] = hh + 1;

      } // if filter end
    }

  } // if filter end
} // end KER FUNC
__global__ void OnAdd_kernel(update* d_updateBatch, int batchelements, int* d_meta, int* d_data, int* d_weight,int* d_rev_residual_capacity,int* d_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
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