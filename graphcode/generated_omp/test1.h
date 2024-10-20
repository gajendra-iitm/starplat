#ifndef GENCPP_TEST1_H
#define GENCPP_TEST1_H
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.cpp"
#include"../atomicUtil.h"

using namespace std;
env environment("omp", "static", "test1");
void string_test(string s);

#endif
