//header flies
// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#ifndef GENCPP_BC_DSLV2_H
#define GENCPP_BC_DSLV2_H
#include <cooperative_groups.h>
#include <vector>
#include <chrono>
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../graph.hpp"
//#include "../libcuda.cuh"
#include "../dynamic_mst_delete_cuda/libcuda.cuh"

//static bc kernels
//--------------------------------------------------------------------------------------------//
__global__ void fwd_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,float* d_BC) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  if(d_level[v] == *d_hops_from_source) {
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == -1) {
        d_level[w] = *d_hops_from_source + 1;
        *d_finished = false;
      }
      if(d_level[w] == *d_hops_from_source + 1) {
        d_sigma[w] = d_sigma[w]+ d_sigma[v];

      }
    }
  } // end if d lvl
} // kernel end

__global__ void back_pass(int n, int* d_meta,int* d_data,int* d_weight, float* d_delta, float* d_sigma, int* d_level, int* d_hops_from_source, bool* d_finished,float* d_BC) {
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= n) return;
  auto grid = cooperative_groups::this_grid();
  if(d_level[v] == *d_hops_from_source-1) {
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if(d_level[w] == *d_hops_from_source) {
        atomicAdd(&d_delta[v],  (d_sigma[v] / d_sigma[w]) * (1 + d_delta[w]));

      } // end IF  
    } // end FOR
  } // end if d lvl
} // kernel end

__global__ void Compute_BC_kernel(int V, int E,float* d_delta,float* d_BC){ // BEGIN KER FUN via ADDKERNEL
  float num_nodes  = V;
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if(v >= V) return;
  d_BC[v] = d_BC[v] + d_delta[v];
} // end KER FUNC

#endif


//here computeBcDyn() fn that makes use of above static bc kernels
//compute_bc uses static bc kernels
//--------------------------------------------------------------------------------------------//
void Compute_BC(graph& g,float* BC,std::set<int>& sourceSet)

{
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_weight;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp;
    temp = g.indexofNodes[i];
    h_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp;
    temp = g.edgeList[i];
    h_data[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_weight;

  cudaMalloc(&d_meta, sizeof(int)*(1+V));
  cudaMalloc(&d_data, sizeof(int)*(E));
  cudaMalloc(&d_weight, sizeof(int)*(E));

  cudaMemcpy(  d_meta,   h_meta, sizeof(int)*(V+1), cudaMemcpyHostToDevice);
  cudaMemcpy(  d_data,   h_data, sizeof(int)*(E), cudaMemcpyHostToDevice);
  cudaMemcpy(d_weight, h_weight, sizeof(int)*(E), cudaMemcpyHostToDevice);

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
  float* d_BC;
  cudaMalloc(&d_BC, sizeof(float)*(V));


  //BEGIN DSL PARSING 
  initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_BC,(float)0);

  float* d_sigma;
  cudaMalloc(&d_sigma, sizeof(float)*(V));

  float* d_delta;
  cudaMalloc(&d_delta, sizeof(float)*(V));

  //FOR SIGNATURE of SET - Assumes set for on .cu only
  std::set<int>::iterator itr;
  for(itr=sourceSet.begin();itr!=sourceSet.end();itr++) 
  {
    int src = *itr;
    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_delta,(float)0);

    initKernel<float> <<<numBlocks,threadsPerBlock>>>(V,d_sigma,(float)0);

    initIndex<float><<<1,1>>>(V,d_sigma,src,(float)1); //InitIndexDevice

    //EXTRA vars for ITBFS AND REVBFS
    bool finished;
    int hops_from_source=0;
    bool* d_finished;       cudaMalloc(&d_finished,sizeof(bool) *(1));
    int* d_hops_from_source;cudaMalloc(&d_hops_from_source, sizeof(int));  cudaMemset(d_hops_from_source,0,sizeof(int));
    int* d_level;           cudaMalloc(&d_level,sizeof(int) *(V));

    //EXTRA vars INITIALIZATION
    initKernel<int> <<<numBlocks,threadsPerBlock>>>(V,d_level,-1);
    initIndex<int><<<1,1>>>(V,d_level,src, 0);

    // long k =0 ;// For DEBUG
    do {
      finished = true;
      cudaMemcpy(d_finished, &finished, sizeof(bool)*(1), cudaMemcpyHostToDevice);

      //Kernel LAUNCH
      fwd_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data,d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished,d_BC); ///DONE from varList

      incrementDeviceVar<<<1,1>>>(d_hops_from_source);
      cudaDeviceSynchronize(); //MUST - rupesh
      ++hops_from_source; // updating the level to process in the next iteration
      // k++; //DEBUG

      cudaMemcpy(&finished, d_finished, sizeof(bool)*(1), cudaMemcpyDeviceToHost);
    }while(!finished);

    hops_from_source--;
    cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);

    //BACKWARD PASS
    while(hops_from_source > 1) {

      //KERNEL Launch
      back_pass<<<numBlocks,threadsPerBlock>>>(V, d_meta, d_data, d_weight, d_delta, d_sigma, d_level, d_hops_from_source, d_finished
        ,d_BC); ///DONE from varList

      hops_from_source--;
      cudaMemcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1), cudaMemcpyHostToDevice);
    }
    //accumulate_bc<<<numBlocks,threadsPerBlock>>>(V,d_delta, d_BC, d_level, src);
    Compute_BC_kernel<<<numBlocks, threadsPerBlock>>>(V,E,d_delta,d_BC);
    cudaDeviceSynchronize();




  }

  //cudaFree up!! all propVars in this BLOCK!
  cudaFree(d_delta);
  cudaFree(d_sigma);

  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

  cudaMemcpy(      BC,     d_BC, sizeof(float)*(V), cudaMemcpyDeviceToHost);
} //end FUN



