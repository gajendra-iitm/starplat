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
    double starttime = omp_get_wtime();
   long num_triangles = Compute_TC(g);
   double endtime = omp_get_wtime();
    std::cout << "Number of triangles in the graph: " << num_triangles << std::endl;
    std::cout<<"Time taken : "<<endtime-starttime<<std::endl;
    return 0;
    }