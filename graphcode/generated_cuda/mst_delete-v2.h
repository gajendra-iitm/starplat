// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_DELETE-V2_H
#define GENCPP_MST_DELETE-V2_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void recolor(bool* isCovered,graph& g,int* parent,int* destination
);



__global__ void recolor_kernel(int V, int E,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (true){ // if filter begin 
    d_parent[v] = v;

  } // if filter end
} // end KER FUNC
__device__ bool rfinish1 ; // DEVICE ASSTMENT in .h

__global__ void recolor_kernel(int V, int E, int* d_meta, int* d_data,int* d_destination,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int mycolor = d_parent[v]; // DEVICE ASSTMENT in .h

  int newcomponent = mycolor; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    if (d_destination[e] != -1){ // if filter begin 
      int othercolor = d_parent[dst]; // DEVICE ASSTMENT in .h

      if (othercolor < newcomponent){ // if filter begin 
        newcomponent = othercolor;

      } // if filter end

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  if (mycolor > newcomponent){ // if filter begin 
    d_parent[v] = newcomponent;
    rfinish1 = false;

  } // if filter end
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    if (d_destination[e] != -1){ // if filter begin 
      if (d_parent[dst] > newcomponent){ // if filter begin 
        d_parent[dst] = newcomponent;
        rfinish1 = false;

      } // if filter end

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__device__ bool rfinish2 ; // DEVICE ASSTMENT in .h

__global__ void recolor_kernel(int V, int E,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int par = d_parent[v]; // DEVICE ASSTMENT in .h

  if (d_parent[par] != par){ // if filter begin 
    d_parent[v] = d_parent[par];
    rfinish2 = false;

  } // if filter end
} // end KER FUNC

#endif
