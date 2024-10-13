#ifndef GENCPP_PARAMBYREF_TEST_H
#define GENCPP_PARAMBYREF_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <CL/cl.h>
#include "../graph.hpp"

void test(double &arg1,int &arg2);

#endif
