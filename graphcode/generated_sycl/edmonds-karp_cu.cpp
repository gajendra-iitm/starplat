#ifndef GENCPP_EDMONDS-KARP_CU_H
#define GENCPP_EDMONDS-KARP_CU_H
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

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < E; i += stride) d_cap[i] = (int)weight[t];});
}).wait();

do{
  new_flow = 0;
  int* d_n_flow;
  d_n_flow=malloc_device<int>(V, Q);

  int* d_par;
  d_par=malloc_device<int>(V, Q);

  bool* d_in_bfs;
  d_in_bfs=malloc_device<bool>(V, Q);

  d_in_bfs[s] = true;
  d_par[s] = -2;
  terminate = false;

  //EXTRA vars for ITBFS AND REVBFS
  bool finished;
  int hops_from_source=0;
  bool* d_finished;       d_finished = malloc_device<bool>(1, Q);
  int* d_hops_from_source; d_hops_from_source = malloc_device<int>(1, Q);
  Q.submit([&](handler &h){ h.single_task([=](){ *d_hops_from_source = 0; }); }).wait();
  int* d_level;           d_level = malloc_device<int>(V, Q);

  //EXTRA vars INITIALIZATION
  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < V; i += stride) d_level[i] = (int)-1;
  }); }).wait();
Q.submit([&](handler &h){ h.single_task([=](){
  d_level[s] = 0;
}); }).wait();

// long k =0 ;// For DEBUG
do {
finished = true;
Q.submit([&](handler &h)
  { h.memcpy(d_finished, &finished, sizeof(bool)*(1)); })
.wait();

//Kernel LAUNCH
Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){
  if(d_level[v] == *d_hops_from_source) {
    if (d_in_bfs[cur] == true && terminate == false){ // if filter begin 
      // for all else part
      for (int edge = d_meta[cur]; edge < d_meta[cur+1]; edge++) { // FOR NBR ITR 
        int nbr = d_data[edge];
        int e = edge;
        if (d_par[nbr] == -1 && d_cap[e] > 0){ // if filter begin 
          d_par[nbr] = cur;
          cnew_flow = d_n_flow[cur];
          if (cnew_flow > d_cap[e]){ // if filter begin 
            cnew_flow = d_cap[e];

          } // if filter end
          if (nbr == d){ // if filter begin 
            new_flow = cnew_flow;
            terminate = true;

          } // if filter end
          d_in_bfs[nbr] = true;
          d_n_flow[nbr] = cnew_flow;

        } // if filter end

      } //  end FOR NBR ITR. TMP FIX!

    } // if filter end
  } // end if d lvl
}});}).wait(); // kernel end

Q.submit([&](handler &h){ h.single_task([=](){ *d_hops_from_source += 1; }); }).wait();
++hops_from_source; // updating the level to process in the next iteration

Q.submit([&](handler &h)
{ h.memcpy(&finished, d_finished, sizeof(bool)*(1)); })
.wait();
}while(!finished);

hops_from_source--;
Q.submit([&](handler &h)
{ h.memcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1)); })
.wait();

//BACKWARD PASS
while(hops_from_source > 1) {

//KERNEL Launch

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){
if(d_level[v] == *d_hops_from_source-1) {
} // end if d lvl
 }});}).wait(); // kernel end

hops_from_source--;
Q.submit([&](handler &h)
{ h.memcpy(d_hops_from_source, &hops_from_source, sizeof(int)*(1)); })
.wait();
}
flow = flow + new_flow;
cur = d;

//free up!! all propVars in this BLOCK!
free(d_in_bfs, Q);
free(d_par, Q);
free(d_n_flow, Q);

}while(new_flow != 0);

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
