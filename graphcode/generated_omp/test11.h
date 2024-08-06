#ifndef GENCPP_TEST_H
#define GENCPP_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <atomic>
#include <omp.h>
#include "../graph.hpp"
#include "../atomicUtil.h"

using namespace std;
// void test1(graph& g , float* features , int* labels);

env environment("omp", "static", "test.cc");

#endif
