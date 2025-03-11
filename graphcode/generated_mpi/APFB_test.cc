#include"APFB_test.h"

auto APFB(Graph& g, int nc, boost::mpi::communicator world )
{
  int totalMatching = 0;
  int v = 0;
  g.create_bfs_dag(v);
  for(int phase=0;phase<g.num_bfs_phases();phase++)
  {
    for( int u : g.get_bfs_nodes_for_phase(phase) )
    {
      totalMatching_leader_rank = world.rank();
      totalMatching = totalMatching + 1;
    }
    world.barrier();
  }
  for(int phase=g.num_bfs_phases()-1;phase>0;phase--)
  {
    for(int u : g.get_bfs_nodes_for_phase(phase))
    {
    }
    world.barrier();
  }
  return totalMatching;

}
