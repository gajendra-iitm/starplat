//make sure to first generate the cuda backend code:
//by cd into 'src'
//command to run on your terminal to generate "betweeness centrality (for static graphs) cuda backend code" using bc_dslV2 as input dsl:
// ./StarPlat -s -f ../graphcode/staticDSLCodes/bc_dslV2 -b cuda
//for executing on Google collab: remember to add ! before the above command.

//generating the bc_dslV2 cuda backend code by using the above command will generate two files 'bc_dslV2.cu' and 'bc_dslV2.h' 
//in the directory ../graphcode/generated_cuda which is required for this code to run
//as we have the main caller function for that code written here in this file.
//nvcc bc_dslV2mainCuda.cu -o bc_dslV2mainCuda -arch=sm_70 -std=c++14 -rdc=true
//./bc_dslV2mainCuda ../graphcode/generated_cuda/sample_graph.txt ../graphcode/generated_cuda/src_nodes.txt
#include <cuda_runtime.h>
#include <cuda.h>
#include <iostream>
#include "/lfs/usrhome/mtech/cs23m006/new_repo/starplat/graphcode/generated_cuda/APFB_Matching.cu"

// main fn: reads Input and Calls `Compute_BC`
int main(int argc, char* argv[]) {
    char* filePath;
    char* updateFile;
    if (argc == 2) {
        filePath = argv[1];
    } else if (argc==3){
        filePath = argv[1];
        updateFile=argv[2];
        
    }else{
        return 1;
    }
    graph g(filePath);
    g.parseGraph();
    std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start, 0);
    APFB(g, g.num_nodes());
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    std::cout << "\nGPU Time: " << milliseconds / 1000.0 << " seconds" << std::endl;
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    return 0;
}