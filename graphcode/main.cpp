#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>
#include <dlfcn.h> 
#include "/lfs/usrhome/mtech/cs23m006/new_repo/starplat/graphcode/generated_omp/triangle_counting_dsl.cc"
int main(int argc,char *argv[]) {
    char *filePath = argc > 1 ? argv[1] : nullptr ;
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