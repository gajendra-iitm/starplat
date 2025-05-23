#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <thrust/sort.h>
#include <thrust/device_vector.h>
#include <thrust/unique.h>  // Ensure this is included
#include <thrust/execution_policy.h>  // Needed for explicit policy
#include <cuda_runtime.h>

using namespace std;

// ============================
// EdgeTuple Structure
// ============================
struct EdgeTuple {
    int node_p, node_q;
    float weight;
};

// ============================
// EdgeComparator Structure
// ============================
// Define a struct for sorting edges
struct EdgeComparator {
    __host__ __device__
    bool operator()(const EdgeTuple &a, const EdgeTuple &b) const {
        return (a.node_p < b.node_p) || (a.node_p == b.node_p && a.node_q < b.node_q);
    }
};

// ============================
// Graph Structure (CSR Format)
// ============================
struct Graph {
    int num_nodes, num_edges;
    int *d_row_ptr, *d_col_idx;
    float *d_edge_weights;
    float *d_alpha, *d_beta;
    float d_m;
};

// ============================
// Partition Structure
// ============================
struct Partition
{
    int *d_community; // Community mapping for each node in G
};

// ============================
// Global Vertex-Community Map
// ============================
struct VertexCommunityMap
{
    int *d_vertex_map; // Maps original nodes to final community
    int size;
};

// ============================
// Parse Graph Function
// ============================

void parseGraph(const std::string &filename, Graph &G) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::tuple<int, int, float>> edges;
    int max_node = 0;
    int p, q;
    float w;

    // Read the file line by line
    while (infile >> p >> q >> w) {
        edges.emplace_back(p, q, w);
        max_node = std::max({max_node, p, q});
    }
    infile.close();

    G.num_nodes = max_node + 1; // Nodes are 0-based, so max_node + 1
    G.num_edges = edges.size();

    // Sort edges based on (source, destination) to help with CSR construction
    std::sort(edges.begin(), edges.end());

    // Allocate memory for CSR arrays
    cudaMallocManaged(&G.d_row_ptr, (G.num_nodes + 1) * sizeof(int));
    cudaMallocManaged(&G.d_col_idx, G.num_edges * sizeof(int));
    cudaMallocManaged(&G.d_edge_weights, G.num_edges * sizeof(float));

    std::vector<int> h_row_ptr(G.num_nodes + 1, 0);
    std::vector<int> h_col_idx(G.num_edges);
    std::vector<float> h_edge_weights(G.num_edges);

    // Fill col_idx and edge_weights, and compute row_ptr
    int current_node = 0;
    h_row_ptr[0] = 0;

    for (int i = 0; i < edges.size(); i++)
    {
        int u = std::get<0>(edges[i]);
        int v = std::get<1>(edges[i]);
        float weight = std::get<2>(edges[i]);

        while (current_node < u)
        {
            h_row_ptr[++current_node] = i;  // Mark row start
        }
        h_col_idx[i] = v;
        h_edge_weights[i] = weight;
    }

    while (current_node < G.num_nodes)
    {
        h_row_ptr[++current_node] = G.num_edges;
    }

    // Copy data to GPU memory
    cudaMemcpy(G.d_row_ptr, h_row_ptr.data(), (G.num_nodes + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(G.d_col_idx, h_col_idx.data(), G.num_edges * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(G.d_edge_weights, h_edge_weights.data(), G.num_edges * sizeof(float), cudaMemcpyHostToDevice);
}

// ============================
// Parse New Edges Function
// ============================

void parseNewEdges(const std::string& filename, int& num_new_edges, int*& d_new_edges, float*& d_new_edge_weights) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::tuple<int, int, float>> edges;
    int p, q;
    char type;

    // Read edges from the file
    while (infile >> type >> p >> q)
    {
        float w = (type == 'a') ? 1.0f : -1.0f;
        edges.emplace_back(p, q, w);
        edges.emplace_back(q, p, w);
    }
    infile.close();

    // Number of new edges
    num_new_edges = edges.size();

    // Allocate unified memory (directly accessible on both CPU and GPU)
    cudaMallocManaged(&d_new_edges, num_new_edges * 2 * sizeof(int));  // (p, q) for each edge
    cudaMallocManaged(&d_new_edge_weights, num_new_edges * sizeof(float));  // weight for each edge

    // Directly store values into unified memory
    for (int i = 0; i < num_new_edges; i++)
    {
        d_new_edges[2 * i] = std::get<0>(edges[i]);      // p
        d_new_edges[2 * i + 1] = std::get<1>(edges[i]);  // q
        d_new_edge_weights[i] = std::get<2>(edges[i]);   // w
    }

    // Ensure memory synchronization
    cudaDeviceSynchronize();
}

// ============================
// Function Prototypes
// ============================

// ============================
// For Static Louvain
// ============================

void louvain(Graph &G, VertexCommunityMap &vertex_community_map);

void singletonPartition(Graph &G, Partition &P);

float computeGraphWeight(Graph &G);

__global__ void initializeVertexCommunityMap(int *d_vertex_map, int num_nodes);

__global__ void initializeAlphaBetaKernel(int *row_ptr, int *col_idx, float *edge_weights, float *d_alpha, float *d_beta, int numVertices);
void initializeAlphaBeta(Graph &G);

__global__ void computeAlphaKernel(int *row_ptr, int *col_idx, float *edge_weights,
                                   int *d_community, float *d_alpha, int numVertices);
void computeAlpha(Graph &G, Partition &P);

__global__ void moveNodesKernel(int *row_ptr, int *col_idx, float *edge_weights, float m,
                                int *P, float *alpha, float *beta, int *d_best_community, float *d_k_x, int numVertices);
void moveNodes(Graph &G, Partition &P, int &updated);

__global__ void updateAlphaBetaKernel(int *d_community, int *d_community_map,
                                      float *d_alpha, float *d_beta,
                                      float *d_new_alpha, float *d_new_beta,
                                      int num_nodes);

__global__ void updateVertexCommunityMap(int *d_vertex_map, int *d_community, int *d_community_map, int num_nodes);

__global__ void storeCommunityEdges(
    int numVertices, int *d_row_ptr, int *d_col_idx, float *d_edge_weights, // Original graph (CSR)
    int *d_community, int *d_community_map,
    EdgeTuple *d_edge_buffer, int *d_edge_counter // Temporary storage & counter
);

__global__ void reduceEdges(
    EdgeTuple *d_edge_buffer, int num_edges, // Sorted edge list
    int *d_row_ptr, int *d_final_edge_count // Final edge count
);

__global__ void populateCSR(
    EdgeTuple *d_edge_buffer, int num_edges,
    int *d_row_ptr, int *d_col_idx, float *d_edge_weights
);

void aggregateGraph(Graph &G, Partition &P, VertexCommunityMap &vertex_community_map);

// ============================
// For Dynamic Louvain
// ============================

__global__ void updateAlphaBeta(int *d_new_edges, float *d_new_edge_weights,
                                int num_new_edges, int *d_vertex_map,
                                float *d_alpha, float *d_beta);

__global__ void computeAdditionalEdgesPerNode(int *d_new_edges, int num_new_edges, int *d_additional_edges_per_node);
__global__ void updateRowPtr(int *d_row_ptr, int *d_new_row_ptr, int *d_additional_edges_per_node, int num_nodes);
__global__ void insertOldEdges(
    int *d_row_ptr, int *d_col_idx, float *d_edge_weights, int num_edges,
    int *d_new_col_idx, float *d_new_edge_weights_arr, int *d_new_row_ptr
);
__global__ void insertNewEdges(
    int *d_new_edges, float *d_new_edge_weights, int num_new_edges,
    int *d_new_col_idx, float *d_new_edge_weights_arr, int *d_new_row_ptr
);

void addEdgesToGraph(Graph &G, int *d_new_edges, float *d_new_edge_weights, int num_new_edges);


__global__ void markVisited(int *d_new_edges, int num_new_edges, int *d_visited);

__global__ void processActiveVertices(Graph G, Graph G_init, int *d_vertex_map,
                                      int *d_active_vertices, int num_active_vertices, int *d_visited);

__global__ void computeAlphaDynamicKernel(int *row_ptr, int *col_idx, float *edge_weights,
                                          int *d_vertex_map, float *d_alpha, int numVertices);
void computeAlphaDynamic(Graph &G, Graph &G_init, int* d_vertex_map);

__global__ void updateAlphaBetaForMappedNodes(int *d_vertex_map, float *d_alpha_G, float *d_beta_G,
                                              float *d_alpha_G_init, float *d_beta_G_init,
                                              int num_nodes);
void updateAlphaBetaFromAggregated(Graph &G, Graph &G_init, VertexCommunityMap &vertex_community_map);

void dynamicLouvain(Graph &G, Graph &G_init, VertexCommunityMap &vertex_community_map, int *d_new_edges, int num_new_edges);

// ========================================================
// Functions for Dynamic Louvain
// ========================================================

// ============================
// Update alpha and beta in G
// ============================

__global__ void updateAlphaBeta(int *d_new_edges, float *d_new_edge_weights,
                                int num_new_edges, int *d_vertex_map,
                                float *d_alpha, float *d_beta) {

    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < num_new_edges)
    {
        int u = d_new_edges[2 * idx];       // First vertex of the edge
        int v = d_new_edges[2 * idx + 1];   // Second vertex of the edge
        float w = d_new_edge_weights[idx];  // Edge weight

        int comm_u = d_vertex_map[u];  // Get the community of u
        int comm_v = d_vertex_map[v];  // Get the community of v

        if (comm_u == comm_v)
        {
            // Edge is inside the same community, affects alpha
            atomicAdd(&d_alpha[comm_u], w);
        }
        // Edge contributes to beta for both communities
        atomicAdd(&d_beta[comm_u], w);
    }
}

