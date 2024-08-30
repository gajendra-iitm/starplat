#include "../mpi_header/graph_mpi.h"
#include "PageRankDSLV3.h"
#include "../mpi_header/profileAndDebug/mpi_debug.c"

int main(int argc, char *argv[])
{

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    Graph graph(argv[1], world);

    // PageRank
    float beta = 0.01;
    float delta = 0.85;
    int maxIter = 100;
    NodeProperty<float> pageRank;

    double t1 = MPI_Wtime();
    ComputePageRank(graph, beta, delta, maxIter, pageRank, world);
    double t2 = MPI_Wtime();

//    print_mpi_statistics();
    if (world.rank() == 0)
    {
        float sum = 0.0;
        for (int i = 0; i < graph.num_nodes(); i++) {
            float v = pageRank.getValue(i);
            sum += v;
            printf("Node %d: %.6g\n", i, v);
        }
       std::cout << "TIME:[" << (t2 - t1) << "]" << std::endl;
    }

    // world.barrier();
    return 0;
}
