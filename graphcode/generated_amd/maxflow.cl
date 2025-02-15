#include "libOpenCL.h"
__kernel void maxflow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_cf,__global int* s,__global int* N,__global int * d_e,__global int * d_h){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  if (v == (*s)){ // if filter begin 
    d_h[v] = (*N);
    d_e[v] = 0;

  } // if filter end
  else
  d_h[v] = 0;
  d_e[v] = 0;


  //ForAll started here
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int nbr = d_data[edge];
    int e1 = edge; d_cf[e1] = d_weight[e1];
    if (d_cf[e1] == -1){ // if filter begin 
      d_cf_backward[e1] = 0;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int nbr = d_data[edge];
  __kernel void maxflow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
    __global int* d_weight,__global int* d_rev_meta ,__global int * d_h,__global int* v,__global int * d_e){ // BEGIN KER FUN via ADDKERNEL
    unsigned int v = get_global_id(0);
    if(v >= V) return;
    int cycle = 100; 
    do{
      int e1 = d_e[v]; 
      int h1 = INT_MAX; 
      int v1 = -1; 

      //ForAll started here
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int nbr = d_data[edge];
        int h2 = d_h[nbr]; 
        if (h2 < h1){ // if filter begin 
          v1 = nbr;
          h1 = h2;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      if (d_h[v] > h1){ // if filter begin 
        int ee = edge; int d = -1; 
        if (e1 < d_cf_forward[ee]){ // if filter begin 
          d = e1;

        } // if filter end
        else
        d = d_cf_forward[ee];

        d_cf_backward[ee] = d_cf_backward[ee]- d;


      } // if filter end

    }while(cycle > 0);
  } // end KER FUNC
