#include <cuda.h>
#include <curand_kernel.h>

template <typename T>
__global__ void initKernel0(T* init_array, T id, T init_value) {  // MOSTLY 1 thread kernel inits one type value at index id
  init_array[id] = init_value;
}

template <typename T>
__global__ void initKernel(unsigned V, T* init_array, T init_value) {  // intializes one 1D array with init val
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array[id] = init_value;
  }
}

template <typename T1, typename T2>
__global__ void initKernel2(unsigned V, T1* init_array1, T1 init_value1, T2* init_array2, T2 init_value2) {  // intializes two 1D array may be of two types
  unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (id < V) {
    init_array1[id] = init_value1;
    init_array2[id] = init_value2;
  }
}

//NOT USED
__global__ void accumulate_bc(unsigned n, double* d_delta, double* d_nodeBC, int* d_level, unsigned s) {
  unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid >= n || tid == s || d_level[tid] == -1) return;
  d_nodeBC[tid] += d_delta[tid] / 2.0;
}

//~ initIndex<<<1.1>>>(v,arr,s,val);
template <typename T>
__global__ void initIndex(int V, T* init_array, int s, T init_value) {  // intializes an index 1D array with init val
  //~ unsigned id = threadIdx.x + blockDim.x * blockIdx.x;
  if (s < V) {  // bound check
    init_array[s] = init_value;
  }
}

// Single thread kernels. Basically i++ or i-- on device
template <typename T>
__global__ void incrementDeviceVar(T* d_var) {
  *d_var = *d_var + 1;
}
template <typename T>
__global__ void decrementDeviceVar(T* d_var) {
  *d_var = *d_var - 1;
}

__device__ void shuffleNeighbors(int *d_data, int *d_meta, int V)
{
  curandState state;
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  unsigned long long seed = 42;
  curand_init(seed, idx, 0, &state);
  for(int i = 0; i<V; i++)
  {
    int start = d_meta[i];
    int end = d_meta[i+1]-1;
    int range = end - start + 1;
    for(int j = start; j<=end; j++)
    {
      int r = curand_uniform(&state);
      int temp = d_data[j];
      d_data[j] = d_data[start + r];
      d_data[start + r] = temp;
    }
  }
}

__host__ void getCUDAMST(int* h_meta, int *h_data, int *h_weight, int *h_mst_meta, int *h_mst_data, int* h_mst_weight, int V){
  std::vector<std::vector<std::pair<int, int>>> adj(V);
  for (int i = 0; i < V; i++) {
    int start = h_meta[i];
    int end = h_meta[i + 1] - 1;
    for (int j = start; j <= end; j++) {
      adj[i].push_back({h_data[j], h_weight[j]});
    }
  }
  // Prim's Algorithm for Minimum Spanning Tree (MST)
  std::vector<bool> inMST(V, false);
  std::vector<int> key(V, INT_MAX);
  std::vector<int> parent(V, -1);

  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
  key[0] = 0;
  pq.push({0, 0}); // {key, vertex}

  while (!pq.empty()) {
    int u = pq.top().second;
    pq.pop();

    if (inMST[u]) continue;
    inMST[u] = true;

    for (auto &[v, weight] : adj[u]) {
      if (!inMST[v] && weight < key[v]) {
        key[v] = weight;
        parent[v] = u;
        pq.push({key[v], v});
      }
    }
  }
  // Store the MST in h_mst_meta, h_mst_data, and h_mst_weight
  std::vector<std::vector<std::pair<int, int>>> mst_adj(V); // {neighbor, weight}
  for (int i = 1; i < V; i++) {
    if (parent[i] != -1) {
      mst_adj[parent[i]].push_back({i, key[i]});
      mst_adj[i].push_back({parent[i], key[i]});
    }
  }

  int edge_count = 0;
  h_mst_meta[0] = 0;
  for (int i = 0; i < V; i++) {
    edge_count += mst_adj[i].size();
    h_mst_meta[i + 1] = edge_count;
  }

  int weight_index = 0;
  for (int i = 0; i < V; i++) {
    for (auto &[neighbor, weight] : mst_adj[i]) {
      h_mst_data[h_mst_meta[i]++] = neighbor;
      h_mst_weight[weight_index++] = weight;
    }
  }

  // Reset h_mst_meta to correct offsets
  for (int i = V; i > 0; i--) {
    h_mst_meta[i] = h_mst_meta[i - 1];
  }
  h_mst_meta[0] = 0;
  // Print the MST using h_mst_meta, h_mst_data, and h_mst_weight
  for (int i = 0; i < V; i++) {
    int start = h_mst_meta[i];
    int end = h_mst_meta[i + 1];
    for (int j = start; j < end; j++) {
    }
  }
}

//GPU specific check for neighbours for TC algorithm
__device__ bool findNeighborSorted(int s, int d, int *d_meta, int *d_data)  //we can move this to graph.hpp file
{
  int startEdge = d_meta[s];
  int endEdge = d_meta[s + 1] - 1;

  if (d_data[startEdge] == d)
    return true;
  if (d_data[endEdge] == d)
    return true;

  int mid = (startEdge + endEdge) / 2;

  while (startEdge <= endEdge) {
    if (d_data[mid] == d)
      return true;

    if (d < d_data[mid])
      endEdge = mid - 1;
    else
      startEdge = mid + 1;

    mid = (startEdge + endEdge) / 2;
  }

  return false;
}


#define cudaCheckError()                                                               \
  {                                                                                    \
    cudaError_t e = cudaGetLastError();                                                \
    if (e != cudaSuccess) {                                                            \
      printf("Cuda failure %s:%d: '%s'\n", __FILE__, __LINE__, cudaGetErrorString(e)); \
      exit(0);                                                                         \
    }                                                                                  \
  }
