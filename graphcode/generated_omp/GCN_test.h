#ifndef GENCPP_GCN_TEST_H
#define GENCPP_GCN_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.cpp"
#include"../atomicUtil.h"

using namespace std;
env environment("omp", "static", "GCN_test");
void GCN(graph& g , GNN  gnn , vector<int> neuronsPerHiddenLayer);

#endif
