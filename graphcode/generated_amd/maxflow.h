#ifndef GENCPP_MAXFLOW_H
#define GENCPP_MAXFLOW_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void maxflow(graph& g,int s,int t);

#endif
