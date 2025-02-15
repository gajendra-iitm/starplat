// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "push_relabel_check.h"

void relabel(graph& g,int u,int* residue,int* label,
  thrust::host_vector<int> count)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();







  // CSR END
  //LAUNCH CONFIG
  const unsigned threadsPerBlock = 512;
  unsigned numThreads   = (V < threadsPerBlock)? 512: V;
  unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


  // TIMER START
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float milliseconds = 0;
  cudaEventRecord(start,0);


  //DECLAR DEVICE AND HOST vars in params
  int* d_residue;
  cudaMalloc(&d_residue, sizeof(int)*(E));

  int* d_label;
  cudaMalloc(&d_label, sizeof(int)*(V));


  //BEGIN DSL PARSING 
  int x = label[u]; // asst in .cu

  int new_label = g.num_nodes() + 2; // asst in .cu

  int residual_capacity = edge;
  if (residue[residual_capacity] > 0){ // if filter begin 
    if (new_label < v){ // if filter begin 
      new_label = v;

    } // if filter end

  } // if filter end

} //  end FOR NBR ITR. TMP FIX!
if (new_label < g.num_nodes() + 1 && new_label + 1 > x){ // if filter begin 
  d_label[u] = new_label + 1;

} // if filter end

//TIMER STOP
cudaEventRecord(stop,0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&milliseconds, start, stop);
printf("GPU Time: %.6f ms\n", milliseconds);

cudaMemcpy( residue, d_residue, sizeof(int)*(E), cudaMemcpyDeviceToHost);
cudaMemcpy(   label,  d_label, sizeof(int)*(V), cudaMemcpyDeviceToHost);
} //end FUN
void fixGap(thrust::host_vector<int> count,int* label)

{
// CSR BEGIN
int V = label.num_nodes();
int E = label.num_edges();

printf("#nodes:%d\n",V);
printf("#edges:%d\n",E);
int* edgeLen = label.getEdgeLen();







// CSR END
//LAUNCH CONFIG
const unsigned threadsPerBlock = 512;
unsigned numThreads   = (V < threadsPerBlock)? 512: V;
unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;


// TIMER START
cudaEvent_t start, stop;
cudaEventCreate(&start);
cudaEventCreate(&stop);
float milliseconds = 0;
cudaEventRecord(start,0);


//DECLAR DEVICE AND HOST vars in params
int* d_label;
cudaMalloc(&d_label, sizeof(int)*(V));


//BEGIN DSL PARSING 
int gap = count.getIdx(0); // asst in .cu

if (gap == -1){ // if filter begin 
} // if filter end
cudaMemcpyToSymbol(::gap, &gap, sizeof(int), 0, cudaMemcpyHostToDevice);
fixGap_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_label);
cudaDeviceSynchronize();
cudaMemcpyFromSymbol(&gap, ::gap, sizeof(int), 0, cudaMemcpyDeviceToHost);



()
//TIMER STOP
cudaEventRecord(stop,0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&milliseconds, start, stop);
printf("GPU Time: %.6f ms\n", milliseconds);

cudaMemcpy(   label,  d_label, sizeof(int)*(V), cudaMemcpyDeviceToHost);
} //end FUN
