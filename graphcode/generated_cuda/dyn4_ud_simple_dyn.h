// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_DYN4_UD_SIMPLE_H
#define GENCPP_DYN4_UD_SIMPLE_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void staticMaxFlow(graph& g,int source,int sink,int* d_residual_capacity,
  int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_parallel_edge,
  int kernel_cycles);

__device__ int source ;
__device__ int sink ;
__device__ int kernel_cycles ;


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
__global__ void staticMaxFlow_kernel_7(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_reverse_edge,int* d_residual_capacity,int* d_excess,int* d_rev_residual_capacity,int* d_parallel_edge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      if (d > 0){ // if filter begin 
        atomicAdd(& d_excess[vv] , d);
        atomicSub(& d_excess[v] , d);
        atomicSub(& d_residual_capacity[forward_edge] , d);
        atomicAdd(& d_rev_residual_capacity[forward_edge] , d);
        int p = d_parallel_edge[forward_edge]; // DEVICE ASSTMENT in .h

        if (p != -1){ // if filter begin 
          atomicAdd(& d_residual_capacity[p] , d);
          atomicSub(& d_rev_residual_capacity[p] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int backward_edge = edge;
      int forward_edge = d_reverse_edge[backward_edge]; // DEVICE ASSTMENT in .h

      if (forward_edge != -1){ // if filter begin 
        int d = d_rev_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

        if (d > 0){ // if filter begin 
          atomicAdd(& d_excess[vv] , d);
          atomicSub(& d_excess[v] , d);
          atomicSub(& d_residual_capacity[forward_edge] , d);
          atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag1 ; // DEVICE ASSTMENT in .h

__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_parallel_edge) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if(d_level[v] == *d_hops_from_source) {
    int current = d_height[v] + 1; // DEVICE ASSTMENT in .h

    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == -1) {
        d_level[w] = *d_hops_from_source + 1;
        *d_finished = false;
      }
      if(d_level[w] == *d_hops_from_source + 1) {
        int e = edge;
        if (d_rev_residual_capacity[e] > 0){ // if filter begin 
          d_height[w] = current;

        } // if filter end

      }
    }
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
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
            d_height[w] = current;

          } // if filter end

        } // if filter end

      }
    }
  } // end if d lvl
} // kernel end

__global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,int* d_residual_capacity,int* d_rev_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_parallel_edge) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  auto grid = cooperative_groups::this_grid();
  if(d_level[v] == *d_hops_from_source-1) {
  } // end if d lvl
} // kernel end

