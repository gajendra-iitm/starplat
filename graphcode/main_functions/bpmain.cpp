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

    int k;

    g.parseGraph();
    std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;

    std::map<std::pair<int, int>, int> weight;
    // Initialize weight for edges
    for (const auto& edge : g.get_edges()) {
        weight[edge] = 1; // Assuming default weight is 1 for all edges
    }

    int k = 10; // Example value for k

    double starttime = omp_get_wtime();
    bp(g, k, weight); // Call the bp function
    double endtime = omp_get_wtime();

    std::cout << "\nTime taken: " << endtime - starttime << " seconds" << std::endl;
    return 0;
    }