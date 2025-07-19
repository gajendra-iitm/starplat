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
#include "./generated_omp/dynamicBatchTCV2_dyn.cc"
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

    float* vc = new float[g.num_nodes()];
    g.parseGraph();
     std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
     std::cout << "Number of edges: " << g.num_edges() << std::endl;
    double starttime = omp_get_wtime();
    long numberOfTriangles=staticTC(g);
    double endtime = omp_get_wtime();
    // for (int i = 0; i < g.num_nodes(); i++) {
    //     std::cout<< vc[i] << std::endl;
    // }
    std::cout << "Number of triangles present: " << numberOfTriangles << std::endl;
    std::cout<<"\nTime taken : "<<endtime-starttime<<std::endl;
    return 0;
    }