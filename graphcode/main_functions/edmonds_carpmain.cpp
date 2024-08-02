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

    std::cout << "Number of nodes: " << g.num_nodes() << std::endl;

// Initialize weights
std::map<std::pair<int, int>, int> weight;
for (int node : g.get_nodes()) {
    for (const auto& neighbor : g.neighbors(node)) {
        weight[{node, neighbor.first}] = neighbor.second;
    }
}

auto nodes = g.get_nodes();
int s = *nodes.begin();
int d = *nodes.rbegin();

double starttime = omp_get_wtime();
int max_flow = ek(g, s, d, weight);
double endtime = omp_get_wtime();

std::cout << "Max Flow: " << max_flow << std::endl;
std::cout << "Time taken: " << endtime - starttime << " seconds" << std::endl;
return 0;
    }