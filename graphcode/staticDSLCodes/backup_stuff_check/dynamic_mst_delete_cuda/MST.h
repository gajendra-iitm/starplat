// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_MST_V1_H
#define GENCPP_MST_V1_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda.h>
#include "graphstatic.hpp"
#include "libcuda.cuh"
#include <cooperative_groups.h>

void Boruvka(graphstatic& g);
__global__ void Boruvka_kernel_1(int V, int* d_color){ 
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  d_color[u] = u;
} 
__device__ bool noNewComp ; 

__global__ void Boruvka_kernel_2(int V, int* d_offset,int *d_diff_offset, int* d_edgeList,int* d_diff_edgeList, int* d_edgeLen,int* d_diff_edgeLen, int* d_minEdge,int* d_color){ // BEGIN KER FUN via ADDKERNEL
  int src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  int edge = d_offset[src];
  for (; edge < d_offset[src+1]; edge++) { 
    int dst = d_edgeList[edge];
    if(dst==-1) continue;
    if (d_color[src] != d_color[dst]){
      int e = edge;
      int minEdge = d_minEdge[src];
      if (minEdge == -1){ 
        d_minEdge[src] = e;

      }
      else{ 
        int minDst = d_edgeList[minEdge];
        if (d_edgeLen[e] < d_edgeLen[minEdge] || (d_edgeLen[e] == d_edgeLen[minEdge] && d_color[dst] < d_color[minDst])){ 
          d_minEdge[src] = e;
        } 
      } 
    } 
  } 
  edge = d_offset[V];
  int diff_minEdge=-1;
  for (int j = d_diff_offset[src]; j < d_diff_offset[src+1]; j++) { 
    int dst = d_diff_edgeList[j];
    if(dst==-1) {continue;}
    if (d_color[src] != d_color[dst]){ 
      int e = edge+j;
      if (diff_minEdge == -1){ 
        diff_minEdge= e;
      }
      else { 
        int minDst = d_diff_edgeList[diff_minEdge-edge];
        if (d_diff_edgeLen[j] < d_diff_edgeLen[diff_minEdge-edge] || (d_diff_edgeLen[j] == d_diff_edgeLen[diff_minEdge-edge] && d_color[dst] < d_color[minDst])){ 
          diff_minEdge = e;
        } 
      } 
    } 
  } 
  if(diff_minEdge!=-1)
  {
    if(d_minEdge[src]==-1)
    {
      d_minEdge[src] = diff_minEdge;
    } else {
        int minDst = d_edgeList[d_minEdge[src]];
        int mindiffDst = d_diff_edgeList[diff_minEdge-edge];
        if (d_diff_edgeLen[diff_minEdge-edge] < d_edgeLen[d_minEdge[src]] || (d_diff_edgeLen[diff_minEdge-edge] == d_edgeLen[d_minEdge[src]] && d_color[mindiffDst] < d_color[minDst])){ 
          d_minEdge[src] = diff_minEdge;
        } 
    }
  }
} 

__device__ bool finishedMinEdge ; // DEVICE ASSTMENT in .h

__global__ void Boruvka_kernel_3_1(int V, int*d_offset,int* d_edgeLen,int* d_diff_edgeLen, int* d_minEdge,int* d_color, int *d_minEdgeOfCompW){ // BEGIN KER FUN via ADDKERNEL
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int comp = d_color[u];
  int e = d_minEdge[u];
  
  if (e != -1){ 
    int w = -1;
    if(e<d_offset[V])
    {
       w = d_edgeLen[e];
    }
    else{
      w = d_diff_edgeLen[e-d_offset[V]];
    }
    atomicCAS(&d_minEdgeOfCompW[comp],(int)-1,w);
    atomicMin(&d_minEdgeOfCompW[comp],w);
    
  } 
}

