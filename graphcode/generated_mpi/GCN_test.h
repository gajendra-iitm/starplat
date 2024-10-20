#ifndef GENCPP_GCN_TEST_H
#define GENCPP_GCN_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

using namespace std;
env environment("mpi", "static", "GCN_test");
void GCN(Graph& g, Container<int>& neuronsPerHiddenLayer, boost::mpi::communicator world );

#endif
