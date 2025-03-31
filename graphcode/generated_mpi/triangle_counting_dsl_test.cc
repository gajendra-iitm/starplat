#include "../mpi_header/graph_mpi.h"
#include "../mpi_header/profileAndDebug/mpi_debug.c"
#include "triangle_counting_dsl.h"

int main(int argc, char *argv[])
{

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    Graph graph(argv[1], world, 1);

    double t1 = MPI_Wtime();
    long tc = Compute_TC(graph, world);
    double t2 = MPI_Wtime();

    print_mpi_statistics();
    if (world.rank() == 0)
    {
        std::cout << "Triangle Count: " << tc << std::endl;
        std::cout << "TIME:[" << (t2 - t1) << "]" << std::endl;
    }

    world.barrier();
    return 0;
}
