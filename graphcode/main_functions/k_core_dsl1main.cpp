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
     int k = 3; // adjust this value as needed
    double starttime = omp_get_wtime();
    k_core(g, k);
    double endtime = omp_get_wtime();
    std::cout << "Time taken: " << endtime - starttime << " seconds" << std::endl;
    std::cout << "The number of nodes in the " << k << "-core is: " << kcore_count << std::endl;
    return 0;
    }