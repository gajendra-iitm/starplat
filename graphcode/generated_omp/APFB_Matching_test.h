#ifndef GENCPP_APFB_MATCHING_TEST_H
#define GENCPP_APFB_MATCHING_TEST_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"
#include"../atomicUtil.h"

auto mod(int a , int b);
auto APFB(graph& g , int nc);

#endif
