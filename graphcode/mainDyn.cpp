#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <set>
//#include <map>
//#include <algorithm>
//#include <cstring>
//#include <climits>
//#include <boost/mpi.hpp>
//#include <iostream>
#include "/lfs/usrhome/oth/rnintern/cs23m006/starplat_new/starplat/graphcode/mpi_header/graph_mpi.h"
#include "/lfs/usrhome/oth/rnintern/cs23m006/starplat_new/starplat/graphcode/mpi_header/profileAndDebug/mpi_debug.c"
// #include "./generated_omp/v_cover.cc"
#include"/lfs/usrhome/oth/rnintern/cs23m006/starplat_new/starplat/graphcode/generated_mpi/APFB_DM.cc"
//#include "/lfs/usrhome/oth/rnintern/cs23m006/starplat_new/starplat/graphcode/generated_mpi/triangle_counting_dsl.cc"
// void DynAPFB(Graph& g, int nc, Updates * updateBatch, int batchSize, boost::mpi::communicator world );

int main(int argc, char *argv[]) {
    char* filePath;
    char* updateFile;
		boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    if (argc == 1) {
        std::string inputPath;
        std::cout << "Enter the path to the graph file: ";
        std::getline(std::cin, inputPath);

        filePath = new char[inputPath.length() + 1]; 
        std::strcpy(filePath, inputPath.c_str());
    } else if (argc == 2) {
        filePath = argv[1];
    }else if(argc ==3 ){
        filePath = argv[1];
        updateFile = argv[2];
    } else {
        return 1;
    }

   Graph graph(argv[1],world,1);
   //  Updates(char* file,boost::mpi::communicator world, Graph * g, float percent_updates=100);
   Updates updates(argv[2],world,&graph,50);

    //float* vc = new float[g.num_nodes()];
    //g.parseGraph();
     //std::cout << "Number of nodes: " << g.num_nodes() << std::endl;
     //std::cout <<"Number of edges: " << g.num_edges() << std::endl;
    //double starttime = omp_get_wtime();
    double t1 = MPI_Wtime();
    DynAPFB(graph,graph.num_nodes(),&updates,50,world);
    std::cout << "No of nodes  " << graph.num_nodes() << std::endl;
	double t2 = MPI_Wtime();
		
		if (world.rank() == 0)
    {
        // std::cout << "Total Matching: " << ans << std::endl;
        print_mpi_statistics();
        std::cout << "TIME:[" << (t2 - t1) << "]" << std::endl;
    }
    //double endtime = omp_get_wtime();
    // for (int i = 0; i < g.num_nodes(); i++) {
    //     std::cout<< vc[i] << std::endl;
    // }
    //std::cout<<"\nTime taken : "<<endtime-starttime<<std::endl;
    world.barrier();
    return 0;
    }