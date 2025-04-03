#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <climits>

#include "/lfs/usrhome/mtech/cs23m006/new_repo/starplat/graphcode/generated_omp/APFB_Matching.cc"

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
    std::vector<update> parseUpdates=parseUpdateFile(updateFile,0.004);
    int batchSize=parseUpdates.size();
    std::cout <<"batch size = " <<batchSize/2 << std::endl;
     std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
     std::cout << "Number of edges: " << g.num_edges() << std::endl;
    double starttime = omp_get_wtime();
    APFB(g, g.num_nodes()/2);//, parseUpdates, batchSize);
    double endtime = omp_get_wtime();

    std::cout<<"\nTime taken : "<<endtime-starttime<<std::endl;
    return 0;
    }
