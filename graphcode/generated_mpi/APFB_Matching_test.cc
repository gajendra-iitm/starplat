#include"APFB_Matching_test.h"

auto mod(int a, int b, boost::mpi::communicator world )
{
  return a - b * (a / b);

}
auto APFB(Graph& g, int nc, boost::mpi::communicator world )
{
  NodeProperty<bool> modified;
  NodeProperty<int> rmatch;
  NodeProperty<int> cmatch;
  modified.attachToGraph(&g, (bool)false);
  rmatch.attachToGraph(&g, (int)-1);
  cmatch.attachToGraph(&g, (int)-1);
  bool noNewPaths = false;
  NodeProperty<int> bfsArray;
  NodeProperty<int> predeccesor;
  NodeProperty<bool> compress;
  NodeProperty<bool> compress_next;
  NodeProperty<bool> visited;
  visited.attachToGraph(&g, (bool)false);
  int evenComponents = 0;
  int oddComponents = 0;
  int unmatchedComponents = 0;
  int totalMatching = 0;
  while ( !noNewPaths )
  {
    int L0 = 0;
    int NOT_VISITED = L0 - 1;
    bfsArray.attachToGraph(&g, (int)NOT_VISITED);
    world.barrier();
    for (int c = g.start_node(); c <= g.end_node(); c ++) 
    {
      if (c < nc )
      {
        if (cmatch.getValue(c) == -1 )
        {
          bfsArray.setValue(c,L0);
        }
      }
    }
    world.barrier();


    predeccesor.attachToGraph(&g, (int)-1);
    int bfsLevel = L0;
    bool noNewVertices = false;
    while ( !noNewVertices )
    {
      int noNewPaths_leader_rank = -1 ;
      int noNewVertices_leader_rank = -1 ;

      world.barrier();
      for (int col_vertex = g.start_node(); col_vertex <= g.end_node(); col_vertex ++) 
      {
        if (col_vertex < nc )
        {
          if (bfsArray.getValue(col_vertex) == bfsLevel )
          {
            for (int neigh_row : g.getNeighbors(col_vertex)) 
            {
              if (neigh_row >= nc )
              {
                int col_match = rmatch.getValue(neigh_row);
                if (col_match > -1 )
                {
                  if (bfsArray.getValue(col_match) == NOT_VISITED )
                  {
                    noNewVertices_leader_rank = world.rank();
                    noNewVertices = false;
                    bfsArray.setValue(col_match,bfsLevel + 1);
                    predeccesor.setValue(neigh_row,col_vertex);
                  }
                }
                if (col_match == -1 )
                {
                  rmatch.setValue(neigh_row,-2);
                  predeccesor.setValue(neigh_row,col_vertex);
                  noNewPaths_leader_rank = world.rank();
                  noNewPaths = false;
                }
              }
            }


          }
        }
      }
      world.barrier();
      int noNewPaths_leader_rank_temp = noNewPaths_leader_rank;
      MPI_Allreduce(&noNewPaths_leader_rank_temp,&noNewPaths_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&noNewPaths,1,MPI_C_BOOL,noNewPaths_leader_rank,MPI_COMM_WORLD);

      int noNewVertices_leader_rank_temp = noNewVertices_leader_rank;
      MPI_Allreduce(&noNewVertices_leader_rank_temp,&noNewVertices_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&noNewVertices,1,MPI_C_BOOL,noNewVertices_leader_rank,MPI_COMM_WORLD);




      bfsLevel = bfsLevel + 1;
      noNewVertices = modified.aggregateValue(NOT);
    }
    compress.attachToGraph(&g, (bool)false);
    compress_next.attachToGraph(&g, (bool)false);
    world.barrier();
    for (int r = g.start_node(); r <= g.end_node(); r ++) 
    {
      if (r >= nc && rmatch.getValue(r) == -2 )
      {
        compress.setValue(r,true);
      }
    }
    world.barrier();


    bool compressed = false;
    while ( !compressed )
    {
      int compressed_leader_rank = -1 ;

      world.barrier();
      for (int row_vertex = g.start_node(); row_vertex <= g.end_node(); row_vertex ++) 
      {
        if (compress.getValue(row_vertex) == true )
        {
          if (row_vertex >= nc )
          {
            int matched_col = predeccesor.getValue(row_vertex);
            int matched_row = cmatch.getValue(matched_col);
            bool isValid = true;
            if (matched_row != -1 )
            {
              if (predeccesor.getValue(matched_row) == matched_col )
              {
                isValid = false;
              }
            }
            if (isValid )
            {
              cmatch.setValue(matched_col,row_vertex);
              rmatch.setValue(row_vertex,matched_col);
              if (matched_row != -1 )
              {
                compress_next.setValue(matched_row,true);
                compressed_leader_rank = world.rank();
                compressed = false;
              }
            }
          }
        }
      }
      world.barrier();
      int compressed_leader_rank_temp = compressed_leader_rank;
      MPI_Allreduce(&compressed_leader_rank_temp,&compressed_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
      MPI_Bcast(&compressed,1,MPI_C_BOOL,compressed_leader_rank,MPI_COMM_WORLD);




      world.barrier();
      for (int row_vertex = g.start_node(); row_vertex <= g.end_node(); row_vertex ++) 
      {
        if (row_vertex >= nc )
        {
          compress.setValue(row_vertex,compress_next.getValue(row_vertex));
          compress_next.setValue(row_vertex,false);
        }
      }
      world.barrier();


      compressed = modified.aggregateValue(NOT);
    }
    world.barrier();
    for (int r = g.start_node(); r <= g.end_node(); r ++) 
    {
      if (r >= nc )
      {
        int c = rmatch.getValue(r);
        if (c >= 0 )
        {
          if (cmatch.getValue(c) != r )
          {
            rmatch.setValue(r,-1);
          }
        }
        if (c == -2 )
        {
          rmatch.setValue(r,-1);
        }
      }
    }
    world.barrier();


    noNewPaths = modified.aggregateValue(NOT);
  }

  world.barrier();
  int totalMatching_leader_rank = -1 ;
  for(int r = g.start_node(); r <= g.end_node(); r ++){
    if(rmatch.getValue(r) != -1){
      totalMatching_leader_rank = world.rank();
      totalMatching = totalMatching + 1;
    }
  }
  world.barrier();
  int totalMatching_leader_rank_temp = totalMatching_leader_rank;
  MPI_Allreduce(&totalMatching_leader_rank_temp,&totalMatching_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&totalMatching,1,MPI_INT,totalMatching_leader_rank,MPI_COMM_WORLD);

  NodeProperty<bool> in_bfs;
  NodeProperty<bool> isMatching;
  NodeProperty<int> bfsDist;
  bfsDist.attachToGraph(&g, (int)0);
  in_bfs.attachToGraph(&g, (bool)false);
  isMatching.attachToGraph(&g, (bool)false);
  int componentSize = 0;
  int unmatchedCount = 0;
  int altEdgeCount = 0;
  int evenComponents_leader_rank = -1 ;
  int oddComponents_leader_rank = -1 ;
  int unmatchedComponents_leader_rank = -1 ;

  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    visited.setValue(v,true);
    in_bfs.setValue(v,true);
    isMatching.setValue(v,(cmatch.getValue(v) != -1 || rmatch.getValue(v) != -1));
    g.create_bfs_dag(v);
    for(int phase=0;phase<g.num_bfs_phases();phase++)
    {
      for( int u : g.get_bfs_nodes_for_phase(phase) )
      {
        bool isMatchedEdge = (cmatch.getValue(u) != -1 || rmatch.getValue(u) != -1);
        for (int neigh : g.getNeighbors(u)) 
        {
          if (!visited.getValue(neigh) )
          {
            bool nextIsMatchedEdge = !isMatchedEdge;
            if ((isMatchedEdge && (cmatch.getValue(u) == neigh || rmatch.getValue(u) == neigh)) || (!isMatchedEdge && (cmatch.getValue(u) == -1 || rmatch.getValue(u) == -1)) )
            {
              visited.setValue(neigh,true);
              altEdgeCount_leader_rank = world.rank();
              altEdgeCount = altEdgeCount + 1;
              isMatching.setValue(neigh,nextIsMatchedEdge);
              in_bfs.setValue(neigh,true);
            }
          }
        }


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
    if (unmatchedCount == 0 && mod(altEdgeCount,2, world) == 0 )
    {
      evenComponents_leader_rank = world.rank();
      evenComponents = evenComponents + 1;
    }
    else
    if (mod(altEdgeCount,2, world) == 1 )
    {
      oddComponents_leader_rank = world.rank();
      oddComponents = oddComponents + 1;
    }
    else
    {
      unmatchedComponents_leader_rank = world.rank();
      unmatchedComponents = unmatchedComponents + 1;
    }
  }
  world.barrier();
  int evenComponents_leader_rank_temp = evenComponents_leader_rank;
  MPI_Allreduce(&evenComponents_leader_rank_temp,&evenComponents_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&evenComponents,1,MPI_INT,evenComponents_leader_rank,MPI_COMM_WORLD);

  int oddComponents_leader_rank_temp = oddComponents_leader_rank;
  MPI_Allreduce(&oddComponents_leader_rank_temp,&oddComponents_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&oddComponents,1,MPI_INT,oddComponents_leader_rank,MPI_COMM_WORLD);

  int unmatchedComponents_leader_rank_temp = unmatchedComponents_leader_rank;
  MPI_Allreduce(&unmatchedComponents_leader_rank_temp,&unmatchedComponents_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&unmatchedComponents,1,MPI_INT,unmatchedComponents_leader_rank,MPI_COMM_WORLD);




  
  int totalMatching_leader_rank = -1 ;

  world.barrier();
  for (int v = g.start_node(); v <= g.end_node(); v ++) 
  {
    if (rmatch.getValue(v) != -1 )
    {
      totalMatching_leader_rank = world.rank();
      totalMatching = totalMatching + 1;
    }
  }
  world.barrier();
  int totalMatching_leader_rank_temp = totalMatching_leader_rank;
  MPI_Allreduce(&totalMatching_leader_rank_temp,&totalMatching_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&totalMatching,1,MPI_INT,totalMatching_leader_rank,MPI_COMM_WORLD);




  return totalMatching;

}
