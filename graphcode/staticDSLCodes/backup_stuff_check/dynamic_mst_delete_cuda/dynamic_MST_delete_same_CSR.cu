#include "graph.hpp"
#include "MST.h"
#include<bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include<bits/stdc++.h>
using namespace std;

#define YETTOBEPROCESSED 0
#define FINISHEDPROCESSING 1
#define ADDANDCOLOUR 2
#define ADDEDGE 3
#define THREADS_PER_BLOCK 1024

__device__ bool colourfinish;
__device__ bool colourfinish2;

void checkCudaError( int  i)
{       
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess)  
  {   
    printf("%d , CUDA error: %s\n", i, cudaGetErrorString(error));
  } 
} 




__global__ void update_global(int * offset, int * edgeList, int * diff_offset, int * diff_edgeList, int * component, int  n,bool *d_isMSTEdge, bool *d_diff_isMSTEdge){ 
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= n) return;
  int my_color = component[u]; 
  int  new_component = my_color;
  for( int  i=offset[u];i<offset[u+1];i++)
  { 
    if(!d_isMSTEdge[i] ) continue;
    int v = edgeList[i];
    new_component = min(new_component,component[v]);
  }
  for( int  i=diff_offset[u];i<diff_offset[u+1];i++)
  { 
    if(!d_diff_isMSTEdge[i] ) continue;
     int  v=diff_edgeList[i];
    new_component = min(new_component,component[v]);
  }
  if(new_component<component[my_color]){
    component[my_color] = new_component;
    colourfinish = false;
  }
  for( int  i=offset[u];i<offset[u+1];i++)
  { 
    if(!d_isMSTEdge[i] ) continue;
    int v = edgeList[i];
    if(new_component<component[v]){
      component[component[v]] = new_component;colourfinish = false;
    }
  }
  for( int  i=diff_offset[u];i<diff_offset[u+1];i++)
  { 
    if(!d_diff_isMSTEdge[i] ) continue;
     int  v=diff_edgeList[i];
      if(new_component<component[v]){
      component[component[v]] = new_component;colourfinish = false;
    }
  }
} 

__global__ void settle(int V,int* d_color){ 
  int u = blockIdx.x * blockDim.x + threadIdx.x;
  if(u >= V) return;
  int my_color = d_color[u]; 
  int other_color = d_color[my_color]; 
  if (my_color != other_color){  
    colourfinish2 = false;
    d_color[u] = other_color;
  }
} 

