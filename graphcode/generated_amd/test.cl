#include "libOpenCL.h"
__kernel void bp1_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
} // end KER FUNC