// ============================
// Computing AdditionalEdgesPerNode
// ============================

__global__ void computeAdditionalEdgesPerNode(int *d_new_edges, int num_new_edges, int *d_additional_edges_per_node) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= num_new_edges) return;

    int u = d_new_edges[2 * idx];
    atomicAdd(&d_additional_edges_per_node[u], 1);
}

// ============================
// Updating RowPtr for Graph
// ============================

__global__ void updateRowPtr(int *d_row_ptr, int *d_new_row_ptr, int *d_prefix_sum, int num_nodes) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid > num_nodes) return;

    d_new_row_ptr[tid] = d_row_ptr[tid] + d_prefix_sum[tid];
}

// ============================
// Insert Old Edges in Graph
// ============================

__global__ void insertOldEdges(
    int *d_row_ptr, int *d_col_idx, float *d_edge_weights, int num_nodes,
    int *d_new_col_idx, float *d_new_edge_weights_arr, int *d_new_row_ptr)
{
    int v = blockIdx.x * blockDim.x + threadIdx.x;
    if (v >= num_nodes) return;

    // Get the range of edges for this vertex
    int start = d_row_ptr[v];
    int end = d_row_ptr[v + 1];
    int new_start = d_new_row_ptr[v];

    for (int i = start; i < end; i++)
    {
        int new_pos = new_start + (i - start);  // Offset by the correct amount
        d_new_col_idx[new_pos] = d_col_idx[i];
        d_new_edge_weights_arr[new_pos] = d_edge_weights[i];
    }
    d_new_row_ptr[v] = d_new_row_ptr[v] + (end-start);
}

// ============================
// Insert New Edges in Graph
// ============================

__global__ void insertNewEdges(
    int *d_new_edges, float *d_new_edge_weights, int num_new_edges,
    int *d_new_col_idx, float *d_new_edge_weights_arr, int *d_new_row_ptr
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= num_new_edges) return;

    int u = d_new_edges[2 * idx];
    int v = d_new_edges[2 * idx + 1];
    float weight = d_new_edge_weights[idx];

    int insert_pos_u = atomicAdd(&d_new_row_ptr[u], 1);

    d_new_col_idx[insert_pos_u] = v;
    d_new_edge_weights_arr[insert_pos_u] = weight;
}

// ============================
// Add Edges To Graph Function
// ============================

