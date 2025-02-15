// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_DSL_BORUVKAS-V2_H
#define GENCPP_MST_DSL_BORUVKAS-V2_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void mst_boruvkas(graph& g);



__global__ void mst_boruvkas_kernel_1(int V, int E, int* d_meta, int* d_data,int* d_parent,bool* d_isCovered,int* d_destination){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    d_isCovered[e] = false;
    d_destination[e] = dst;

  } //  end FOR NBR ITR. TMP FIX!
  if (d_parent[v] == -1){ // if filter begin 
    d_parent[v] = v;

  } // if filter end
} // end KER FUNC
__device__ bool hasComponent ; // DEVICE ASSTMENT in .h

__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void mst_boruvkas_kernel_2(int V, int E, int* d_meta, int* d_data,int* d_minweight,bool* d_isCovered,int* d_minedge, int *d_weight){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    if (d_isCovered[e] == false){ // if filter begin 
      int x = d_minweight[src]; // DEVICE ASSTMENT in .h
      if (x == -1 || x > d_weight[e]){ // if filter begin 
        d_minweight[src] = e.weight;
        d_minedge[src] = e;
      } // if filter end
    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void mst_boruvkas_kernel_3(int V, int E,int* d_minweight,int* d_minedge,bool* d_modified,bool* d_modified_next,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 
    int par = d_parent[v]; // DEVICE ASSTMENT in .h

    if (d_minweight[v] < d_minweight[par]){ // if filter begin 
      d_minweight[par] = d_minweight[v];
      d_minedge[par] = d_minedge[v];
      finished = false;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void mst_boruvkas_kernel_4(int V, int E,int* d_parent,int* d_destination,int* d_minedge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_parent[v] == v){ // if filter begin 
    int e = d_minedge[v]; // DEVICE ASSTMENT in .h

    if (e != -1){ // if filter begin 
      int dst1 = d_destination[e]; // DEVICE ASSTMENT in .h

      int pardst1 = d_parent[e]; // DEVICE ASSTMENT in .h

      int epar = d_minedge[pardst1]; // DEVICE ASSTMENT in .h

      if (epar != -1){ // if filter begin 
        int dstdst = d_destination[epar]; // DEVICE ASSTMENT in .h
        int dstdstpar = d_parent[dstdst];
        if (dstdstpar == v && pardst1 < v){ // if filter begin 
          d_minedge[pardst1] = -1;

        } // if filter end

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void mst_boruvkas_kernel_5(int V, int E,int* d_parent,int* d_destination,int* d_minedge,bool* d_isCovered){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_parent[v] == v){ // if filter begin 
    int e = d_minedge[v]; // DEVICE ASSTMENT in .h

    if (e != -1){ // if filter begin 
      d_isCovered[e] = true;
      hasComponent = true;
      int dst = d_destination[e]; // DEVICE ASSTMENT in .h

      d_parent[v] = dst;

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void mst_boruvkas_kernel_6(int V, int E,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int par = d_parent[v]; // DEVICE ASSTMENT in .h

  if (d_parent[par] != par){ // if filter begin 
    finished = false;
    d_parent[v] = d_parent[par];

  } // if filter end
} // end KER FUNC

#endif
