#ifndef GENCPP_TEST_H
#define GENCPP_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

using namespace std;
void test1(Graph& g, NodeProperty<float>& features, NodeProperty<int>& labels, boost::mpi::communicator world );

#endif