void addEdgesToGraph(Graph &G, int *d_new_edges, float *d_new_edge_weights, int num_new_edges) {
    int num_nodes = G.num_nodes;
    int new_num_edges = G.num_edges + num_new_edges;

    // Allocate new memory for CSR representation
    int *d_new_row_ptr, *d_new_col_idx;
    float *d_new_edge_weights_arr;

    cudaMallocManaged(&d_new_row_ptr, (num_nodes + 1) * sizeof(int));
    cudaMallocManaged(&d_new_col_idx, new_num_edges * sizeof(int));
    cudaMallocManaged(&d_new_edge_weights_arr, new_num_edges * sizeof(float));

    cudaMemset(d_new_col_idx, 0, new_num_edges * sizeof(int));
    cudaMemset(d_new_edge_weights_arr, 0, new_num_edges * sizeof(float));

    // Step 1: Compute number of new edges per node
    int *d_additional_edges_per_node;
    cudaMallocManaged(&d_additional_edges_per_node, num_nodes * sizeof(int));
    cudaMemset(d_additional_edges_per_node, 0, num_nodes * sizeof(int));

    int threadsPerBlock = 1024;
    int blocksPerGrid = (num_new_edges + threadsPerBlock - 1) / threadsPerBlock;

    computeAdditionalEdgesPerNode<<<blocksPerGrid, threadsPerBlock>>>(
        d_new_edges, num_new_edges, d_additional_edges_per_node
    );
    cudaDeviceSynchronize();

    thrust::device_vector<int> d_prefix_sum(num_nodes + 1, 0);
    thrust::inclusive_scan(thrust::device, d_additional_edges_per_node,
                           d_additional_edges_per_node + num_nodes,
                           d_prefix_sum.begin() + 1);

    cudaFree(d_additional_edges_per_node);

    updateRowPtr<<<(num_nodes + threadsPerBlock - 1) / threadsPerBlock, threadsPerBlock>>>(
        G.d_row_ptr, d_new_row_ptr, thrust::raw_pointer_cast(d_prefix_sum.data()), num_nodes
    );
    cudaDeviceSynchronize();

    d_prefix_sum.clear();
    d_prefix_sum.shrink_to_fit();

    int *d_new_row_ptr_temp;
    cudaMallocManaged(&d_new_row_ptr_temp, (num_nodes + 1) * sizeof(int));
    cudaMemcpy(d_new_row_ptr_temp, d_new_row_ptr, (num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToDevice);

    // Step 2: Insert old and new edges into col_idx and edge_weights
    blocksPerGrid = (G.num_nodes + threadsPerBlock - 1) / threadsPerBlock;

    insertOldEdges<<<blocksPerGrid, threadsPerBlock>>>(
        G.d_row_ptr, G.d_col_idx, G.d_edge_weights, G.num_nodes,
        d_new_col_idx, d_new_edge_weights_arr, d_new_row_ptr
    );
    cudaDeviceSynchronize();

    blocksPerGrid = (num_new_edges + threadsPerBlock - 1) / threadsPerBlock;

    insertNewEdges<<<blocksPerGrid, threadsPerBlock>>>(
        d_new_edges, d_new_edge_weights, num_new_edges,
        d_new_col_idx, d_new_edge_weights_arr, d_new_row_ptr
    );
    cudaDeviceSynchronize();

    cudaFree(d_new_row_ptr);

    // Free old memory and update graph structure
    cudaFree(G.d_row_ptr);
    cudaFree(G.d_col_idx);
    cudaFree(G.d_edge_weights);

    G.num_edges = new_num_edges;

    cudaMalloc(&G.d_row_ptr, (G.num_nodes + 1) * sizeof(int));
    cudaMalloc(&G.d_col_idx, G.num_edges * sizeof(int));
    cudaMalloc(&G.d_edge_weights, G.num_edges * sizeof(float));

    cudaMemcpy(G.d_row_ptr, d_new_row_ptr_temp, (G.num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_col_idx, d_new_col_idx, G.num_edges * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_edge_weights, d_new_edge_weights_arr, G.num_edges * sizeof(float), cudaMemcpyDeviceToDevice);

    // Step 4: Free temporary allocations
    cudaFree(d_new_row_ptr_temp);
    cudaFree(d_new_col_idx);
    cudaFree(d_new_edge_weights_arr);
}

// ============================
// Mark visited kernel
// ============================

__global__ void markVisited(int *d_new_edges, int num_new_edges, int *d_visited) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < num_new_edges) {
        int v = d_new_edges[2 * idx + 1];
        d_visited[v] = 1;
    }
}

// ============================
// Process Active Nodes kernel
// ============================

__global__ void processActiveVertices(Graph G, Graph G_init, int *d_vertex_map,
                                      int *d_active_vertices, int num_active_vertices, int *d_visited) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= G_init.num_nodes) return;

    int v = tid;

    int initial_community = d_vertex_map[v];
    int best_community = initial_community;
    float best_delta_Q = 0.0f;

    float k_x = 0.0f;  // Sum of all edge weights connected to x
    float Ex_p = 0.0f; // Sum of edge weights from x to its own community

    for (int i = G_init.d_row_ptr[v]; i < G_init.d_row_ptr[v + 1]; i++)
    {
        k_x += G_init.d_edge_weights[i];
        if (d_vertex_map[G_init.d_col_idx[i]] == initial_community)
        {
            Ex_p += G_init.d_edge_weights[i]; // Only consider edges within the same community
        }
    }

    float alpha_1 = G.d_alpha[initial_community];
    float beta_1 = G.d_beta[initial_community];

    // Iterate over neighbors
    for (int i = G_init.d_row_ptr[v]; i < G_init.d_row_ptr[v + 1]; i++)
    {
        int neighbor = G_init.d_col_idx[i];
        int neighbor_community = d_vertex_map[neighbor];

        if (neighbor_community == initial_community)
        {
            continue; // Skip if the neighbor is in the same community
        }

        float Ex_q = 0.0f;
        // Compute Ex_q: sum of edge weights from x to community q
        for (int j = G_init.d_row_ptr[v]; j < G_init.d_row_ptr[v + 1]; j++)
        {
            if (d_vertex_map[G_init.d_col_idx[j]] == neighbor_community)
            {
                Ex_q += G_init.d_edge_weights[j];
            }
        }

        // Get alpha and beta for the neighbor's community
        float alpha_2 = G.d_alpha[neighbor_community];
        float beta_2 = G.d_beta[neighbor_community];

        // Compute delta Q
        float delta_Q =
            (((alpha_1 - 2 * Ex_p) / G.d_m) - powf((beta_1 - k_x) / G.d_m, 2)) +
            (((alpha_2 + 2 * Ex_q) / G.d_m) - powf((beta_2 + k_x) / G.d_m, 2)) -
            (((alpha_1 / G.d_m) - powf(beta_1 / G.d_m, 2)) +
             ((alpha_2 / G.d_m) - powf(beta_2 / G.d_m, 2)));

        // Update best move if delta_Q is positive
        if (delta_Q > best_delta_Q)
        {
            best_delta_Q = delta_Q;
            best_community = neighbor_community;
        }
    }

    if (best_community != initial_community)
    {
        d_vertex_map[v] = best_community;

        for (int i = G_init.d_row_ptr[v]; i < G_init.d_row_ptr[v + 1]; i++)
        {
            d_visited[G_init.d_col_idx[i]] = 1;
        }

        atomicAdd(&G.d_beta[initial_community], -k_x);
        atomicAdd(&G.d_beta[best_community], k_x);
    }
}

// ============================
// Compute Alpha Kernel
// ============================
__global__ void computeAlphaDynamicKernel(int *row_ptr, int *col_idx, float *edge_weights,
                                   int *d_vertex_map, float *d_alpha, int numVertices) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= numVertices) return;

    int community_x = d_vertex_map[tid];

    for (int i = row_ptr[tid]; i < row_ptr[tid + 1]; i++) {
        int neighbor = col_idx[i];
        int community_y = d_vertex_map[neighbor];

        if (community_x == community_y)
        {
            atomicAdd(&d_alpha[community_x], edge_weights[i]);
            if(neighbor == tid)
            {
                atomicAdd(&d_alpha[community_x], edge_weights[i]);
            }
        }
    }
}

// ============================
// Compute Alpha Dynamic Host Wrapper
// ============================
void computeAlphaDynamic(Graph &G, Graph &G_init, int* d_vertex_map) {
    int threadsPerBlock = 1024;
    int blocksPerGrid = (G_init.num_nodes + threadsPerBlock - 1) / threadsPerBlock;

    // Reset alpha values to zero
    cudaMemset(G.d_alpha, 0, G.num_nodes * sizeof(float));

    computeAlphaDynamicKernel<<<blocksPerGrid, threadsPerBlock>>>(G_init.d_row_ptr, G_init.d_col_idx,
                                                           G_init.d_edge_weights, d_vertex_map,
                                                           G.d_alpha, G_init.num_nodes);

    cudaDeviceSynchronize();
}

// ========================================================
// updateAlphaBeta Kernel For Dynamic Louvain
// ========================================================

