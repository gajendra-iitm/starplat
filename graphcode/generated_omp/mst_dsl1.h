#ifndef GENCPP_MST_DSL1_H
#define GENCPP_MST_DSL1_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void Boruvka(graph& g);

#endif
