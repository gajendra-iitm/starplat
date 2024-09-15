#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <climits>
// #include "./update.hpp"
// #include "./update.hpp"
// #include"./atomicUtil.h"
// #include "./generated_omp/v_cover.cc"
#include "./generated_omp/test_abhirup.cc"
// #include "header.h"
// #include "./garph.hpp"
// void DynTC(graph& g , std::vector<update> updateBatch , int batchSize);
int main(int argc, char*argv[]) {
    char* filePath;
    char* updateFile;
    if (argc == 1) {
        std::string inputPath;
        std::cout << "Enter the path to the graph file: ";
        std::getline(std::cin, inputPath);

        filePath = new char[inputPath.length() + 1]; 
        std::strcpy(filePath, inputPath.c_str());

        // ######## importing updates ##########
        std::cout << "Enter updated path location: ";
        std::getline(std::cin, inputPath);

        updateFile = new char[inputPath.length() + 1]; 
        std::strcpy(updateFile, inputPath.c_str());
    } else if (argc == 2) {
        filePath = argv[1];
    } else if (argc==3){
        filePath = argv[1];
        updateFile=argv[2];
    }else{
        return 1;
    }

    graph g(filePath);
    
    float* vc = new float[g.num_nodes()];
    g.parseGraph();
    std::vector<update> parseUpdates=parseUpdateFile(updateFile,100);
    int batchSize=parseUpdates.size();
    std::cout <<"batch size = " <<batchSize << std::endl;
     std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
     std::cout << "Number of edges: " << g.num_edges() << std::endl;
    double starttime = omp_get_wtime();
    long triangle_count=compute_static_TC(g);
    // DynTC(g,parseUpdates,batchSize);
    double endtime = omp_get_wtime();
    // for (int i = 0; i < g.num_nodes(); i++) {
    //     std::cout<< vc[i] << std::endl;
    // }
    std::cout << "Number of triangles present: " << triangle_count << std::endl;
    std::cout<<"\nTime taken : "<<endtime-starttime<<std::endl;
    return 0;
    }