__global__ void staticMaxFlow_kernel_12(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_residual_capacity,int* d_height,int* d_rev_residual_capacity,int* d_parallel_edge,int* d_reverse_edge,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source && v != sink && d_height[v] < V){ // if filter begin 
    flag1 = true;
    int cycle = kernel_cycles; // DEVICE ASSTMENT in .h

    do{
      if (d_excess[v] > 0 && d_height[v] < V){ // if filter begin 
        int hh = INT_MAX; // DEVICE ASSTMENT in .h

        int v_0 = -1; // DEVICE ASSTMENT in .h

        int forward_edge = -1; // DEVICE ASSTMENT in .h

        int backward_edge = -1; // DEVICE ASSTMENT in .h

        for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
          int vv = d_data[edge];
          int e = edge;
          if (d_height[vv] < hh && d_residual_capacity[e] > 0){ // if filter begin 
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
          if (e != -1){ // if filter begin 
            if (d_height[vv] < hh && d_rev_residual_capacity[e] > 0){ // if filter begin 
              v_0 = vv;
              hh = d_height[vv];
              backward_edge = e;
              forward_edge = -1;

            } // if filter end

          } // if filter end

        } //  end FOR NBR ITR. TMP FIX!
        if (d_height[v] > hh && v_0 != -1){ // if filter begin 
          if (forward_edge != -1){ // if filter begin 
            int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

            int p = d_parallel_edge[forward_edge]; // DEVICE ASSTMENT in .h

            int d = fec; // DEVICE ASSTMENT in .h

            if (d_excess[v] < fec){ // if filter begin 
              d = d_excess[v];

            } // if filter end
            atomicSub(& d_excess[v] , d);
            atomicAdd(& d_excess[v_0] , d);
            atomicSub(& d_residual_capacity[forward_edge] , d);
            atomicAdd(& d_rev_residual_capacity[forward_edge] , d);
            if (p != -1){ // if filter begin 
              atomicAdd(& d_residual_capacity[p] , d);
              atomicSub(& d_rev_residual_capacity[p] , d);

            } // if filter end

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
      cycle = cycle - 1;

    }while(cycle > 0);

  } // if filter end
} // end KER FUNC
__device__ int source ;
__device__ int sink ;
__device__ int kernel_cycles ;
__device__ int V ; // DEVICE ASSTMENT in .h

__global__ void incremental_kernel_15(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_residual_capacity,int* d_reverse_edge,int* d_excess,int* d_rev_residual_capacity,int* d_parallel_edge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == source){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      if (d > 0){ // if filter begin 
        atomicAdd(& d_excess[vv] , d);
        atomicSub(& d_excess[v] , d);
        atomicSub(& d_residual_capacity[forward_edge] , d);
        atomicAdd(& d_rev_residual_capacity[forward_edge] , d);
        int p = d_parallel_edge[forward_edge]; // DEVICE ASSTMENT in .h

        if (p != -1){ // if filter begin 
          atomicAdd(& d_residual_capacity[p] , d);
          atomicSub(& d_rev_residual_capacity[p] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int backward_edge = edge;
      int forward_edge = d_reverse_edge[backward_edge]; // DEVICE ASSTMENT in .h

      if (forward_edge != -1){ // if filter begin 
        int d = d_rev_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

        if (d > 0){ // if filter begin 
          atomicAdd(& d_excess[vv] , d);
          atomicSub(& d_excess[v] , d);
          atomicSub(& d_residual_capacity[forward_edge] , d);
          atomicAdd(& d_rev_residual_capacity[forward_edge] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag1 ; // DEVICE ASSTMENT in .h

__global__ void incremental_kernel_18(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_rev_residual_capacity,int* d_excess,int* d_reverse_edge,int* d_height,int* d_residual_capacity,int* d_parallel_edge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] > 0 && v != source && v != sink && d_height[v] < V){ // if filter begin 
    flag1 = true;
    int cycle = kernel_cycles; // DEVICE ASSTMENT in .h

    do{
      if (d_excess[v] > 0 && d_height[v] < V){ // if filter begin 
        int hh = INT_MAX; // DEVICE ASSTMENT in .h

        int v_0 = -1; // DEVICE ASSTMENT in .h

        int forward_edge = -1; // DEVICE ASSTMENT in .h

        int backward_edge = -1; // DEVICE ASSTMENT in .h

        for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
          int vv = d_data[edge];
          int e = edge;
          if (d_height[vv] < hh && d_residual_capacity[e] > 0){ // if filter begin 
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
          if (e != -1){ // if filter begin 
            if (d_height[vv] < hh && d_rev_residual_capacity[e] > 0){ // if filter begin 
              v_0 = vv;
              hh = d_height[vv];
              backward_edge = e;
              forward_edge = -1;

            } // if filter end

          } // if filter end

        } //  end FOR NBR ITR. TMP FIX!
        if (d_height[v] > hh && v_0 != -1){ // if filter begin 
          if (forward_edge != -1){ // if filter begin 
            int fec = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

            int p = d_parallel_edge[forward_edge]; // DEVICE ASSTMENT in .h

            int d = fec; // DEVICE ASSTMENT in .h

            if (d_excess[v] < fec){ // if filter begin 
              d = d_excess[v];

            } // if filter end
            atomicSub(& d_excess[v] , d);
            atomicAdd(& d_excess[v_0] , d);
            atomicSub(& d_residual_capacity[forward_edge] , d);
            atomicAdd(& d_rev_residual_capacity[forward_edge] , d);
            if (p != -1){ // if filter begin 
              atomicAdd(& d_residual_capacity[p] , d);
              atomicSub(& d_rev_residual_capacity[p] , d);

            } // if filter end

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
      cycle = cycle - 1;

    }while(cycle > 0);

  } // if filter end
} // end KER FUNC
__device__ int source ;
__device__ int sink ;
__device__ int cycle ;
__device__ int V ; // DEVICE ASSTMENT in .h

__global__ void decremental_kernel_21(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_reverse_edge,int* d_rev_residual_capacity,int* d_excess,int* d_residual_capacity,int* d_parallel_edge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v == sink){ // if filter begin 
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge;
      int d = d_rev_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

      if (d > 0){ // if filter begin 
        atomicAdd(& d_excess[v] , d);
        atomicSub(& d_excess[vv] , d);
        atomicSub(& d_rev_residual_capacity[forward_edge] , d);
        atomicAdd(& d_residual_capacity[forward_edge] , d);
        int p = d_parallel_edge[forward_edge]; // DEVICE ASSTMENT in .h

        if (p != -1){ // if filter begin 
          atomicSub(& d_residual_capacity[p] , d);
          atomicAdd(& d_rev_residual_capacity[p] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int vv = d_src[edge] ;
      int backward_edge = edge;
      int forward_edge = d_reverse_edge[backward_edge]; // DEVICE ASSTMENT in .h

      if (forward_edge != -1){ // if filter begin 
        int d = d_residual_capacity[forward_edge]; // DEVICE ASSTMENT in .h

        if (d > 0){ // if filter begin 
          atomicAdd(& d_excess[v] , d);
          atomicSub(& d_excess[vv] , d);
          atomicAdd(& d_residual_capacity[forward_edge] , d);
          atomicSub(& d_rev_residual_capacity[forward_edge] , d);

        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__device__ bool flag1 ; // DEVICE ASSTMENT in .h

__global__ void decremental_kernel_24(int V, int E, int* d_meta, int* d_data, int* d_src, int *d_rev_meta,int* d_excess,int* d_residual_capacity,int* d_height,int* d_parallel_edge,int* d_reverse_edge,int* d_rev_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] < 0 && v != source && v != sink && d_height[v] < V){ // if filter begin 
    flag1 = true;
    int cycle = kernel_cycles; // DEVICE ASSTMENT in .h

    do{
      if (d_excess[v] < 0 && d_height[v] < V){ // if filter begin 
        int hh = INT_MAX; // DEVICE ASSTMENT in .h

        int v_0 = -1; // DEVICE ASSTMENT in .h

        int inward_edge = -1; // DEVICE ASSTMENT in .h

        int outward_edge = -1; // DEVICE ASSTMENT in .h

        for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
          int vv = d_data[edge];
          int e = edge;
          if (d_height[vv] < hh && d_rev_residual_capacity[e] > 0){ // if filter begin 
            v_0 = vv;
            hh = d_height[vv];
            outward_edge = e;

          } // if filter end

        } //  end FOR NBR ITR. TMP FIX!
        for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
        {
          int vv = d_src[edge] ;
          int rev = edge;
          int e = d_reverse_edge[rev];
          if (e != -1){ // if filter begin 
            if (d_height[vv] < hh && d_residual_capacity[e] > 0){ // if filter begin 
              v_0 = vv;
              hh = d_height[vv];
              inward_edge = e;
              outward_edge = -1;

            } // if filter end

          } // if filter end

        } //  end FOR NBR ITR. TMP FIX!
        if (d_height[v] > hh && v_0 != -1){ // if filter begin 
          if (outward_edge != -1){ // if filter begin 
            int fec = d_rev_residual_capacity[outward_edge]; // DEVICE ASSTMENT in .h

            int p = d_parallel_edge[outward_edge]; // DEVICE ASSTMENT in .h

            int d = fec; // DEVICE ASSTMENT in .h

            if (d_excess[v] < fec){ // if filter begin 
              d = d_excess[v] * -1;

            } // if filter end
            atomicAdd(& d_excess[v] , d);
            atomicSub(& d_excess[v_0] , d);
            atomicSub(& d_rev_residual_capacity[outward_edge] , d);
            atomicAdd(& d_residual_capacity[outward_edge] , d);
            if (p != -1){ // if filter begin 
              atomicSub(& d_residual_capacity[p] , d);
              atomicAdd(& d_rev_residual_capacity[p] , d);

            } // if filter end

          } // if filter end
          else {
            int fec = d_rev_residual_capacity[inward_edge]; // DEVICE ASSTMENT in .h

            int d = fec; // DEVICE ASSTMENT in .h

            if (d_excess[v] < fec){ // if filter begin 
              d = d_excess[v] * -1;

            } // if filter end
            atomicAdd(& d_excess[v] , d);
            atomicSub(& d_excess[v_0] , d);
            atomicSub(& d_residual_capacity[inward_edge] , d);
            atomicAdd(& d_rev_residual_capacity[inward_edge] , d);

          }

        } // if filter end
        else {
          if (v_0 != -1){ // if filter begin 
            d_height[v] = hh + 1;

          } // if filter end
        }

      } // if filter end
      cycle = cycle - 1;

    }while(cycle > 0);

  } // if filter end
} // end KER FUNC
__global__ void OnAdd_kernel(update* d_updateBatch, int batchelements, int* d_meta, int* d_data, int* d_weight,int* d_rev_residual_capacity,int* d_parallel_edge,int* d_residual_capacity){ // BEGIN KER FUN via ADDKERNEL
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
  d_residual_capacity[uedge] = d_residual_capacity[uedge] + new_capacity - d_weight[uedge];
  int p = d_parallel_edge[uedge]; // DEVICE ASSTMENT in .h

  if (p != -1){ // if filter begin 
    d_rev_residual_capacity[p] = d_rev_residual_capacity[p] + new_capacity - d_weight[uedge];
  } // if filter end
  d_weight[uedge] = new_capacity;
} // end KER FUNC
__global__ void recalculate_max_flow_kernel_28(int V, int E, int* d_meta, int* d_data, int* d_src, int* d_weight, int *d_rev_meta,int* d_reverse_edge,int* d_rev_residual_capacity,int* d_residual_capacity,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dest = d_data[edge];
    int e = edge;
    if (d_residual_capacity[e] < 0){ // if filter begin 
      d_rev_residual_capacity[e] = d_rev_residual_capacity[e] + d_residual_capacity[e];
      d_residual_capacity[e] = 0;

    } // if filter end
    else {
      if (d_rev_residual_capacity[e] < 0){ // if filter begin 
        d_residual_capacity[e] = d_residual_capacity[e] + d_rev_residual_capacity[e];
        d_rev_residual_capacity[e] = 0;

      } // if filter end
    }

  } //  end FOR NBR ITR. TMP FIX!
  int e1 = 0; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e = edge;
    int f = d_weight[e] - d_residual_capacity[e]; // DEVICE ASSTMENT in .h

    e1 = e1 - f;

  } //  end FOR NBR ITR. TMP FIX!
  for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
  {
    int src = d_src[edge] ;
    int rev = edge; // DEVICE ASSTMENT in .h

    int e = d_reverse_edge[rev]; // DEVICE ASSTMENT in .h

    if (e != -1){ // if filter begin 
      e1 = e1 + d_rev_residual_capacity[e];

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  d_excess[v] = e1;
} // end KER FUNC
__global__ void recalculate_max_flow_kernel_32(int V, int E,int* d_excess){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_excess[v] < 0){ // if filter begin 
    atomicAdd(& d_excess[sink] , d_excess[v]);

  } // if filter end
} // end KER FUNC
