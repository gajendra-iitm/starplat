#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <climits>
#include "./generated_omp/triangle_counting_dsl_complete_graph.cc"

int main(int argc, char *argv[])
{
    char *filePath;

    if (argc == 1)
    {
        std::string inputPath;
        std::cout << "Enter the path to the graph file: ";
        std::getline(std::cin, inputPath);

        filePath = new char[inputPath.length() + 1];
        std::strcpy(filePath, inputPath.c_str());
    }
    else if (argc == 2)
    {
        filePath = argv[1];
    }
    else
    {
        return 1;
    }

    geomCompleteGraph g(filePath);
    g.parseGraph();

    long numberOfTriangles= Compute_TC(g);

    std::cout<<numberOfTriangles<<std::endl;

    return 0;
}