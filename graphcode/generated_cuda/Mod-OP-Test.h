// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MOD-OP-TEST_H
#define GENCPP_MOD-OP-TEST_H
#include <stdio.h>
#include <stdlib.h>
#include "ParallelHeapCudaClass.cu"
#include "deepak_map_cuda.cu"
#include <limits.h>
#include <cuda.h>
#include "../graph.hpp"
#include "../libcuda.cuh"
#include <cooperative_groups.h>

void tst(int a,int b);

__device__ int a ;
__device__ int b ;


__device__ int c ; // DEVICE ASSTMENT in .h


#endif