__global__ void Boruvka_kernel_3_2(int V, int*d_offset, int* d_edgeList,  int* d_edgeLen,int* d_diff_edgeList,  int* d_diff_edgeLen, int* d_minEdge,int* d_color,int *d_minEdgeOfCompW,int *d_minEdgeOfCompMin)
{ 
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int comp = d_color[u]; 
  int minEdgeW = d_minEdgeOfCompW[comp]; 
  int e = d_minEdge[u]; 
  if (e != -1){ 
    int w = -1;
    int dst = -1;
    if(e<d_offset[V])
    {
      w = d_edgeLen[e];
      dst = d_edgeList[e];
    }
    else
    {
      w = d_diff_edgeLen[e-d_offset[V]];
      dst = d_diff_edgeList[e-d_offset[V]];
    }
    int minval = min((int)dst,(int)u);
    if(minEdgeW < w) return;
    atomicCAS(&d_minEdgeOfCompMin[comp],-1,minval);
    atomicMin(&d_minEdgeOfCompMin[comp],minval);
  } 
} 
__global__ void Boruvka_kernel_3_3(int V, int* d_edgeList, int* d_edgeLen,int *d_diff_edgeLen, int* d_diff_edgeList,int* d_minEdge,int* d_minEdgeOfComp,int* d_color, int *d_minEdgeOfCompW,int *d_minEdgeOfCompMin, int *d_minEdgeOfCompMax, int *d_offset){ // BEGIN KER FUN via ADDKERNEL
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int comp = d_color[u]; 
  int minEdgeW = d_minEdgeOfCompW[comp]; 
  int e = d_minEdge[u]; 
  if(e!=-1)
  {
    int w = -1;
    int dst = -1;
    if(e<d_offset[V])
    {
      w = d_edgeLen[e];
      dst = d_edgeList[e];
    }
    else
    {
      w = d_diff_edgeLen[e-d_offset[V]];
      dst = d_diff_edgeList[e-d_offset[V]];
    }
    int minval = min(dst,u);
    int maxval = max(dst,u);
    if(minEdgeW < w || d_minEdgeOfCompMin[comp]!= minval) return;
    atomicCAS(&d_minEdgeOfCompMax[comp],-1,maxval);
    atomicMin(&d_minEdgeOfCompMax[comp],maxval);
  }
} // end KER FUNC
__global__ void Boruvka_kernel_3_4(int V, int* d_offset, int* d_edgeLen,int *d_edgeList, int* d_diff_offset, int* d_diff_edgeLen,int *d_diff_edgeList, int* d_minEdge,int* d_minEdgeOfComp,int* d_color, int *d_minEdgeOfCompW,int *d_minEdgeOfCompMin, int *d_minEdgeOfCompMax){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int comp = d_color[u]; 
  int minEdgeW = d_minEdgeOfCompW[comp]; 
  int e = d_minEdge[u]; 
  if(e!=-1)
  {
    int w = -1;
    int dst = -1;
    if(e<d_offset[V])
    {
      w = d_edgeLen[e];
      dst = d_edgeList[e];
    }
    else
    {
      w = d_diff_edgeLen[e-d_offset[V]];
      dst = d_diff_edgeList[e-d_offset[V]];
    }
    int minval = min(dst,u);
    int maxval = max(dst,u);
    if(minEdgeW < w || d_minEdgeOfCompMin[comp]!= minval || d_minEdgeOfCompMax[comp]!= maxval ) return;
    d_minEdgeOfComp[comp] = e;
  }
} 

__global__ void Boruvka_kernel_4(int V, int* d_offset,int *d_edgeList,int *d_edgeLen,int *d_diff_offset,int *d_diff_edgeList,int *d_diff_edgeLen,int* d_minEdgeOfComp,int* d_color)
{
  int src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == src){ 
    int srcMinEdge = d_minEdgeOfComp[src];
    if (srcMinEdge != -1){ 
      int dst = -1;
      if(srcMinEdge<d_offset[V])
      {
        dst = d_edgeList[srcMinEdge];
      } else {
        dst = d_diff_edgeList[srcMinEdge-d_offset[V]];
      }
      int dstLead = d_color[dst];
      int dstMinEdge = d_minEdgeOfComp[dstLead];
      if (dstMinEdge != -1){ 
        int dstOfDst = -1;
        if(dstMinEdge<d_offset[V]) {
          dstOfDst = d_edgeList[dstMinEdge];
        } else {
          dstOfDst = d_diff_edgeList[dstMinEdge-d_offset[V]];
        }
        int dstOfDstLead = d_color[dstOfDst];
        if (d_color[src] == d_color[dstOfDstLead] && d_color[src] < d_color[dstLead]){ 
          d_minEdgeOfComp[src] = -1;
        } 
      }
    } 
  } 
}

__global__ void Boruvka_kernel_5(int V,int * d_minEdgeOfComp,int *d_offset,int* d_color,bool *d_isMSTEdge,bool *d_diff_isMSTEdge){ // BEGIN KER FUN via ADDKERNEL
  int src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == src){  
    int srcMinEdge = d_minEdgeOfComp[src];
    if (srcMinEdge != -1){ 
      if(srcMinEdge<d_offset[V]){
        d_isMSTEdge[srcMinEdge] = true;
      } else {
        d_diff_isMSTEdge[srcMinEdge-d_offset[V]] = true;
      }
    }
  } 
} 

__global__ void Boruvka_kernel_6(int V,  int* d_offset,int *d_edgeList,int *d_diff_edgeList,int* d_minEdgeOfComp,int* d_color)
{ 
  int src = blockIdx.x * blockDim.x + threadIdx.x;
  if(src >= V) return;
  if (d_color[src] == src){ 
    int srcMinEdge = d_minEdgeOfComp[src];
    if (srcMinEdge != -1){ 
      noNewComp = false;
      int dst = -1;
      if(srcMinEdge<d_offset[V]){
        dst = d_edgeList[srcMinEdge];
      } else {
        dst = d_diff_edgeList[srcMinEdge-d_offset[V]];
      }
      d_color[src] = d_color[dst];
    } 
  }
}
__device__ bool finished ; 

__global__ void Boruvka_kernel_7(int V,int* d_color){ 
  float num_nodes  = V;
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int my_color = d_color[u]; 
  int other_color = d_color[my_color]; 
  if (my_color != other_color){  
    finished = false;
    d_color[u] = other_color;
  }
} 
#endif