__global__ void updateAlphaBetaForMappedNodes(int *d_vertex_map, float *d_alpha_G, float *d_beta_G,
                                              float *d_alpha_G_init, float *d_beta_G_init,
                                              int num_nodes) {
    int v = blockIdx.x * blockDim.x + threadIdx.x;
    if (v >= num_nodes) return;  // Out-of-bounds check

    int community = d_vertex_map[v];  // Get the community of vertex v

    d_alpha_G_init[community] = d_alpha_G[community];  // Copy alpha from community
    d_beta_G_init[community] = d_beta_G[community];  // Copy beta from community
}

// ========================================================
// updateAlphaBeta For Dynamic Louvain Wrapper
// ========================================================

void updateAlphaBetaFromAggregated(Graph &G, Graph &G_init, VertexCommunityMap &vertex_community_map) {
    int num_nodes = G_init.num_nodes;

    // Allocate memory for alpha and beta in G_init if not already allocated
    cudaMalloc(&G_init.d_alpha, num_nodes * sizeof(float));
    cudaMalloc(&G_init.d_beta, num_nodes * sizeof(float));

    // Allocate memory for updated alpha and beta values for G_init (on GPU)
    cudaMemset(G_init.d_alpha, 0, num_nodes * sizeof(float));  // Initialize alpha to zero
    cudaMemset(G_init.d_beta, 0, num_nodes * sizeof(float));  // Initialize beta to zero

    // Call kernel to update only mapped nodes
    int threadsPerBlock = 1024;
    int numBlocks = (num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    updateAlphaBetaForMappedNodes<<<numBlocks, threadsPerBlock>>>(
        vertex_community_map.d_vertex_map, G.d_alpha, G.d_beta,
        G_init.d_alpha, G_init.d_beta, num_nodes);

    cudaDeviceSynchronize();
}

// ============================
// Dynamic Louvain Function
// ============================

void dynamicLouvain(Graph &G, Graph &G_init, VertexCommunityMap &vertex_community_map, int *d_new_edges, int num_new_edges) {
    int num_nodes = G_init.num_nodes;

    // Allocate host memory for debugging
    float *h_alpha = (float*)malloc(G.num_nodes * sizeof(float));
    float *h_beta = (float*)malloc(G.num_nodes * sizeof(float));

    cudaMemcpy(h_alpha, G.d_alpha, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_beta, G.d_beta, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);

    float Q_initial = 0.0f;
    for (int c = 0; c < G.num_nodes; c++)
    {
        Q_initial += (h_alpha[c]/G.d_m) - pow((h_beta[c] / G.d_m), 2);
    }

    // Allocate memory for visited array
    int *d_visited;
    cudaMallocManaged(&d_visited, num_nodes * sizeof(int));
    cudaMemset(d_visited, 0, num_nodes * sizeof(int));

    // Kernel to mark visited nodes
    int threadsPerBlock = 1024;
    int blocksPerGrid = (num_new_edges + threadsPerBlock - 1) / threadsPerBlock;
    markVisited<<<blocksPerGrid, threadsPerBlock>>>(d_new_edges, num_new_edges, d_visited);
    cudaDeviceSynchronize();

    // Step 1: Allocate d_active_vertices
    int* d_active_vertices;
    cudaMallocManaged(&d_active_vertices, num_nodes * sizeof(int));

    // Step 2: Copy d_visited into d_active_vertices
    cudaMemcpy(d_active_vertices, d_visited, num_nodes * sizeof(int), cudaMemcpyDeviceToDevice);

    // Step 3: Count number of 1's in d_visited
    int num_active_vertices = thrust::count(thrust::device, d_visited, d_visited + num_nodes, 1);

    // Iterate while there are active vertices
    while (num_active_vertices > 0)
    {
        // Reset visited array for the next iteration
        cudaMemset(d_visited, 0, num_nodes * sizeof(int));
        cudaDeviceSynchronize();

        // Launch kernel to process active vertices
        blocksPerGrid = (num_nodes + threadsPerBlock - 1) / threadsPerBlock;

        processActiveVertices<<<blocksPerGrid, threadsPerBlock>>>(
            G, G_init, vertex_community_map.d_vertex_map, d_active_vertices,
            num_nodes, d_visited);
        cudaDeviceSynchronize();

        computeAlphaDynamic(G,G_init,vertex_community_map.d_vertex_map);

        cudaMemcpy(h_alpha, G.d_alpha, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(h_beta, G.d_beta, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);

        float Q = 0.0f;
        for (int c = 0; c < G.num_nodes; c++)
        {
            Q += (h_alpha[c]/G.d_m) - pow((h_beta[c] / G.d_m), 2);
        }

        if(Q > Q_initial + 1e-6)
        {
            Q_initial = Q;
        }
        else
        {
            break;
        }

        // Compute new set of active vertices based on updated d_visited
        cudaMemcpy(d_active_vertices, d_visited, num_nodes * sizeof(int), cudaMemcpyDeviceToDevice);

        // Step 3: Count number of 1's in d_visited
        num_active_vertices = thrust::count(thrust::device, d_visited, d_visited + num_nodes, 1);
    }

    // Free memory
    cudaFree(d_visited);

    Partition P;
    cudaMalloc(&P.d_community, G_init.num_nodes * sizeof(int));
    cudaDeviceSynchronize();

    cudaMemcpy(P.d_community, vertex_community_map.d_vertex_map,
              G_init.num_nodes * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaDeviceSynchronize();

    updateAlphaBetaFromAggregated(G, G_init, vertex_community_map);

    blocksPerGrid = (G_init.num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    initializeVertexCommunityMap<<<blocksPerGrid, threadsPerBlock>>>(vertex_community_map.d_vertex_map, G_init.num_nodes);
    cudaDeviceSynchronize();

    aggregateGraph(G_init,P,vertex_community_map);
    cudaFree(P.d_community);
    louvain(G_init, vertex_community_map);
}

// ========================================================
// Functions for Static Louvain
// ========================================================

// ============================
// Computing m
// ============================
float computeGraphWeight(Graph &G)
{
    float totalWeight = 0;

    // Copy row_ptr, col_idx, and edge_weights from device to host
    vector<int> h_row_ptr(G.num_nodes + 1);
    vector<int> h_col_idx(G.num_edges);
    vector<float> h_edge_weights(G.num_edges);

    cudaMemcpy(h_row_ptr.data(), G.d_row_ptr, (G.num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_col_idx.data(), G.d_col_idx, G.num_edges * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_edge_weights.data(), G.d_edge_weights, G.num_edges * sizeof(float), cudaMemcpyDeviceToHost);

    // Traverse each node's adjacency list
    for (int u = 0; u < G.num_nodes; ++u)
    {
        for (int idx = h_row_ptr[u]; idx < h_row_ptr[u + 1]; ++idx)
        {
            int v = h_col_idx[idx];
            float w = h_edge_weights[idx];

            // Double the weight for self-loops
            if (u == v)
            {
                totalWeight += (2*w);
            }
            else
            {
                totalWeight += w;
            }
        }
    }

    return totalWeight;
}

// ============================
// Initialize VertexCommunityMap
// ============================
__global__ void initializeVertexCommunityMap(int *d_vertex_map, int num_nodes)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < num_nodes)
    {
        d_vertex_map[tid] = tid; // Each node starts in its own community
    }
}

// ============================
// Initialize AlphaBeta Kernel
// ============================
__global__ void initializeAlphaBetaKernel(int *row_ptr, int *col_idx, float *edge_weights, float *d_alpha, float *d_beta, int numVertices) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= numVertices) return;

    float k_x = 0; // Sum of edge weights of edges connected to node x
    float self_edge_weight = 0;  // Stores weight of self-edge if it exists

    for (int i = row_ptr[tid]; i < row_ptr[tid + 1]; i++) {
        int neighbor = col_idx[i];

        k_x += edge_weights[i];

        if (neighbor == tid)
        {
            self_edge_weight = edge_weights[i];  // Capture self-edge weight
        }
    }

    d_alpha[tid] = self_edge_weight;
    d_beta[tid] = k_x;    // Beta is the sum of edges incident to the node
}

// ============================
// Initialize AlphaBeta Host Wrapper
// ============================
void initializeAlphaBeta(Graph &G) {
    int numThreads = 1024;
    int numBlocks = (G.num_nodes + numThreads - 1) / numThreads;

    cudaMallocManaged(&G.d_alpha, G.num_nodes * sizeof(float));
    cudaMallocManaged(&G.d_beta, G.num_nodes * sizeof(float));

    cudaMemset(G.d_alpha, 0, G.num_nodes * sizeof(float));
    cudaMemset(G.d_beta, 0, G.num_nodes * sizeof(float));

    initializeAlphaBetaKernel<<<numBlocks, numThreads>>>(G.d_row_ptr, G.d_col_idx, G.d_edge_weights, G.d_alpha, G.d_beta, G.num_nodes);
    cudaDeviceSynchronize();
}

// ============================
// Compute Alpha Kernel
// ============================
__global__ void computeAlphaKernel(int *row_ptr, int *col_idx, float *edge_weights,
                                   int *d_community, float *d_alpha, int numVertices) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= numVertices) return;

    int community_x = d_community[tid];

    for (int i = row_ptr[tid]; i < row_ptr[tid + 1]; i++) {
        int neighbor = col_idx[i];
        int community_y = d_community[neighbor];

        if (community_x == community_y)
        {
            atomicAdd(&d_alpha[community_x], edge_weights[i]);
            if(neighbor == tid)
            {
                atomicAdd(&d_alpha[community_x], edge_weights[i]);
            }
        }
    }
}

