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
    // Define the set of source nodes
    std::set<int> sourceSet;
    for (int i = 0; i < g.num_nodes(); ++i) {
        sourceSet.insert(i); // Assuming nodes are numbered from 0 to num_nodes()-1
    }

    // Define the BC property
    std::map<int, float> BC;
    g.parseGraph();
   std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;

    // Define the set of source nodes
    std::set<int> sourceSet;
    for (int i = 0; i < g.num_nodes(); ++i) {
        sourceSet.insert(i); // Assuming nodes are numbered from 0 to num_nodes()-1
    }

    // Define the BC property
    std::map<int, float> BC;

    double starttime = omp_get_wtime();
    Compute_BC(g, BC, sourceSet); // Call the Compute_BC function
    double endtime = omp_get_wtime();

    std::cout << "\nTime taken: " << endtime - starttime << " seconds" << std::endl;

    // // Output BC values for all nodes
    // for (const auto& entry : BC) {
    //     std::cout << "Node " << entry.first << " BC: " << entry.second << std::endl;
    // }

    return 0;
    }