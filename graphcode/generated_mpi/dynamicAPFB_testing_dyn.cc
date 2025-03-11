#include"dynamicAPFB_testing_dyn.h"

auto mod(int a, int b, boost::mpi::communicator world )
{
  return a - b * (a / b);

}
auto APFB(Graph& g, int nc, NodeProperty<int>& rmatch, NodeProperty<int>& cmatch
  , boost::mpi::communicator world )
{
  NodeProperty<bool> modified;
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
  int totalMatching = 0;
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
auto dynamicAPFB_testing_del(Graph& g, int previouslyMatched, EdgeProperty<bool>& modified, Updates * deleteBatch, 
  NodeProperty<int>& rmatch, NodeProperty<int>& cmatch, boost::mpi::communicator world )
{
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
  int noNewPaths_leader_rank = -1 ;

  world.barrier();
  for(Update update : deleteBatch->getUpdates())
  {
    while ( !noNewPaths )
    {
      int L0 = 0;
      int NOT_VISITED = L0 - 1;
      bfsArray.attachToGraph(&g, (int)NOT_VISITED);
      for (int c = 0; c < g.num_nodes(); c ++) 
      {
        if (c < nc )
        {
          if (cmatch.getValue(c) == -1 )
          {
            bfsArray.setValue(c,L0);
          }
        }
      }


      predeccesor.attachToGraph(&g, (int)-1);
      int bfsLevel = L0;
      bool noNewVertices = false;
      while ( !noNewVertices )
      {
        for (int col_vertex = 0; col_vertex < g.num_nodes(); col_vertex ++) 
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


        bfsLevel = bfsLevel + 1;
        noNewVertices = modified.aggregateValue(NOT);
      }
      compress.attachToGraph(&g, (bool)false);
      compress_next.attachToGraph(&g, (bool)false);
      for (int r = 0; r < g.num_nodes(); r ++) 
      {
        if (r >= nc && rmatch.getValue(r) == -2 )
        {
          compress.setValue(r,true);
        }
      }


      bool compressed = false;
      while ( !compressed )
      {
        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
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
                  compressed = false;
                }
              }
            }
          }
        }


        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
        {
          if (row_vertex >= nc )
          {
            compress.setValue(row_vertex,compress_next.getValue(row_vertex));
            compress_next.setValue(row_vertex,false);
          }
        }


        compressed = modified.aggregateValue(NOT);
      }
      for (int r = 0; r < g.num_nodes(); r ++) 
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


      noNewPaths = modified.aggregateValue(NOT);
    }
  }
  world.barrier();
  int noNewPaths_leader_rank_temp = noNewPaths_leader_rank;
  MPI_Allreduce(&noNewPaths_leader_rank_temp,&noNewPaths_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&noNewPaths,1,MPI_C_BOOL,noNewPaths_leader_rank,MPI_COMM_WORLD);




  int totalMatching = 0;
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




  totalMatching = previouslyMatched - totalMatching;
  return totalMatching;

}
auto dynamicAPFB_testing_add(Graph& g, int previouslyMatched, EdgeProperty<bool>& modified, Updates * addBatch, 
  NodeProperty<int>& rmatch, NodeProperty<int>& cmatch, boost::mpi::communicator world )
{
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
  int noNewPaths_leader_rank = -1 ;

  world.barrier();
  for(Update update : addBatch->getUpdates())
  {
    while ( !noNewPaths )
    {
      int L0 = 0;
      int NOT_VISITED = L0 - 1;
      bfsArray.attachToGraph(&g, (int)NOT_VISITED);
      for (int c = 0; c < g.num_nodes(); c ++) 
      {
        if (c < nc )
        {
          if (cmatch.getValue(c) == -1 )
          {
            bfsArray.setValue(c,L0);
          }
        }
      }


      predeccesor.attachToGraph(&g, (int)-1);
      int bfsLevel = L0;
      bool noNewVertices = false;
      while ( !noNewVertices )
      {
        for (int col_vertex = 0; col_vertex < g.num_nodes(); col_vertex ++) 
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


        bfsLevel = bfsLevel + 1;
        noNewVertices = modified.aggregateValue(NOT);
      }
      compress.attachToGraph(&g, (bool)false);
      compress_next.attachToGraph(&g, (bool)false);
      for (int r = 0; r < g.num_nodes(); r ++) 
      {
        if (r >= nc && rmatch.getValue(r) == -2 )
        {
          compress.setValue(r,true);
        }
      }


      bool compressed = false;
      while ( !compressed )
      {
        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
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
                  compressed = false;
                }
              }
            }
          }
        }


        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
        {
          if (row_vertex >= nc )
          {
            compress.setValue(row_vertex,compress_next.getValue(row_vertex));
            compress_next.setValue(row_vertex,false);
          }
        }


        compressed = modified.aggregateValue(NOT);
      }
      for (int r = 0; r < g.num_nodes(); r ++) 
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


      noNewPaths = modified.aggregateValue(NOT);
    }
  }
  world.barrier();
  int noNewPaths_leader_rank_temp = noNewPaths_leader_rank;
  MPI_Allreduce(&noNewPaths_leader_rank_temp,&noNewPaths_leader_rank,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
  MPI_Bcast(&noNewPaths,1,MPI_C_BOOL,noNewPaths_leader_rank,MPI_COMM_WORLD);




  int totalMatching = 0;
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




  totalMatching = previouslyMatched + totalMatching;
  return totalMatching;

}
void DynAPFB(Graph& g, int nc, Updates * updateBatch, int batchSize
  , boost::mpi::communicator world )
{
  NodeProperty<int> rmatch;
  NodeProperty<int> cmatch;
  int matchCount = APFB(g,nc,rmatch,cmatch, world);
  updateBatch->splitIntoSmallerBatches(batchSize);
  while(updateBatch->nextBatch())
  {
    EdgeProperty<bool> modified_del;
    modified_del.attachToGraph(&g, (bool)false);
    Updates * deleteBatch = updateBatch->getCurrentDeleteBatch();
    Updates * addBatch = updateBatch->getCurrentAddBatch();
    for(Update u : updateBatch->getCurrentDeleteBatch()->getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      int _t1 = 0 ;

      for (int nbr : g.getNeighbors(src)) 
      {
        nbr = g.get_other_vertex(src, _t1);
        Edge e = g.get_edge_i(src, _t1);
        if (nbr == dest )
        {
          modified_del.setValue(e,true);
        }
        _t1++;
      }



    }
    matchCount = dynamicAPFB_testing_add(g,matchCount,modified_del,addBatch,rmatch,cmatch, world);
    updateBatch->updateCsrDel(&g);

    updateBatch->updateCsrAdd(&g);

    EdgeProperty<bool> modified_add;
    modified_add.attachToGraph(&g, (bool)false);
    for(Update u : updateBatch->getCurrentAddBatch()->getUpdates())
    {
      int src = u.source;
      int dest = u.destination;
      int _t1 = 0 ;

      for (int nbr : g.getNeighbors(src)) 
      {
        nbr = g.get_other_vertex(src, _t1);
        Edge e = g.get_edge_i(src, _t1);
        if (nbr == dest )
        {
          modified_add.setValue(e,true);
        }
        _t1++;
      }



    }
    matchCount = dynamicAPFB_testing_del(g,matchCount,modified_add,deleteBatch,rmatch,cmatch, world);

  }

}