// ============================
// Compute Alpha Host Wrapper
// ============================
void computeAlpha(Graph &G, Partition &P) {
    int threadsPerBlock = 1024;
    int blocksPerGrid = (G.num_nodes + threadsPerBlock - 1) / threadsPerBlock;

    // Reset alpha values to zero
    cudaMemset(G.d_alpha, 0, G.num_nodes * sizeof(float));

    computeAlphaKernel<<<blocksPerGrid, threadsPerBlock>>>(G.d_row_ptr, G.d_col_idx,
                                                           G.d_edge_weights, P.d_community,
                                                           G.d_alpha, G.num_nodes);
    cudaDeviceSynchronize();
}

// ============================
// Singleton Partition Function
// ============================
void singletonPartition(Graph &G, Partition &P)
{
    cudaMalloc(&P.d_community, G.num_nodes * sizeof(int));

    int *h_community = new int[G.num_nodes];
    for (int i = 0; i < G.num_nodes; i++)
    {
        h_community[i] = i; // Each node starts in its own community
    }

    cudaMemcpy(P.d_community, h_community, G.num_nodes * sizeof(int), cudaMemcpyHostToDevice);
    delete[] h_community;
}

// ============================
// MoveNodes Kernel
// ============================
__global__ void moveNodesKernel(int *row_ptr, int *col_idx, float *edge_weights, float m,
                                int *P, float *d_alpha, float *d_beta, int *d_best_community, float *d_k_x, int numVertices)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= numVertices) return;

    int initial_community = P[tid];
    int best_community = initial_community;
    float best_delta_Q = 0.0f;

    float k_x = 0.0f;  // Sum of all edge weights connected to x
    float Ex_p = 0.0f; // Sum of edge weights from x to its own community

    for (int i = row_ptr[tid]; i < row_ptr[tid + 1]; i++)
    {
        k_x += edge_weights[i];
        if (P[col_idx[i]] == initial_community)
        {
            Ex_p += edge_weights[i]; // Only consider edges within the same community
        }
    }

    float alpha_1 = d_alpha[initial_community];
    float beta_1 = d_beta[initial_community];

    // Iterate over neighbors
    for (int i = row_ptr[tid]; i < row_ptr[tid + 1]; i++)
    {
        int neighbor = col_idx[i];
        int neighbor_community = P[neighbor];

        if (neighbor_community == initial_community)
        {
            continue; // Skip if the neighbor is in the same community
        }

        float Ex_q = 0.0f;
        // Compute Ex_q: sum of edge weights from x to community q
        for (int j = row_ptr[tid]; j < row_ptr[tid + 1]; j++)
        {
            if (P[col_idx[j]] == neighbor_community)
            {
                Ex_q += edge_weights[j];
            }
        }

        // Get alpha and beta for the neighbor's community
        float alpha_2 = d_alpha[neighbor_community];
        float beta_2 = d_beta[neighbor_community];

        // Compute delta Q
        float delta_Q =
            (((alpha_1 - 2 * Ex_p) / m) - powf((beta_1 - k_x) / m, 2)) +
            (((alpha_2 + 2 * Ex_q) / m) - powf((beta_2 + k_x) / m, 2)) -
            (((alpha_1 / m) - powf(beta_1 / m, 2)) +
             ((alpha_2 / m) - powf(beta_2 / m, 2)));

        // Update best move if delta_Q is positive
        if (delta_Q > best_delta_Q)
        {
            best_delta_Q = delta_Q;
            best_community = neighbor_community;
        }
    }

    if (best_community != initial_community)
    {
        d_best_community[tid] = best_community;
        d_k_x[tid] = k_x;
    }
    else
    {
        d_best_community[tid] = -1;
    }
}