void colourinitialkernel2( int * offset, int * edgeList, int * diff_offset, int * diff_edgeList, int * component, int  n,bool * d_isMSTEdge, bool *d_diff_isMSTEdge)
{
  
  bool finished = false;
  bool finished2 = false;
   int  numThreads=THREADS_PER_BLOCK;
   int  numBlocks=(n+numThreads-1)/numThreads;
  while(finished==false)
  {   
    finished = true;
    cudaMemcpyToSymbol(::colourfinish, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
    update_global<<<numBlocks,numThreads>>>(offset,edgeList,diff_offset,diff_edgeList,component,n,d_isMSTEdge,d_diff_isMSTEdge);
    cudaMemcpyFromSymbol( &finished,::colourfinish, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    finished2 = finished;
    while(finished2==false){
      finished2 = true;
      cudaMemcpyToSymbol(::colourfinish2, &finished2, sizeof(bool), 0, cudaMemcpyHostToDevice);
      settle<<<numBlocks,numThreads>>>(n,component);
      cudaMemcpyFromSymbol( &finished2,::colourfinish2, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    }
  }
}

__global__ void modifyMST_delete( int  x,  int  y, int  w, int * offset, int * edgeList, int * edgeLen, int * diff_offset, int * diff_edgeList, int * diff_edgeLen,bool *d_isMST, bool* d_diff_isMST)
{
  // printf("start\n");
  for( int  i=offset[x];i<offset[x+1];i++)
  {
    if(edgeList[i]==y && edgeLen[i]==w)
    {
      edgeList[i]=-1;
      edgeLen[i]=-1;
      d_isMST[i] = false;
      break;
    }
  }
  for( int  i=diff_offset[x];i<diff_offset[x+1];i++)
  {
    if(diff_edgeList[i]==y && diff_edgeLen[i]==w)
    {
      diff_edgeList[i]=-1;
      diff_edgeLen[i]=-1;
      d_diff_isMST[i] =false;
      break;
    }
  }
  for( int  i=offset[y];i<offset[y+1];i++)
  {
    if(edgeList[i]==x && edgeLen[i]==w)
    {
      edgeList[i]=-1;
      edgeLen[i]=-1;
      d_isMST[i] = false;
      break;
    }
  }
  for( int  i=diff_offset[y];i<diff_offset[y+1];i++)
  {
    if(diff_edgeList[i]==x &&  diff_edgeLen[i]==w)
    {
      diff_edgeList[i]=-1;
      diff_edgeLen[i]=-1;
      d_diff_isMST[i] =false;
      break;
    }
  }
  // printf("np\n");
}


long long int Boruvka(int V,int E,int curr_csr_size,int curr_diff_size,int * d_offset,int * d_diff_offset,int * d_edgeList,int * d_diff_edgeList,int * d_edgeLen,int * d_diff_edgeLen,int *h_offset,int *h_edgeLen,int *h_diff_offset,int *h_diff_edgeLen)
{


bool* h_isMST;
bool* h_diff_isMST;
  


h_isMST = (bool *)malloc(sizeof(bool)*curr_csr_size); 
h_diff_isMST = (bool *) malloc(sizeof(bool)*curr_diff_size); 
memset(h_isMST,false,sizeof(h_isMST));
memset(h_diff_isMST,false,sizeof(h_diff_isMST));

bool* d_isMST;
bool* d_diff_isMST;
int * d_color;
  
  
cudaMalloc(&d_color, sizeof(int )*(V));
cudaMalloc(&d_isMST,curr_csr_size*sizeof(int));
cudaMalloc(&d_diff_isMST,curr_diff_size*sizeof(int));
cudaMemcpy(d_isMST,h_isMST,curr_csr_size*sizeof(bool),cudaMemcpyHostToDevice);
cudaMemcpy(d_diff_isMST,h_diff_isMST,curr_diff_size*sizeof(bool),cudaMemcpyHostToDevice);

  

const int threadsPerBlock = THREADS_PER_BLOCK;
int numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
int numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;

bool noNewComp = false; 
int * d_minEdgeOfComp;
int * d_minEdgeOfCompW;
int  *d_minEdgeOfCompMin;
int  *d_minEdgeOfCompMax;
int * d_minEdge;
cudaMalloc(&d_minEdgeOfComp, sizeof(int )*(V));
cudaMalloc(&d_minEdgeOfCompW, sizeof( int )*(V));
cudaMalloc(&d_minEdgeOfCompMin, sizeof( int )*(V));
cudaMalloc(&d_minEdgeOfCompMax, sizeof(int )*(V));
cudaMalloc(&d_minEdge, sizeof( int )*(V));

initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(curr_csr_size,d_isMST,(bool)false);
initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(curr_diff_size,d_diff_isMST,(bool)false);

cudaEvent_t start1, stop1;
cudaEventCreate(&start1);
cudaEventCreate(&stop1);
float milliseconds = 0;
cudaEventRecord(start1,0);

initKernel<int > <<<numBlocks,threadsPerBlock>>>(V,d_color,( int )-1);
Boruvka_kernel_1<<<numBlocks, threadsPerBlock>>>( V,d_color);
while(!noNewComp) {
  noNewComp = true;
  cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);
  Boruvka_kernel_2<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_diff_offset,d_edgeList,d_diff_edgeList,d_edgeLen,d_diff_edgeLen, d_minEdge,d_color);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompW,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMin,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMax,(int)-1);
  Boruvka_kernel_3_1<<<numBlocks,threadsPerBlock>>>(V,d_offset,d_edgeLen,d_diff_edgeLen,d_minEdge,d_color,d_minEdgeOfCompW);
  Boruvka_kernel_3_2<<<numBlocks,threadsPerBlock>>>(V,d_offset, d_edgeList,d_edgeLen,d_diff_edgeList,d_diff_edgeLen, d_minEdge,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin);
  Boruvka_kernel_3_3<<<numBlocks,threadsPerBlock>>>(V,d_edgeList,d_edgeLen,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax,d_offset);
  Boruvka_kernel_3_4<<<numBlocks,threadsPerBlock>>>(V, d_offset,d_edgeLen,d_edgeList,d_diff_offset,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax);
  cudaDeviceSynchronize();
  Boruvka_kernel_4<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_edgeLen,d_diff_offset,d_diff_edgeList,d_diff_edgeLen,d_minEdgeOfComp,d_color);
  cudaDeviceSynchronize();

  Boruvka_kernel_5<<<numBlocks, threadsPerBlock>>>(V,d_minEdgeOfComp,d_offset,d_color,d_isMST,d_diff_isMST);
  cudaDeviceSynchronize();
  
  cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
  
  Boruvka_kernel_6<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_diff_edgeList,d_minEdgeOfComp,d_color);
  cudaDeviceSynchronize();
 
  cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  bool finished = false; 
  while(!finished) {
    finished = true;
    cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Boruvka_kernel_7<<<numBlocks, threadsPerBlock>>>(V,d_color);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  }
  checkCudaError(-10);
    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  } 
  cudaDeviceSynchronize();
  cudaEventRecord(stop1,0); cudaEventSynchronize(stop1); cudaEventElapsedTime(&milliseconds, start1, stop1);
  printf("Static MST GPU Time: %.6f ms\n", milliseconds);
  cudaMemcpy(h_isMST, d_isMST, curr_csr_size * sizeof(bool), cudaMemcpyDeviceToHost);
  cudaMemcpy(h_diff_isMST, d_diff_isMST, curr_diff_size * sizeof(bool), cudaMemcpyDeviceToHost);
  long long int  mst = 0;
  int  counter = 0;
  for( int  i=0;i<V;i++)
  {
    for(int  j=h_offset[i];j<h_offset[i+1];j++)
    {
      if(h_isMST[j]==true)
      {
        mst += h_edgeLen[j]; 
        counter++;
      }
    }
    for(int  j=h_diff_offset[i];j<h_diff_offset[i+1];j++)
    {
      if(h_diff_isMST[j]==true)
      {
        mst += h_diff_edgeLen[j]; 
        counter++;
      }
    }
  }
  printf("MST Weight: %lld counter:%d \n", mst,counter);
  return mst;
} 





