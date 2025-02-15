#include "libOpenCL.h"
__kernel void initd_level_kernel(__global int *d_level, unsigned int V)
{
  unsigned int id = get_global_id(0);
  if(id>=V) return ;
  d_level[id] = -1;
}
__kernel void initIndexd_level_kernel(__global int* d_level, int src)
{
  d_level[src] = 0;
}
__kernel void fwd_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished
  ,__global int* d_cap ,__global int* d_weight)
{
  unsigned int cur = get_global_id(0);
  if(cur>= V) return ;
  if(d_level[cur]== (*d_hops_from_source))
  {
    if (d_in_bfs[cur] == true && terminate == false){ // if filter begin 

      //ForAll started here
      for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
        int nbr = d_data[edge];
        int e = edge; if (d_par[nbr] == -1 && d_cap[e] > 0){ // if filter begin 
          d_par[nbr] = cur;
          cnew_flow = d_n_flow[cur];
          if (cnew_flow > d_cap[e]){ // if filter begin 
            cnew_flow = d_cap[e];

          } // if filter end
          if (nbr == d){ // if filter begin 
            new_flow = cnew_flow;
            terminate = true;

          } // if filter end
          d_in_bfs[nbr] = true;
          d_n_flow[nbr] = cnew_flow;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end
  }// end of if block
} // end of kernel
__kernel void back_pass_kernel(int V, __global int* d_meta, __global int* d_data, __global int* d_weight, __global int* d_level, __global int* d_hops_from_source, __global int* d_finished,__global int* d_cap ,__global int* d_weight)
{
  unsigned int cur = get_global_id(0);
  if(cur>= V) return ;
  if(d_level[cur]==(*d_hops_from_source)-1)
  {
  }//end of if
}//end of kernel
