#ifndef GENCPP_TEST_ABHIRUP_H
#define GENCPP_TEST_ABHIRUP_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto compute_static_TC(Graph& g, boost::mpi::communicator world );

#endif
