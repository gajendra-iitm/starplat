// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_EDMONDS-KARP_H
#define GENCPP_EDMONDS-KARP_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void ek(graph& g,int s,int d,int* weight
);



__device__ int flow ; // DEVICE ASSTMENT in .h

__device__ int new_flow ; // DEVICE ASSTMENT in .h

__device__ int cnew_flow ; // DEVICE ASSTMENT in .h

__device__ int e_cap ; // DEVICE ASSTMENT in .h

__device__ bool terminate ; // DEVICE ASSTMENT in .h


#endif
