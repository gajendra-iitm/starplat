#include "libOpenCL.h"
__kernel void initlabel_kernel( __global int *d_label , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_label[id] = value;
  }
}

__kernel void initIndexlabel_kernel(__global int *d_label, int source , int val){
  d_label[source] = val ; 
}
__kernel void initexcess_kernel( __global int *d_excess , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_excess[id] = value;
  }
}
__kernel void do_max_flow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int* source,__global int * d_residual_capacity,__global int* excesstotal,__global int * d_excess){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  if (v == (*source)){ // if filter begin 

    //ForAll started here
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int vv = d_data[edge];
      int forward_edge = edge; int backward_edge = edge; int d = d_residual_capacity[forward_edge]; 
      atomic_add( excesstotal, (int)d);

      d_excess[vv] = d_excess[vv]+ d;

      d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;

      d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;


    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end
} // end KER FUNC
__kernel void do_max_flow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_excess,__global int* source,__global int* sink,__global int * d_label,__global int * d_residual_capacity,__global int* kernel_parameter){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  int x = (*kernel_parameter); 
  do{
    x = x - 1;
    int hh = 100000000; 
    int lv = -1; 
    if (d_excess[v] > 0 && v != (*source) && v != (*sink) && d_label[v] < V){ // if filter begin 

      //ForAll started here
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int vv = d_data[edge];
        int e = edge; int xxx = d_residual_capacity[e]; 
        if (d_label[vv] < hh && xxx > 0){ // if filter begin 
          lv = vv;
          hh = d_label[vv];

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!
      if (d_label[v] > hh && lv != -1){ // if filter begin 
        int current_edge = edge; int cec = d_residual_capacity[current_edge]; 
        if (d_excess[v] > 0 && cec > 0){ // if filter begin 
          int forward_edge = edge; int backward_edge = edge; int fec = d_residual_capacity[forward_edge]; 
          int bec = d_residual_capacity[backward_edge]; 
          int d = fec; 
          if (d_excess[v] < fec){ // if filter begin 
            d = d_excess[v];

          } // if filter end
          d_excess[v] = d_excess[v]- d;

          d_excess[lv] = d_excess[lv]+ d;

          d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;

          d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;


        } // if filter end

      } // if filter end
      else
      if (lv != -1){ // if filter begin 
        d_label[v] = hh + 1;

      } // if filter end

    } // if filter end

  }while(x > 0);
} // end KER FUNC
__kernel void do_max_flow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_residual_capacity,__global int * d_excess,__global int * d_label){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;

  //ForAll started here
  for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
    int vv = d_data[edge];
    int e = edge; int ee = edge; int ec = d_residual_capacity[e]; 
    if (ec > 0 && d_label[v] > d_label[vv] + 1 && d_excess[v] > 0){ // if filter begin 
      int d = ec; 
      if (d > d_excess[v]){ // if filter begin 
        d = d_excess[v];
      } // if filter end
      d_excess[v] = d_excess[v]- d;

      d_excess[vv] = d_excess[vv]+ d;

      d_residual_capacity[e] = d_residual_capacity[e]- d;

      d_residual_capacity[ee] = d_residual_capacity[ee]+ d;


    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
} // end KER FUNC
__kernel void initvisit_kernel( __global int *d_visit , int value, int v){
  unsigned int id = get_global_id(0);
  if(id<v){
    d_visit[id] = value;
  }
}
__kernel void do_max_flow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_residual_capacity,__global int* finished,__global int * d_label,__global int * d_visit){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  if (d_visit[v] == 1){ // if filter begin 

    //ForAll started here
    for (int edge = d_rev_meta[v]; edge < d_rev_meta[v+1]; edge++)
    {
      int w = d_src[edge] ;
      int e = edge; int ec = d_residual_capacity[e]; 
      if (d_visit[w] == 0 && ec > 0){ // if filter begin 
        d_visit[w] = 1;
        d_label[w] = d_label[v] + 1;
        atomic_add( finished, (int)1);


      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    d_visit[v] = 2;

  } // if filter end
} // end KER FUNC
__kernel void do_max_flow_kernel(int V,  int E, __global int* d_meta, __global int* d_data, __global int* d_src,
  __global int* d_weight,__global int* d_rev_meta ,__global int * d_visit,__global int* excesstotal,__global int * d_excess,__global int * d_label){ // BEGIN KER FUN via ADDKERNEL
  unsigned int v = get_global_id(0);
  if(v >= V) return;
  if (d_visit[v] == 0){ // if filter begin 
    d_label[v] = 1000000000;
    int x = d_excess[v]; 
    atomic_add( excesstotal, (int)x);

    d_excess[v] = 0;

  } // if filter end
} // end KER FUNC
