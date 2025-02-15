#ifndef GENCPP_PUSH_RELABEL_H
#define GENCPP_PUSH_RELABEL_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void do_max_flow(graph& g,int source,int sink,int kernel_parameter
);

#endif