// main fn () bc_dslV2mainCuda
//--------------------------------------------------------------------------------------------//
// main fn: reads Input and Calls `Compute_BC`
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> <source_nodes_file>" << std::endl;
        return 1;
    }

    char* graphFilePath = argv[1];
    char* sourceNodesFilePath = argv[2];

    // Load graph
    graph g(graphFilePath);
    g.parseGraph();

    std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;

    // Read source nodes from file
    std::set<int> sourceSet;
    std::ifstream sourceFile(sourceNodesFilePath);
    if (!sourceFile) {
        std::cerr << "Error: Unable to open source nodes file: " << sourceNodesFilePath << std::endl;
        return 1;
    }

    int node;
    while (sourceFile >> node) {
        if (node >= 0 && node < g.num_nodes()) {
            sourceSet.insert(node);
        }
    }
    sourceFile.close();

    std::cout << "Source nodes loaded: ";
    for (int src : sourceSet) {
      std::cout << src << " ";
    }
    std::cout << std::endl;



    float* BC = new float[g.num_nodes()];
    std::fill_n(BC, g.num_nodes(), 0.0f);//to set the array BC values to 0

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

//to list out all the edges of each node in the graph.
    for (int i = 0; i < g.num_nodes(); ++i) {
      std::cout << "Node " << i << " has edges: ";
        for (int j = g.indexofNodes[i]; j < g.indexofNodes[i + 1]; j++) {
          std::cout << g.edgeList[j] << " ";
        }
      std::cout << std::endl;//new line after listing out all the edges of the node i.
    }

    cudaEventRecord(start, 0);
    printf("Calling Compute_BC now.\n");
    Compute_BC(g, BC, sourceSet);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    std::cout << "\nGPU Time: " << milliseconds / 1000.0 << " seconds" << std::endl;

    for (int i = 0; i < g.num_nodes(); ++i) {
        std::cout << "Node " << i << " BC: " << BC[i] << std::endl;
    }

    delete[] BC;
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    return 0;
}





//--------------------------------------------------------------------------------------------//
//input graph handling-- most probably thru main fn only
//input updates incremental, decremental-- same file or different files? what does these update files has
//using sssp dyn main () fn code as reference
//main() fn
//--------------------------------------------------------------------------------------------//
// int main(int argc, char **argv)
// {
//   //take input graph file
//   graph G("../inputGraphs/inputs/pokec.txt", "cuda", true);
//   G.parseGraph();

//   int *dist = new int[G.num_nodes()];
//   int *parent = new int[G.num_nodes()];
//   int *edgeLen = G.getEdgeLen();
//   //compute_bc here
//   //Compute_SSSP(G, dist, parent, edgeLen, 0);

//   //boolean array to keep track of added and deleted edges (through nodes)  
//   bool *modified_add = new bool[G.num_nodes()];
//   bool *modified_del = new bool[G.num_nodes()];
//   int elements = 0;

//   std::vector<update> updateEdges = G.parseUpdates("../inputGraphs/updates/update_pokec.txt");

//   int percent = atoi(argv[1])
//   int updateSize = percent * 0.01 * G.num_edges();
//   if (!G.isGraphDirected() && updateSize & 1)
//     updateSize++;
//   int batchSize = updateSize;
//   // int batchSize = 1000000;

//   std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
//   std::chrono::duration<double, std::milli> timeTaken;

//   startTime = std::chrono::high_resolution_clock::now();
//   for (int k = 0; k < updateSize; k += batchSize)
//   {
//     if ((k + batchSize) > updateSize)
//       elements = updateSize - k;
//     else
//       elements = batchSize;

//     for (int i = 0; i < G.num_nodes(); i++)
//     {
//       modified_add[i] = false;
//       modified_del[i] = false;
//     }

//     for (int i = k; i < (k + batchSize) && i < updateSize; i++)
//     {
//       if (updateEdges[i].type == 'd')
//       {
//         update u = updateEdges[i];
//         int src = u.source;
//         int dest = u.destination;

//         if (parent[dest] >= 0 && parent[dest] == src)
//         {
//           dist[dest] = INT_MAX;
//           parent[dest] = -1;
//           modified_del[dest] = true;
//         }
//       }
//     }

//     //update graph CSR with updated deleted edges
//     G.updateCSR_Del(updateEdges, k, elements);
//     //compute_dynBcDel() here
//     Compute_dynamicSSSPDel(G, dist, parent, modified_del);

//     for (int i = k; i < (k + batchSize) && i < updateSize; i++)
//     {
//       if (updateEdges[i].type == 'a')
//       {
//         update u = updateEdges[i];
//         int src = u.source;
//         int dest = u.destination;
//         int weight = u.weight;
//         if (dist[src] != INT_MAX && dist[src] + weight < dist[dest])
//           modified_add[src] = true;
//       }
//     }

//     //update graph CSR with added edges
//     G.updateCSR_Add(updateEdges, k, elements);
//     //compute_dynBcAdd()
//     Compute_dynamicSSSPAdd(G, dist, parent, modified_add);
//   }
//   endTime = std::chrono::high_resolution_clock::now();
//   timeTaken = endTime - startTime;
//   printf("Total time taken for %d percent updates: %.3f ms\n", percent, timeTaken.count());

//   unsigned long long sum = 0;
//   for (int i = 0; i < G.num_nodes(); i++)
//   {
//     if (dist[i] != INT_MAX)
//       sum += dist[i];
//   }
//   printf("Final graph, total path sum = %llu\n\n", sum);

//   free(dist);
//   free(parent);
//   free(modified_add);
//   free(modified_del);

//   return 0;
// }