long long int Recalculate(graph& g, char *updatesinp)
{
int  V = g.num_nodes();
int  E = g.num_edges();
int  curr_csr_size=g.num_edges_CSR();
int  curr_diff_size=g.num_edges_diffCSR();
  
int * h_offset;
int * h_diff_offset;
int * h_edgeList;
int * h_diff_edgeList;
int * h_edgeLen;
int * h_diff_edgeLen;
bool* h_isMST;
bool* h_diff_isMST;
  
h_offset=g.indexofNodes;
h_edgeList=g.edgeList;
h_edgeLen=g.getEdgeLen();
h_diff_offset=g.diff_indexofNodes;
h_diff_edgeList=g.diff_edgeList;
h_diff_edgeLen=g.getDiff_edgeLen();


h_isMST = (bool *)malloc(sizeof(bool)*curr_csr_size); 
h_diff_isMST = (bool *) malloc(sizeof(bool)*curr_diff_size); 
memset(h_isMST,false,sizeof(h_isMST));
memset(h_diff_isMST,false,sizeof(h_diff_isMST));

int * d_offset;
int * d_diff_offset;
int * d_edgeList;
int * d_diff_edgeList;
int * d_edgeLen;
int * d_diff_edgeLen;
bool* d_isMST;
bool* d_diff_isMST;
int * d_color;
  
  
cudaMalloc(&d_color, sizeof(int )*(V));
cudaMalloc(&d_offset,(V+1)*sizeof(int));
cudaMalloc(&d_diff_offset,(V+1)*sizeof(int));
cudaMalloc(&d_edgeList,curr_csr_size*sizeof(int));
cudaMalloc(&d_edgeLen,curr_csr_size*sizeof(int));
cudaMalloc(&d_isMST,curr_csr_size*sizeof(int));
cudaMalloc(&d_diff_edgeList,curr_diff_size*sizeof(int));
cudaMalloc(&d_diff_edgeLen,curr_diff_size*sizeof(int));
cudaMalloc(&d_diff_isMST,curr_diff_size*sizeof(int));

cudaMemcpy(d_offset,h_offset,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_diff_offset,h_diff_offset,(V+1)*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_edgeList,h_edgeList,curr_csr_size*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_edgeLen,h_edgeLen,curr_csr_size*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_isMST,h_isMST,curr_csr_size*sizeof(bool),cudaMemcpyHostToDevice);
cudaMemcpy(d_diff_edgeList,h_diff_edgeList,curr_diff_size*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_diff_edgeLen,h_diff_edgeLen,curr_diff_size*sizeof(int),cudaMemcpyHostToDevice);
cudaMemcpy(d_diff_isMST,h_diff_isMST,curr_diff_size*sizeof(bool),cudaMemcpyHostToDevice);
    
const int threadsPerBlock = THREADS_PER_BLOCK;
int numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;
int numBlocks_Edge    = (E+threadsPerBlock-1)/threadsPerBlock;

bool noNewComp = false; 
int * d_minEdgeOfComp;
int * d_minEdgeOfCompW;
int  *d_minEdgeOfCompMin;
int  *d_minEdgeOfCompMax;
int * d_minEdge;
cudaMalloc(&d_minEdgeOfComp, sizeof(int )*(V));
cudaMalloc(&d_minEdgeOfCompW, sizeof( int )*(V));
cudaMalloc(&d_minEdgeOfCompMin, sizeof( int )*(V));
cudaMalloc(&d_minEdgeOfCompMax, sizeof(int )*(V));
cudaMalloc(&d_minEdge, sizeof( int )*(V));

initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(curr_csr_size,d_isMST,(bool)false);
initKernel<bool> <<<numBlocks_Edge,threadsPerBlock>>>(curr_diff_size,d_diff_isMST,(bool)false);

cudaEvent_t start1, stop1;
cudaEventCreate(&start1);
cudaEventCreate(&stop1);
float milliseconds = 0;
cudaEventRecord(start1,0);

initKernel<int > <<<numBlocks,threadsPerBlock>>>(V,d_color,( int )-1);
Boruvka_kernel_1<<<numBlocks, threadsPerBlock>>>( V,d_color);
while(!noNewComp) {
  noNewComp = true;
  cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);
  Boruvka_kernel_2<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_diff_offset,d_edgeList,d_diff_edgeList,d_edgeLen,d_diff_edgeLen, d_minEdge,d_color);
  cudaDeviceSynchronize();
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompW,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMin,(int)-1);
  initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMax,(int)-1);

  Boruvka_kernel_3_1<<<numBlocks,threadsPerBlock>>>(V,d_offset,d_edgeLen,d_diff_edgeLen,d_minEdge,d_color,d_minEdgeOfCompW);
  Boruvka_kernel_3_2<<<numBlocks,threadsPerBlock>>>(V,d_offset, d_edgeList,d_edgeLen,d_diff_edgeList,d_diff_edgeLen, d_minEdge,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin);
  Boruvka_kernel_3_3<<<numBlocks,threadsPerBlock>>>(V,d_edgeList,d_edgeLen,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax,d_offset);
  Boruvka_kernel_3_4<<<numBlocks,threadsPerBlock>>>(V, d_offset,d_edgeLen,d_edgeList,d_diff_offset,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax);
  cudaDeviceSynchronize();

  Boruvka_kernel_4<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_edgeLen,d_diff_offset,d_diff_edgeList,d_diff_edgeLen,d_minEdgeOfComp,d_color);
  cudaDeviceSynchronize();

  Boruvka_kernel_5<<<numBlocks, threadsPerBlock>>>(V,d_minEdgeOfComp,d_offset,d_color,d_isMST,d_diff_isMST);
  cudaDeviceSynchronize();
  
  cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
  
  Boruvka_kernel_6<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_diff_edgeList,d_minEdgeOfComp,d_color);
  cudaDeviceSynchronize();
 
  cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);

  bool finished = false; 
  while(!finished) {
    finished = true;
    cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
    Boruvka_kernel_7<<<numBlocks, threadsPerBlock>>>(V,d_color);
    cudaDeviceSynchronize();
    cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  } 

    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  } 
  cudaDeviceSynchronize();
  cudaEventRecord(stop1,0); cudaEventSynchronize(stop1); cudaEventElapsedTime(&milliseconds, start1, stop1);
  printf("Initial MST GPU Time: %.6f ms\n", milliseconds);

  // bool* h_isMSTEdge = (bool *)malloc((E)*sizeof(bool));
  cudaMemcpy(h_isMST, d_isMST, curr_csr_size * sizeof(bool), cudaMemcpyDeviceToHost);
  cudaMemcpy(h_diff_isMST, d_diff_isMST, curr_diff_size * sizeof(bool), cudaMemcpyDeviceToHost);
  
  long long int  mst = 0;
  int  counter = 0;
  for( int  i=0;i<g.num_nodes();i++)
  {
    for(int  j=h_offset[i];j<h_offset[i+1];j++)
    {
      if(h_isMST[j]==true)
      {
        mst += h_edgeLen[j]; 
        counter++;
      }
    }
    for(int  j=h_diff_offset[i];j<h_diff_offset[i+1];j++)
    {
      if(h_diff_isMST[j]==true)
      {
        mst += h_diff_edgeLen[j]; 
        counter++;
      }
    }
  }
  printf("MST Weight: %lld counter:%d \n", mst,counter);
  std::vector<update> updateEdges=g.parseUpdates(updatesinp);
  for(auto &u:updateEdges){
    modifyMST_delete<<<1,1>>>(u.source,u.destination,u.weight,d_offset,d_edgeList,d_edgeLen,d_diff_offset,d_diff_edgeList,d_diff_edgeLen,d_isMST,d_diff_isMST);
  }
  cudaEvent_t start2, stop2;
  cudaEventCreate(&start2);
  cudaEventCreate(&stop2);
  float milliseconds2 = 0;
  cudaEventRecord(start2,0);
  Boruvka_kernel_1<<<numBlocks, threadsPerBlock>>>( V,d_color);
  colourinitialkernel2(d_offset,d_edgeList,d_diff_offset,d_diff_edgeList,d_color,V,d_isMST,d_diff_isMST);
  cudaDeviceSynchronize();

  cudaEventRecord(stop2,0); cudaEventSynchronize(stop2); cudaEventElapsedTime(&milliseconds2, start2, stop2);
  printf("Colour MST GPU Time: %.6f ms\n", milliseconds2);

  cudaEvent_t start3, stop3;
  cudaEventCreate(&start3);
  cudaEventCreate(&stop3);
  float milliseconds3 = 0;
  cudaEventRecord(start3,0);

  noNewComp = false;
  while(!noNewComp) {
    noNewComp = true;
    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdge,(int)-1);
    
    Boruvka_kernel_2<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_diff_offset,d_edgeList,d_diff_edgeList,d_edgeLen,d_diff_edgeLen, d_minEdge,d_color);
    cudaDeviceSynchronize();

    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfComp,(int)-1);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompW,(int)-1);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMin,(int)-1);
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_minEdgeOfCompMax,(int)-1);
    Boruvka_kernel_3_1<<<numBlocks,threadsPerBlock>>>(V,d_offset,d_edgeLen,d_diff_edgeLen,d_minEdge,d_color,d_minEdgeOfCompW);
    Boruvka_kernel_3_2<<<numBlocks,threadsPerBlock>>>(V,d_offset, d_edgeList,d_edgeLen,d_diff_edgeList,d_diff_edgeLen, d_minEdge,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin);
    Boruvka_kernel_3_3<<<numBlocks,threadsPerBlock>>>(V,d_edgeList,d_edgeLen,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax,d_offset);
    Boruvka_kernel_3_4<<<numBlocks,threadsPerBlock>>>(V, d_offset,d_edgeLen,d_edgeList,d_diff_offset,d_diff_edgeLen,d_diff_edgeList,d_minEdge,d_minEdgeOfComp,d_color,d_minEdgeOfCompW,d_minEdgeOfCompMin,d_minEdgeOfCompMax);
    cudaDeviceSynchronize();
    Boruvka_kernel_4<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_edgeLen,d_diff_offset,d_diff_edgeList,d_diff_edgeLen,d_minEdgeOfComp,d_color);
    cudaDeviceSynchronize();

    Boruvka_kernel_5<<<numBlocks, threadsPerBlock>>>(V,d_minEdgeOfComp,d_offset,d_color,d_isMST,d_diff_isMST);
    cudaDeviceSynchronize();

    cudaMemcpyToSymbol(::noNewComp, &noNewComp, sizeof(bool), 0, cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    Boruvka_kernel_6<<<numBlocks, threadsPerBlock>>>(V,d_offset,d_edgeList,d_diff_edgeList,d_minEdgeOfComp,d_color);
    cudaDeviceSynchronize();

    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    bool finished = false;
    checkCudaError(7);
    while(!finished) {
      finished = true;
      cudaMemcpyToSymbol(::finished, &finished, sizeof(bool), 0, cudaMemcpyHostToDevice);
      cudaDeviceSynchronize();

      Boruvka_kernel_7<<<numBlocks, threadsPerBlock>>>(V,d_color);
      cudaDeviceSynchronize();

      cudaMemcpyFromSymbol(&finished, ::finished, sizeof(bool), 0, cudaMemcpyDeviceToHost);
      cudaDeviceSynchronize();
    } 
    cudaMemcpyFromSymbol(&noNewComp, ::noNewComp, sizeof(bool), 0, cudaMemcpyDeviceToHost);
  } 
  cudaEventRecord(stop3,0); cudaEventSynchronize(stop3); cudaEventElapsedTime(&milliseconds3, start3, stop3);
  printf("Second MST GPU Time: %.6f ms\n", milliseconds3);

  cudaMemcpy(h_isMST, d_isMST, curr_csr_size * sizeof(bool), cudaMemcpyDeviceToHost);
  cudaMemcpy(h_diff_isMST, d_diff_isMST, curr_diff_size * sizeof(bool), cudaMemcpyDeviceToHost);
  checkCudaError(3);
  long long int  mstc = 0;
  counter = 0;
  for(int  i=0;i<g.num_nodes();i++)
  {
    for(int  j=h_offset[i];j<h_offset[i+1];j++)
    {
      if(h_isMST[j]==true)
      {
        mstc+=h_edgeLen[j];
        counter++;
      }
    }
    for( int  j=h_diff_offset[i];j<h_diff_offset[i+1];j++)
    {
      if(h_diff_isMST[j]==true)
      {
        mstc += h_diff_edgeLen[j]; 
        counter++;
      }
    }
  }
  checkCudaError(2);
  printf("Recalculated MST Weight: %lld counter:%d \n", mstc,counter);
   printf("Recalculate MST GPU Time: %.6f ms\n", milliseconds2+milliseconds3);
  long long int staticmst = Boruvka(V,E,curr_csr_size,curr_diff_size,d_offset,d_diff_offset,d_edgeList,d_diff_edgeList,d_edgeLen,d_diff_edgeLen,h_offset,h_edgeLen,h_diff_offset,h_diff_edgeLen);
  if(staticmst!=mstc){
    printf("=====================PANIC INCORRECT========================\n");
  } else {
    printf("=====================ANSWER MATCHES========================\n");
  }
  cudaFree(d_color);
  cudaFree(d_minEdge);
  cudaFree(d_minEdgeOfCompW);
  cudaFree(d_isMST);
  cudaFree(d_diff_isMST);
  cudaFree(d_offset);
  cudaFree(d_diff_offset);
  cudaFree(d_edgeList);
  cudaFree(d_edgeLen);
  cudaFree(d_diff_edgeList);
  cudaFree(d_diff_edgeLen);

  cudaFree(d_minEdgeOfComp);
  cudaFree(d_minEdgeOfCompMin);
  cudaFree(d_minEdgeOfCompMax);

  free(h_isMST);
  free(h_diff_isMST);
  checkCudaError(1);
  return mstc;

} 





int  main( int  argc, char** argv) {
  char* totalgraph=argv[1];
  char* updatesinp = argv[2];
  graph G1(totalgraph,"cuda",true);
  G1.parseGraph();
  Recalculate(G1,updatesinp);
  cudaDeviceSynchronize();
  return 0;
}