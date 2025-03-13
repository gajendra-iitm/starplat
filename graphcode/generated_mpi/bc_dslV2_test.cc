#include "../mpi_header/graph_mpi.h"
#include "bc_dslV2.h"
#include "../mpi_header/profileAndDebug/mpi_debug.c"

int main(int argc, char *argv[])
{
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    Graph graph(argv[1], world, 1);

    // BC
    std::set<int> sourceSet;
    sourceSet.insert(20);
    NodeProperty<float> BC;
    // MPI time start
    double start = MPI_Wtime();
    Compute_BC(graph, BC, sourceSet, world);
    // MPI time end
    double end = MPI_Wtime();

    if (world.rank() == 0)
    {
        printf("Time: %f\n", end - start);
        // // print result
        for (int i = 0; i < graph.num_nodes(); i++)
        {
            float v = BC.getValue(i) / 2;
            printf("Node %d: %.6g\n", i, v);
        }
    }

    return 0;
}
