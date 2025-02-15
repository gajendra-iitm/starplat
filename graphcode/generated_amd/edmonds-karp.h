#ifndef GENCPP_EDMONDS-KARP_H
#define GENCPP_EDMONDS-KARP_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void ek(graph& g,int s,int d,int * weight
);

#endif