// ============================
// MoveNodes Host Wrapper
// ============================
void moveNodes(Graph &G, Partition &P, int &updated)
{
    int threadsPerBlock = 1024;
    int blocksPerGrid = (G.num_nodes + threadsPerBlock - 1) / threadsPerBlock;

    // Allocate host memory for debugging
    float *h_alpha = (float*)malloc(G.num_nodes * sizeof(float));
    float *h_beta = (float*)malloc(G.num_nodes * sizeof(float));
    int *h_community = (int*)malloc(G.num_nodes * sizeof(int));

    cudaMemcpy(h_alpha, G.d_alpha, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_beta, G.d_beta, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);

    float sum_alpha_init = 0.0f;
    float sum_beta_sq_init = 0.0f;

    for (int c = 0; c < G.num_nodes; c++)
    {
        sum_alpha_init += h_alpha[c];
        sum_beta_sq_init += h_beta[c] * h_beta[c];
    }

    float Q_initial = (sum_alpha_init / G.d_m) - (sum_beta_sq_init / (G.d_m * G.d_m));
    // cout<<"THE VALUE OF Q_initial is "<<Q_initial<<"\n\n";

    int updated_for_moveNodes = 0;

    // Allocate memory for best moves and k_x values
    int *d_best_community;
    float *d_k_x;
    cudaMalloc(&d_best_community, G.num_nodes * sizeof(int));
    cudaMalloc(&d_k_x, G.num_nodes * sizeof(float));

    // Allocate host memory for updates
    int *h_best_community = new int[G.num_nodes];
    float *h_k_x = new float[G.num_nodes];
    int *h_P = new int[G.num_nodes];

    // CPU-side community size tracking
    vector<int> h_community_size(G.num_nodes, 1); // Initially, each node is a community of size 1

    do
    {
        moveNodesKernel<<<blocksPerGrid, threadsPerBlock>>>(G.d_row_ptr, G.d_col_idx,
                                                            G.d_edge_weights, G.d_m, P.d_community,
                                                            G.d_alpha, G.d_beta,
                                                            d_best_community, d_k_x, G.num_nodes);
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) {
            printf("CUDA error: %s\n", cudaGetErrorString(err));
        }
        cudaDeviceSynchronize();

        // Copy results back to host
        cudaMemcpy(h_best_community, d_best_community, G.num_nodes * sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(h_k_x, d_k_x, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(h_P, P.d_community, G.num_nodes * sizeof(int), cudaMemcpyDeviceToHost);

        for (int i = 0; i < G.num_nodes; i++)
        {
            if (h_best_community[i] != -1)
            {
                int initial_community = h_P[i];
                int best_community = h_best_community[i];

                // **Check if best_community still has members left**
                if (h_community_size[best_community] > 0)
                {
                    // Perform the move
                    h_P[i] = best_community;

                    h_beta[initial_community] -= h_k_x[i];
                    h_beta[best_community] += h_k_x[i];

                    // Update community sizes
                    h_community_size[initial_community]--;
                    h_community_size[best_community]++;
                }
            }
        }

        // Copy updated communities back to device
        cudaMemcpy(P.d_community, h_P, G.num_nodes * sizeof(int), cudaMemcpyHostToDevice);

        cudaMemcpy(G.d_beta, h_beta, G.num_nodes * sizeof(float), cudaMemcpyHostToDevice);

        computeAlpha(G, P);

        cudaMemcpy(h_alpha, G.d_alpha, G.num_nodes * sizeof(float), cudaMemcpyDeviceToHost);

        float sum_alpha = 0.0f;
        float sum_beta_sq = 0.0f;

        for (int c = 0; c < G.num_nodes; c++)
        {
            sum_alpha += h_alpha[c];
            sum_beta_sq += h_beta[c] * h_beta[c];
        }

        float Q = (sum_alpha / G.d_m) - (sum_beta_sq / (G.d_m * G.d_m));

        // cout<<"Q: "<<Q<<" Q_initial: "<<Q_initial<<endl;
        if(Q > Q_initial + 1e-8)
        {
            updated_for_moveNodes = 1;
            Q_initial = Q;
        }
        else
        {
            updated_for_moveNodes = 0;
        }
    } while (updated_for_moveNodes != 0);

    cudaFree(d_best_community);
    cudaFree(d_k_x);

    delete[] h_best_community;
    delete[] h_k_x;
    delete[] h_P;

    thrust::device_vector<int> d_vec(P.d_community, P.d_community + G.num_nodes);
    thrust::sort(d_vec.begin(), d_vec.end());
    auto new_end = thrust::unique(d_vec.begin(), d_vec.end());

    // Check if the number of unique elements is less than original
    int num_unique = new_end - d_vec.begin();

    if (num_unique < G.num_nodes)
    {
        updated = 1;
    }
    else
    {
        updated = 0;
    }
}

// ============================
// update AlphaBeta Kernel
// ============================

__global__ void updateAlphaBetaKernel(int *d_community, int *d_community_map,
                                      float *d_alpha, float *d_beta,
                                      float *d_new_alpha, float *d_new_beta,
                                      int num_nodes)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= num_nodes) return;

    int new_community = d_community_map[tid];

    if (new_community != -1)
    {
        d_new_alpha[new_community] = d_alpha[tid];
        d_new_beta[new_community] = d_beta[tid];
    }
}

// ============================
// update VertexCommunityMap Kernel
// ============================

__global__ void updateVertexCommunityMap(int *d_vertex_map, int *d_community, int *d_community_map, int num_nodes)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < num_nodes)
    {
        int old_node = d_vertex_map[tid];
        int old_community = d_community[old_node];
        int new_community = d_community_map[old_community]; // Get the new community ID
        d_vertex_map[tid] = new_community; // Update the vertex's community
    }
}

// ============================
// store CommunityEdges Kernel
// ============================

// Kernel to store edges based on the vertex-to-node mapping
__global__ void storeCommunityEdges(int numVertices, int *d_row_ptr, int *d_col_idx,
    float *d_edge_weights, // Original graph (CSR)
    int *d_community, int *d_community_map,
    EdgeTuple *d_edge_buffer, int *d_edge_counter // Temporary storage & counter
)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    if (x >= numVertices) return;

    int p = d_community_map[d_community[x]]; // Get node ID for vertex x
    int start = d_row_ptr[x];
    int end = d_row_ptr[x + 1];

    for (int i = start; i < end; i++)
    {
        int y = d_col_idx[i]; // Neighbor
        float weight = d_edge_weights[i];

        int q = d_community_map[d_community[y]]; // Get node ID for vertex y

        // Get position to store the edge
        int idx = atomicAdd(d_edge_counter, 1);
        if(p!=q)
        {
            d_edge_buffer[idx] = {p, q, weight}; // Store (node_p, node_q, weight)
        }
        else if(x!=y)
        {
            d_edge_buffer[idx] = {p, q, weight/2}; // When self loop add edge with half weight
        }
        else
        {
            d_edge_buffer[idx] = {p, q, weight};
        }
    }
}

// ============================
// reduceEdges Kernel
// ============================

__global__ void reduceEdges(
    EdgeTuple *d_edge_buffer, int num_edges, // Sorted edge list
    int *d_row_ptr, int *d_final_edge_count // Final edge count
)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= num_edges) return;

    if (i == 0 || d_edge_buffer[i].node_p != d_edge_buffer[i - 1].node_p || d_edge_buffer[i].node_q != d_edge_buffer[i - 1].node_q)
    {
        int pos = atomicAdd(d_final_edge_count, 1);

        // Use atomicAdd to correctly increment row pointers
        atomicAdd(&d_row_ptr[d_edge_buffer[i].node_p], 1);
    }
}

// ============================
// populateCSR Kernel
// ============================

__global__ void populateCSR(
    EdgeTuple *d_edge_buffer, int num_edges,
    int *d_row_ptr, int *d_col_idx, float *d_edge_weights
)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= num_edges) return;

    if (i == 0 || d_edge_buffer[i].node_p != d_edge_buffer[i - 1].node_p || d_edge_buffer[i].node_q != d_edge_buffer[i - 1].node_q)
    {
        float total_weight = d_edge_buffer[i].weight;

        // Iterate over subsequent duplicate edges to accumulate weight
        for (int j = i + 1; j < num_edges; j++) {
            if (d_edge_buffer[j].node_p == d_edge_buffer[i].node_p &&
                d_edge_buffer[j].node_q == d_edge_buffer[i].node_q)
            {
                total_weight += d_edge_buffer[j].weight;
            }
            else break; // Stop when we reach a new edge
        }

        // Insert edge into CSR
        int pos = atomicAdd(&d_row_ptr[d_edge_buffer[i].node_p], 1);
        d_col_idx[pos] = d_edge_buffer[i].node_q;
        d_edge_weights[pos] = total_weight;
    }
}

