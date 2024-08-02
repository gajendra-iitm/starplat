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
     std::set<int> sourceSet;
    // Assuming sourceSet is initialized with some nodes
    for (int i = 0; i < g.num_nodes(); ++i) { // Example: first 10 nodes as sources
        sourceSet.insert(i);
    }

    std::map<int, float> CC;

    double starttime = omp_get_wtime();
    Compute_CC(g, CC, sourceSet);
    double endtime = omp_get_wtime();

    for (const auto& pair : CC) {
        std::cout << "Node " << pair.first << " Closeness Centrality: " << pair.second << std::endl;
    }

    std::cout << "\nTime taken: " << endtime - starttime << " seconds" << std::endl;
    return 0;
    }