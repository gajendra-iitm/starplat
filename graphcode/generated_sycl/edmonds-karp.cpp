#ifndef GENCPP_EDMONDS-KARP_H
#define GENCPP_EDMONDS-KARP_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void ek(graph& g,int s,int d,int* weight
)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;
  bool* d_modified_next;

  d_meta=malloc_device<int>((1+V), Q);
  d_data=malloc_device<int>((E), Q);
  d_src=malloc_device<int>((E), Q);
  d_weight=malloc_device<int>((E), Q);
  d_rev_meta=malloc_device<int>((V+1), Q);
  d_modified_next=malloc_device<bool>((V), Q);

  Q.submit([&](handler &h)
    { h.memcpy(  d_meta,   h_meta, sizeof(int)*(V+1)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(  d_data,   h_data, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(   d_src,    h_src, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_weight, h_weight, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1))); })
  .wait();

  // CSR END
  //LAUNCH CONFIG
  int NUM_THREADS = 1048576;
  int stride = NUM_THREADS;

  // TIMER START
  std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

  //DECLAR DEVICE AND HOST vars in params
  int* d_weight;
  d_weight=malloc_device<int>(V, Q);


  //BEGIN DSL PARSING 
  int flow = 0; // asst in main

  int curint prevint new_flow = -1; // asst in main

  int cnew_flow = 0; // asst in main

  int e_cap = 0; // asst in main

  bool terminate = false; // asst in main

  int* d_cap;
  d_cap=malloc_device<int>(E, Q);


  //free up!! all propVars in this BLOCK!
  free(d_cap, Q);

  //TIMER STOP
  std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
  std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

  Q.submit([&](handler &h)
    { h.memcpy(  weight, d_weight, sizeof(int)*(V)); })
  .wait();
} //end FUN

#endif
