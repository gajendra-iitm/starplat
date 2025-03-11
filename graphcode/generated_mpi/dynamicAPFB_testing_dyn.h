#ifndef GENCPP_DYNAMICAPFB_TESTING_H
#define GENCPP_DYNAMICAPFB_TESTING_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

#include"../mpi_header/updates.h"

auto mod(int a, int b, boost::mpi::communicator world );
auto APFB(Graph& g, int nc, NodeProperty<int>& rmatch, NodeProperty<int>& cmatch
  , boost::mpi::communicator world );
auto dynamicAPFB_testing_del(Graph& g, int previouslyMatched, EdgeProperty<bool>& modified, Updates * deleteBatch, 
  NodeProperty<int>& rmatch, NodeProperty<int>& cmatch, boost::mpi::communicator world );
auto dynamicAPFB_testing_add(Graph& g, int previouslyMatched, EdgeProperty<bool>& modified, Updates * addBatch, 
  NodeProperty<int>& rmatch, NodeProperty<int>& cmatch, boost::mpi::communicator world );
void DynAPFB(Graph& g, int nc, Updates * updateBatch, int batchSize
  , boost::mpi::communicator world );

#endif
