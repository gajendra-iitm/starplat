#ifndef GENCPP_TEST1_H
#define GENCPP_TEST1_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<set>
#include<vector>
#include"../mpi_header/graph_mpi.h"

using namespace std;
void string_test(string s, boost::mpi::communicator world );

#endif
