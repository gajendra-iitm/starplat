#ifndef GENCPP_APFB_MATCHING_TEST_H
#define GENCPP_APFB_MATCHING_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto mod(int a, int b, boost::mpi::communicator world );
auto APFB(Graph& g, int nc, boost::mpi::communicator world );

#endif
