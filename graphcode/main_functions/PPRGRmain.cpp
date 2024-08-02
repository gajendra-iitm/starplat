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
    int source = 0; // Assuming source node index
    int sink = g.num_nodes() - 1; // Assuming sink node index as the last node
    int kernel_parameter = 10; // Example value for kernel_parameter(modify)
    double starttime = omp_get_wtime();
    do_max_flow(g, source, sink, kernel_parameter);
    double endtime = omp_get_wtime();
    for (const auto& e : g.edges()) {
    std::cout<< g.source(e) << " " << g.target(e) << "residual capacity: " << e.residual_capacity << std::endl;
}
    std::cout<<"Time taken : "<<endtime-starttime<<std::endl;

    return 0;
    }