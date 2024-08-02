#ifndef GENCPP_PAGERANKDSLV3_H
#define GENCPP_PAGERANKDSLV3_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

void ComputePageRank(graph& g , float beta , float delta , int maxIter , 
  float* pageRank);

#endif
