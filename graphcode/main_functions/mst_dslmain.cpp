#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <climits>
// #include "./generated_omp/v_cover.cc"
int main(int argc, char*argv[]) {
    char* filePath;

    if (argc == 1) {
        std::string inputPath;
        std::cout << "Enter the path to the graph file: ";
        std::getline(std::cin, inputPath);

        filePath = new char[inputPath.length() + 1]; 
        std::strcpy(filePath, inputPath.c_str());
    } else if (argc == 2) {
        filePath = argv[1];
    } else {
        return 1;
    }

    graph g(filePath);
    g.parseGraph();
         std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;
    float beta = 1e-6; // Convergence threshold for large graphs
    float delta = 0.85; // Damping factor
    int maxIter = 150; // Maximum number of iterations

    float* pageRank = new float[g.num_nodes()];

    double starttime = omp_get_wtime();
   Compute_PR(g, beta, delta, maxIter, pageRank);
   double endtime = omp_get_wtime();
    for (int i = 0; i < g.num_nodes(); i++) {
        std::cout << "Node " << i << ": " << pageRank[i] << std::endl;
    }
    std::cout<<"Time taken : "<<endtime-starttime<<std::endl;

    return 0;
    }