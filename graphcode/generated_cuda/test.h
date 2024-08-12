// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_TEST_H
#define GENCPP_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include "../graph.cpp"
#include "../libcuda.cuh"

#include <cooperative_groups.h>

env environment("cuda", "static", "test");
void test1(graph& g,float* features,int* labels);



// CSR BEGIN

int* d_meta;
int* d_data;



// CSR END
//LAUNCH CONFIG
// TIMER START
__device__ int num_epoch ;
__device__ int num_layer ;
__device__ int x ;
__device__ bool flag ;
__device__ bool flag1 ;
__global__ void test1_kernel(int V, int E, int* d_meta, int* d_data,int* d_labels){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  x = x + 1;
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int n = d_data[edge];
    if (d_labels[n] == d_labels[nod]){ // if filter begin 
      int num_neurons = layers.getIdx(x);
      message_pass(nod,n,features,"sum",num_neurons,1);


    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC

#endif
