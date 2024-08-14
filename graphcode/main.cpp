#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>
#include "./generated_omp/dynamicBatchTCV2_dyn.cc"
int main() {
    char filePath[] = "./sample_graphs/sample_graph.txt";
    graph g(filePath);
    g.parseGraph();
     std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
    std::cout << "Number of edges: " << g.num_edges() << std::endl;
    double starttime = omp_get_wtime();
   long num_triangles = staticTC(g);
   double endtime = omp_get_wtime();
    std::cout << "Number of triangles in the graph: " << num_triangles << std::endl;
    std::cout<<"Time taken : "<<endtime-starttime<<std::endl;
    return 0;
}