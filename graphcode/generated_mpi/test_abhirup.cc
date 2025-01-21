#include"test_abhirup.h"

auto compute_static_TC(Graph& g, boost::mpi::communicator world )
{
  long staic_triangle_count = 0;
  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    for (int u : g.getNeighbors(v)) 
    {
      if (v < u )
      {
        for (int w : g.getNeighbors(v)) 
        {
          if (v < w )
          {
            if (g.check_if_nbr(u, w) && u < w )
            {
              staic_triangle_count = ( staic_triangle_count + 1) ;
            }
          }
        }


      }
    }


  }
  world.barrier();

  long staic_triangle_count_temp = staic_triangle_count;
  MPI_Allreduce(&staic_triangle_count_temp,&staic_triangle_count,1,MPI_LONG,MPI_SUM,MPI_COMM_WORLD);
  

  return staic_triangle_count;

}
