#ifndef GENCPP_APFB_TEST_H
#define GENCPP_APFB_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

auto APFB(Graph& g, int nc, boost::mpi::communicator world );

#endif
