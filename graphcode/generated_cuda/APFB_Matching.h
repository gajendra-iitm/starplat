// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_APFB_MATCHING_H
#define GENCPP_APFB_MATCHING_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void APFB(graph& g,int nc);

__device__ int nc ;


__device__ bool noNewPaths ; // DEVICE ASSTMENT in .h

__device__ int L0 ; // DEVICE ASSTMENT in .h

__device__ int NOT_VISITED ; // DEVICE ASSTMENT in .h

__global__ void APFB0_kernel(int V, int E,int* d_cmatch,int* d_bfsArray){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v < nc){ // if filter begin 
    if (d_cmatch[v] == -1){ // if filter begin 
      d_bfsArray[v] = L0;

    } // if filter end

  } // if filter end
} // end KER FUNC
__device__ int bfsLevel ; // DEVICE ASSTMENT in .h

__device__ bool noNewVertices ; // DEVICE ASSTMENT in .h

__global__ void APFB1_kernel(int V, int E, int* d_meta, int* d_data,int* d_bfsArray,int* d_rmatch,int* d_predeccesor){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v < nc){ // if filter begin 
    if (d_bfsArray[v] == bfsLevel){ // if filter begin 
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int neigh_row = d_data[edge];
        if (neigh_row >= nc){ // if filter begin 
          int col_match = d_rmatch[neigh_row]; // DEVICE ASSTMENT in .h

          if (col_match > -1){ // if filter begin 
            if (d_bfsArray[col_match] == NOT_VISITED){ // if filter begin 
              noNewVertices = false;
              d_bfsArray[col_match] = bfsLevel + 1;
              d_predeccesor[neigh_row] = v;

            } // if filter end

          } // if filter end
          if (col_match == -1){ // if filter begin 
            d_rmatch[neigh_row] = -2;
            d_predeccesor[neigh_row] = v;
            noNewPaths = false;

          } // if filter end

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void APFB2_kernel(int V, int E,int* d_rmatch,bool* d_compress){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v >= nc && d_rmatch[v] == -2){ // if filter begin 
    d_compress[v] = true;

  } // if filter end
} // end KER FUNC
__device__ bool compressed ; // DEVICE ASSTMENT in .h

__global__ void APFB3_kernel(int V, int E,bool* d_compress,int* d_predeccesor,int* d_cmatch,bool* d_compress_next,int* d_rmatch){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (d_compress[v] == true){ // if filter begin 
    if (v >= nc){ // if filter begin 
      int matched_col = d_predeccesor[v]; // DEVICE ASSTMENT in .h

      int matched_row = d_cmatch[matched_col]; // DEVICE ASSTMENT in .h

      bool isValid = true; // DEVICE ASSTMENT in .h

      if (matched_row != -1){ // if filter begin 
        if (d_predeccesor[matched_row] == matched_col){ // if filter begin 
          isValid = false;

        } // if filter end

      } // if filter end
      if (isValid){ // if filter begin 
        d_cmatch[matched_col] = v;
        d_rmatch[v] = matched_col;
        if (matched_row != -1){ // if filter begin 
          d_compress_next[matched_row] = true;
          compressed = false;

        } // if filter end

      } // if filter end

    } // if filter end

  } // if filter end
} // end KER FUNC
__global__ void APFB4_kernel(int V, int E,bool* d_compress_next,bool* d_compress){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v >= nc){ // if filter begin 
    d_compress[v] = d_compress_next[v];
    d_compress_next[v] = false;

  } // if filter end
} // end KER FUNC
__global__ void APFB5_kernel(int V, int E,int* d_rmatch,int* d_cmatch){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  if (v >= nc){ // if filter begin 
    int c = d_rmatch[v]; // DEVICE ASSTMENT in .h

    if (c >= 0){ // if filter begin 
      if (d_cmatch[c] != v){ // if filter begin 
        d_rmatch[v] = -1;

      } // if filter end

    } // if filter end
    if (c == -2){ // if filter begin 
      d_rmatch[v] = -1;

    } // if filter end

  } // if filter end
} // end KER FUNC

#endif