// ============================
// Aggregation Host Wrapper
// ============================
void aggregateGraph(Graph &G, Partition &P, VertexCommunityMap &vertex_community_map)
{
    // Step 1: Count unique communities
    int *h_community = new int[G.num_nodes];
    cudaMemcpy(h_community, P.d_community, G.num_nodes * sizeof(int), cudaMemcpyDeviceToHost);

    vector<int> community_map(G.num_nodes, -1);
    int new_num_nodes = 0;
    for (int i = 0; i < G.num_nodes; i++)
    {
        if (community_map[h_community[i]] == -1)
        {
            community_map[h_community[i]] = new_num_nodes++;
        }
    }
    // cout<<"The number of nodes in new graph is "<<new_num_nodes<<endl;

    int *d_community_map;
    cudaMalloc(&d_community_map, G.num_nodes * sizeof(int));
    cudaMemcpy(d_community_map, community_map.data(), G.num_nodes * sizeof(int), cudaMemcpyHostToDevice);

    // Step 2: Launch kernel to update alpha and beta values

    // Allocate memory for new alpha and beta
    float *d_new_alpha, *d_new_beta;
    cudaMalloc(&d_new_alpha, new_num_nodes * sizeof(float));
    cudaMalloc(&d_new_beta, new_num_nodes * sizeof(float));

    cudaMemset(d_new_alpha, 0, new_num_nodes * sizeof(float));
    cudaMemset(d_new_beta,  0, new_num_nodes * sizeof(float));

    // Launch CUDA Kernel
    int threadsPerBlock = 1024;
    int blocksPerGrid = (G.num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    updateAlphaBetaKernel<<<blocksPerGrid, threadsPerBlock>>>(P.d_community, d_community_map,
                                                              G.d_alpha, G.d_beta,
                                                              d_new_alpha, d_new_beta,
                                                              G.num_nodes);
    cudaDeviceSynchronize();

    // Step 3: Launch kernel to update vertex_community_map
    threadsPerBlock = 1024;
    blocksPerGrid = (vertex_community_map.size + threadsPerBlock - 1) / threadsPerBlock;
    updateVertexCommunityMap<<<blocksPerGrid, threadsPerBlock>>>(
        vertex_community_map.d_vertex_map, P.d_community, d_community_map, vertex_community_map.size);
    cudaDeviceSynchronize();

    // Step 4: Allocate new structure for the aggregated graph
    Graph newG;
    newG.num_nodes = new_num_nodes;

    // Allocate memory for new alpha and beta inside newG
    cudaMalloc(&newG.d_alpha, new_num_nodes * sizeof(float));
    cudaMalloc(&newG.d_beta, new_num_nodes * sizeof(float));

    // Copy values from temporary arrays
    cudaMemcpy(newG.d_alpha, d_new_alpha, new_num_nodes * sizeof(float), cudaMemcpyDeviceToDevice);
    cudaMemcpy(newG.d_beta, d_new_beta, new_num_nodes * sizeof(float), cudaMemcpyDeviceToDevice);

    // Free the temporary arrays
    cudaFree(d_new_alpha);
    cudaFree(d_new_beta);

    // Step 5: Collect edges in d_edge_buffer
    int *d_edge_counter;
    cudaMalloc(&d_edge_counter, sizeof(int));
    cudaMemset(d_edge_counter, 0, sizeof(int)); // Initialize to 0

    EdgeTuple *d_edge_buffer;
    cudaMalloc(&d_edge_buffer, G.num_edges * sizeof(EdgeTuple));

    storeCommunityEdges<<<blocksPerGrid, threadsPerBlock>>>(
        G.num_nodes, G.d_row_ptr, G.d_col_idx, G.d_edge_weights,
        P.d_community, d_community_map, d_edge_buffer, d_edge_counter);
    cudaDeviceSynchronize();

    cudaFree(d_community_map);

    // Step 6: Sort edges using Thrust
    thrust::sort(thrust::device, d_edge_buffer, d_edge_buffer + G.num_edges, EdgeComparator());
    cudaDeviceSynchronize();

    int *d_final_edge_count;
    cudaMalloc(&d_final_edge_count, sizeof(int));
    cudaMemset(d_final_edge_count, 0, sizeof(int));

    cudaMalloc(&newG.d_row_ptr, (newG.num_nodes + 1) * sizeof(int));
    cudaMemset(newG.d_row_ptr, 0, (newG.num_nodes + 1) * sizeof(int));
    cudaMalloc(&newG.d_col_idx, G.num_edges * sizeof(int));
    cudaMalloc(&newG.d_edge_weights, G.num_edges * sizeof(float));

    // Step 7: Reduce duplicate edges and compute final edge list
    blocksPerGrid = (G.num_edges + threadsPerBlock - 1) / threadsPerBlock;
    reduceEdges<<<blocksPerGrid, threadsPerBlock>>>(
        d_edge_buffer, G.num_edges,
        newG.d_row_ptr, d_final_edge_count);
    cudaDeviceSynchronize();

    // Step 8: Compute prefix sum for d_row_ptr
    thrust::exclusive_scan(thrust::device, newG.d_row_ptr, newG.d_row_ptr + newG.num_nodes + 1, newG.d_row_ptr);

    int *d_row_ptr;
    cudaMalloc(&d_row_ptr, (newG.num_nodes + 1) * sizeof(int));  // Allocate memory
    cudaMemcpy(d_row_ptr, newG.d_row_ptr, (newG.num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToDevice);

    populateCSR<<<blocksPerGrid, threadsPerBlock>>>(d_edge_buffer, G.num_edges, d_row_ptr, newG.d_col_idx, newG.d_edge_weights);
    cudaDeviceSynchronize();

    cudaFree(d_row_ptr);

    // Step 9: Store final edge count
    cudaMemcpy(&newG.num_edges, d_final_edge_count, sizeof(int), cudaMemcpyDeviceToHost);

    // Step 10: Cleanup
    cudaFree(d_final_edge_count);
    cudaFree(d_edge_buffer);
    delete[] h_community;

    // Step 11: Free old graph memory (before updating G)
    cudaFree(G.d_row_ptr);
    cudaFree(G.d_col_idx);
    cudaFree(G.d_edge_weights);
    cudaFree(G.d_alpha);
    cudaFree(G.d_beta);

    // Step 12: Allocate new memory for G
    cudaMalloc(&G.d_row_ptr, (newG.num_nodes + 1) * sizeof(int));
    cudaMalloc(&G.d_col_idx, newG.num_edges * sizeof(int));
    cudaMalloc(&G.d_edge_weights, newG.num_edges * sizeof(float));
    cudaMalloc(&G.d_alpha, newG.num_nodes * sizeof(float));
    cudaMalloc(&G.d_beta, newG.num_nodes * sizeof(float));

    // Step 13: Copy data from newG to G
    cudaMemcpy(G.d_row_ptr, newG.d_row_ptr, (newG.num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_col_idx, newG.d_col_idx, newG.num_edges * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_edge_weights, newG.d_edge_weights, newG.num_edges * sizeof(float), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_alpha, newG.d_alpha, newG.num_nodes * sizeof(float), cudaMemcpyDeviceToDevice);
    cudaMemcpy(G.d_beta, newG.d_beta, newG.num_nodes * sizeof(float), cudaMemcpyDeviceToDevice);

    // Step 14: Update G's metadata
    G.num_nodes = newG.num_nodes;
    G.num_edges = newG.num_edges;

    // Step 15: Free newG memory (since its data is now in G)
    cudaFree(newG.d_row_ptr);
    cudaFree(newG.d_col_idx);
    cudaFree(newG.d_edge_weights);
    cudaFree(newG.d_alpha);
    cudaFree(newG.d_beta);
}

// ============================
// Main Louvain Function
// ============================
void louvain(Graph &G, VertexCommunityMap &vertex_community_map)
{
    Partition P;
    singletonPartition(G, P);
    initializeAlphaBeta(G);

    bool done = false;
    while (!done)
    {
        int updated = 0;
        moveNodes(G, P, updated);

        done = (updated == 0);

        if (!done)
        {
            aggregateGraph(G, P, vertex_community_map);
            // Free previously allocated memory before reassigning
            cudaFree(P.d_community);
            singletonPartition(G, P);
            G.d_m = computeGraphWeight(G);
        }
    }
    cudaFree(P.d_community);
}

// ============================
// Main Function
// ============================
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <initial_graph_file> <new_edges_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string initial_graph_file = argv[1];
    std::string new_edges_file = argv[2];

    // Define Sample Graph in CSR format
    Graph G, G_init;
    parseGraph(initial_graph_file, G);
    G.d_m = computeGraphWeight(G);

    int num_nodes_initial_graph = G.num_nodes;
    // ********* CREATE A COPY OF THE INITIAL GRAPH (Optimized) *********
    G_init.num_nodes = G.num_nodes;
    G_init.num_edges = G.num_edges;

    cudaMallocManaged(&G_init.d_row_ptr, (G.num_nodes + 1) * sizeof(int));
    cudaMallocManaged(&G_init.d_col_idx, G.num_edges * sizeof(int));
    cudaMallocManaged(&G_init.d_edge_weights, G.num_edges * sizeof(float));

    // Use async memory copies
    cudaMemcpyAsync(G_init.d_row_ptr, G.d_row_ptr, (G.num_nodes + 1) * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpyAsync(G_init.d_col_idx, G.d_col_idx, G.num_edges * sizeof(int), cudaMemcpyDeviceToDevice);
    cudaMemcpyAsync(G_init.d_edge_weights, G.d_edge_weights, G.num_edges * sizeof(float), cudaMemcpyDeviceToDevice);

    // Synchronize to ensure all memory copies complete before proceeding
    cudaDeviceSynchronize();


    // ********* INITIALIZE THE VERTEX COMMUNITY MAP *********

    // Initialize VertexCommunityMap
    VertexCommunityMap vertex_community_map;
    vertex_community_map.size = G.num_nodes;  // Track the size explicitly
    cudaMallocManaged(&vertex_community_map.d_vertex_map, vertex_community_map.size * sizeof(int));

    int threadsPerBlock = 1024;
    int blocksPerGrid = (G.num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    initializeVertexCommunityMap<<<blocksPerGrid, threadsPerBlock>>>(vertex_community_map.d_vertex_map, G.num_nodes);
    cudaDeviceSynchronize();

    cudaEvent_t start, stop;
    float milliseconds;

    // Timing Louvain
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // ********* RUN LOUVAIN *********
    // cout<<"The number of nodes in new graph is "<<G.num_nodes<<endl;
    louvain(G, vertex_community_map);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    std::cout << "Louvain Execution Time : " << milliseconds << " ms" << std::endl;
    cout<<"\n\n";

    // Print community assignments after Louvain
    cout << "Initial Community Mapping:" << endl;
    cout << "{";
    for (int i = 0; i < num_nodes_initial_graph - 1; i++)
    {
        cout <<i << ": " << vertex_community_map.d_vertex_map[i] << ", ";
    }
    cout <<num_nodes_initial_graph - 1 << ": " << vertex_community_map.d_vertex_map[num_nodes_initial_graph - 1] << "}";

    cout<<"\n\n\n";
    // ********* DEFINE EDGES TO BE ADDED (Incremental Case) *********

    int num_new_edges;
    int* d_new_edges;
    float* d_new_edge_weights;

    parseNewEdges(new_edges_file, num_new_edges, d_new_edges, d_new_edge_weights);

    // ********* UPDATE ALPHA AND BETA IN GRAPH *********

    // Set up kernel parameters
    threadsPerBlock = 1024;
    int numBlocks = (num_new_edges + threadsPerBlock - 1) / threadsPerBlock;

    // Call the kernel
    updateAlphaBeta<<<numBlocks, threadsPerBlock>>>(d_new_edges, d_new_edge_weights, num_new_edges, vertex_community_map.d_vertex_map, G.d_alpha, G.d_beta);

     cudaDeviceSynchronize();

    // ********* ADD NEW EDGES TO GRAPH *********
    addEdgesToGraph(G_init, d_new_edges, d_new_edge_weights, num_new_edges);

    // Free device memory
    cudaFree(d_new_edge_weights);

    G_init.d_m = computeGraphWeight(G_init);
    G.d_m = G_init.d_m;

    // ********* CALL DYNAMIC LOUVAIN *********
    // Timing Dynamic Louvain
    cudaEventRecord(start);

    cout<<"The number of nodes in new graph is "<<G_init.num_nodes<<endl;
    dynamicLouvain(G, G_init, vertex_community_map, d_new_edges, num_new_edges);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    std::cout << "Dynamic Louvain Execution Time: " << milliseconds << " ms" << std::endl;
    cout<<"\n\n";

    // Clean up
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    // Print community assignments after Louvain
    cout << "Final Community Mapping:" << endl;
    cout << "{";
    for (int i = 0; i < num_nodes_initial_graph - 1; i++)
    {
        cout <<i << ": " << vertex_community_map.d_vertex_map[i] <<", ";
    }
    cout <<num_nodes_initial_graph-1 << ": " << vertex_community_map.d_vertex_map[num_nodes_initial_graph - 1] <<"}\n";

    // Free memory
    cudaFree(G.d_alpha);
    cudaFree(G.d_beta);
    cudaFree(G.d_row_ptr);
    cudaFree(G.d_col_idx);
    cudaFree(G.d_edge_weights);

    cudaFree(G_init.d_row_ptr);
    cudaFree(G_init.d_col_idx);
    cudaFree(G_init.d_edge_weights);
    cudaFree(G_init.d_alpha);
    cudaFree(G_init.d_beta);

    cudaFree(vertex_community_map.d_vertex_map);

    cudaFree(d_new_edges);

    return 0;
}

