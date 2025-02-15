// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_DELETE_H
#define GENCPP_MST_DELETE_H
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
  initIndex<int><<<1,1>>>(V,d_parent,v,(int)v); //InitIndexDevice
} // end KER FUNC
__device__ bool rfinish1 ; // DEVICE ASSTMENT in .h

__global__ void recolor_kernel(int V, int E, int* d_meta, int* d_data,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int mycolor = d_parent[v]; // DEVICE ASSTMENT in .h

  int newcomponent = mycolor; // DEVICE ASSTMENT in .h

  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int othercolor = d_parent[dst]; // DEVICE ASSTMENT in .h

    if (othercolor < newcomponent){ // if filter begin 
      newcomponent = othercolor;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  if (mycolor > newcomponent){ // if filter begin 
    d_parent[v] = newcomponent;
    rfinish1 = false;

  } // if filter end
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    if (d_parent[dst] > newcomponent){ // if filter begin 
      d_parent[dst] = newcomponent;
      rfinish1 = false;

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
void mst_boruvkas_add_edges(graph& g,bool* isCovered,int* parent);



__global__ void mst_boruvkas_add_edges_kernel(int V, int E, int* d_meta, int* d_data,int* d_destination){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    d_destination[e] = dst;

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__device__ bool hasComponent ; // DEVICE ASSTMENT in .h

__device__ bool finished ; // DEVICE ASSTMENT in .h

__global__ void mst_boruvkas_add_edges_kernel(int V, int E, int* d_meta, int* d_data,int* d_minweight,bool* d_isCovered){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    if (d_isCovered[e] == false){ // if filter begin 
      int x = d_minweight[src]; // DEVICE ASSTMENT in .h

      if (x == -1 || x > d_weight[e]){ // if filter begin 
        x = d_weight[e];

      } // if filter end
      d_minweight[src] = x;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void mst_boruvkas_add_edges_kernel(int V, int E, int* d_meta, int* d_data,int* d_minweight,bool* d_isCovered,int* d_minedge){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    if (d_isCovered[e] == false){ // if filter begin 
      if (d_weight[e] == d_minweight[src]){ // if filter begin 
        d_minedge[src] = e;

      } // if filter end

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__global__ void mst_boruvkas_add_edges_kernel(int V, int E,bool* d_modified,bool* d_modified_next,int* d_minedge,int* d_minweight,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_modified[v] == true){ // if filter begin 
    int par = d_parent[v]; // DEVICE ASSTMENT in .h

    if (d_minweight[v] >= d_minweight[par]){ // if filter begin 
      d_modified[v] = false;
      finished = false;

    } // if filter end
    if (d_minweight[v] < d_minweight[par]){ // if filter begin 
      d_minweight[par] = d_minweight[v];
      d_minedge[par] = d_minedge[v];

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void mst_boruvkas_add_edges_kernel(int V, int E,int* d_parent,int* d_destination,int* d_minedge){ // BEGIN KER FUN via ADDKERNEL
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

        if (dstdst == v && pardst1 < v){ // if filter begin 
          d_minedge[pardst1] = -1;

        } // if filter end

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void mst_boruvkas_add_edges_kernel(int V, int E,int* d_parent,int* d_destination,int* d_minedge,bool* d_isCovered){ // BEGIN KER FUN via ADDKERNEL
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
__global__ void mst_boruvkas_add_edges_kernel(int V, int E,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  int par = d_parent[v]; // DEVICE ASSTMENT in .h

  if (d_parent[par] != par){ // if filter begin 
    finished = false;
    d_parent[v] = d_parent[par];

  } // if filter end
} // end KER FUNC
void total_code(graph& g,bool* isCovered);



__global__ void total_code_kernel(int V, int E, int* d_meta, int* d_data,int* d_destination,int* d_parent){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  initIndex<int><<<1,1>>>(V,d_parent,v,(int)v); //InitIndexDevice
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int dst = d_data[edge];
    int e = edge;
    d_destination[e] = dst;

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC

#endif
