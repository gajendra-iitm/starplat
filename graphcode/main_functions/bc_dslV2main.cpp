//make sure to first generate the static bc OpenMP backend code:
//by cd into 'src'
//command to run on your terminal to generate "betweeness centrality (for static graphs) OpenMP backend code" using bc_dslV2 as input dsl:
// ./StarPlat -s -f ../graphcode/staticDSLCodes/bc_dslV2 -b omp
//for executing on Google collab: remember to add ! before the all the commands.

//generating the bc_dslV2 OpenMP backend code by using the above command will generate two files 'bc_dslV2.cc' and 'bc_dslV2.h' 
//in the directory ../graphcode/generated_omp which is required for this code to run
//as we have the main caller function for that code written here in this file.
//TO COMPILE: g++ -o main bc_dslV2main.cpp -fopenmp
//TO RUN: ./main 
//then enter the <path_to_graph_file> when prompted.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <climits>

//tested for Static BC OpenMP generated code (generated_omp) uncomment the below line to use.
//#include "../generated_omp/bc_dslV2.cc"

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

    //get our graph object 'g'
    graph g(filePath);

    int numOfNodes = g.num_nodes();
    int numOfEdges = g.num_edges();
    
    
    // Define the set of source nodes
    std::set<int> sourceSet;
    for (int i = 0; i < numOfNodes; ++i) {
        sourceSet.insert(i); // Assuming nodes are numbered from 0 to num_nodes()-1
    }

    //allocate memory for BC array for storing BC values.
    float *BC = (float *) malloc(numOfNodes * sizeof(float)) ;

    //parse our grpah 'g' to extract the properties like nodes and edges.
    g.parseGraph();

    printf("Number of nodes: %d\n" , numOfNodes);
    printf("Number of edges: %d\n" , numOfEdges);


    double starttime = omp_get_wtime();
    //calling the Compute_BC function
    Compute_BC(g, BC, sourceSet);
    double endtime = omp_get_wtime();

    printf("Time taken: %lf\n", endtime -starttime);

    for(int i=0; i<numOfNodes; i++){
        printf("Node %d BC value: %f\n",i,BC[i]);
    }

    //free(BC);
    return 0;
}